/*****************************************************************************
 * libmpegts.c :
 *****************************************************************************
 * Copyright (C) 2010 Kieran Kunhya
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#include "common.h"
#include "codecs.h"
#include "atsc/atsc.h"
#include "cablelabs/cablelabs.h"
#include "dvb/dvb.h"
#include "hdmv/hdmv.h"
#include "isdb/isdb.h"
#include "crc/crc.h"
#include <math.h>

static int steam_type_table[23][2] =
{
    { LIBMPEGTS_VIDEO_MPEG2, VIDEO_MPEG2 },
    { LIBMPEGTS_VIDEO_H264,  VIDEO_H264 },
    { LIBMPEGTS_AUDIO_MPEG1, AUDIO_MPEG1 },
    { LIBMPEGTS_AUDIO_MPEG2, AUDIO_MPEG2 },
    { LIBMPEGTS_AUDIO_ADTS,  AUDIO_ADTS },
    { LIBMPEGTS_AUDIO_LATM,  AUDIO_LATM },
    { LIBMPEGTS_AUDIO_AC3,   AUDIO_AC3 },    /* ATSC/Generic */
    { LIBMPEGTS_AUDIO_AC3,   PRIVATE_DATA }, /* DVB */
    { LIBMPEGTS_AUDIO_EAC3,  AUDIO_EAC3 },   /* ATSC/Generic */
    { LIBMPEGTS_AUDIO_EAC3,  PRIVATE_DATA }, /* DVB */
    { LIBMPEGTS_AUDIO_LPCM,  AUDIO_LPCM },
    { LIBMPEGTS_AUDIO_DTS,   AUDIO_DTS },
    { LIBMPEGTS_AUDIO_DOLBY_LOSSLESS,      AUDIO_DOLBY_LOSSLESS },
    { LIBMPEGTS_AUDIO_DTS_HD,              AUDIO_DTS_HD },
    { LIBMPEGTS_AUDIO_DTS_HD_XLL,          AUDIO_DTS_HD_XLL },
    { LIBMPEGTS_AUDIO_EAC3_SECONDARY,      AUDIO_EAC3_SECONDARY },
    { LIBMPEGTS_AUDIO_DTS_HD_SECONDARY,    AUDIO_DTS_HD_SECONDARY },
    { LIBMPEGTS_SUB_PRESENTATION_GRAPHICS, SUB_PRESENTATION_GRAPHICS },
    { LIBMPEGTS_SUB_INTERACTIVE_GRAPHICS,  SUB_INTERACTIVE_GRAPHICS },
    { LIBMPEGTS_SUB_TEXT,    SUB_TEXT },
    { LIBMPEGTS_AUDIO_302M,  PRIVATE_DATA },
    { LIBMPEGTS_SUB_DVB,     PRIVATE_DATA },
    { 0 },
};

/* Descriptors */
static void write_smoothing_buffer_descriptor( bs_t *s, ts_int_program_t *program );
static void write_video_stream_descriptor( bs_t *s, ts_int_stream_t *stream );
static void write_avc_descriptor( bs_t *s, ts_int_stream_t *stream );
static void write_data_stream_alignment_descriptor( bs_t *s );
static void write_timestamp( bs_t *s, uint64_t timestamp );
static void write_null_packet( ts_writer_t *w );

ts_writer_t *ts_create_writer( void )
{
    ts_writer_t *w = calloc( 1, sizeof(*w) );

    if( !w )
    {
        fprintf( stderr, "Malloc failed\n" );
        return -1;
    }

    return w;
}
int ts_setup_302m_stream( ts_writer_t *w, int pid, int bit_depth, int num_channels )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "SMPTE 302M not allowed in Blu-Ray\n" );
        return -1;
    }
    else if( !(bit_depth == 16 || bit_depth == 20 || bit_depth == 24) )
    {
        fprintf( stderr, "Invalid Bit Depth for SMPTE 302M\n" );
        return -1;
    }
    else if( (num_channels & 1) || num_channels <= 0 || num_channels > 8 )
    {
        fprintf( stderr, "Invalid number of channels for SMPTE 302M\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    stream->lpcm_ctx->bits_per_sample = bit_depth;
    stream->lpcm_ctx->num_channels = num_channels;

    stream->main_b.buf_size = SMPTE_302M_AUDIO_BS;

    /* 302M frame size is bit_depth / 4 + 1 */
    stream->rx = 1.2 * ((bit_depth >> 2) + 1) * SMPTE_302M_AUDIO_SR * 8;

    return 0;
}
int write_padding( bs_t *s, uint64_t start )
{
    bs_flush( s );
    uint8_t *p_start = s->p_start;
    int padding_bytes = TS_PACKET_SIZE - (bs_pos( s ) - start) / 8;

    memset( s->p, 0xff, padding_bytes );
    s->p += padding_bytes;

    bs_init( s, s->p, s->p_end - s->p );
    s->p_start = p_start;

    return padding_bytes;
}

void write_bytes( bs_t *s, uint8_t *bytes, int length )
{
    bs_flush( s );
    uint8_t *p_start = s->p_start;

    memcpy( s->p, bytes, length );
    s->p += length;

    bs_init( s, s->p, s->p_end - s->p );
    s->p_start = p_start;
}

/**** Descriptors ****/
/* Registration Descriptor */
void write_registration_descriptor( bs_t *s, int descriptor_tag, int descriptor_length, char *format_id )
{
    bs_write( s, 8, descriptor_tag );    // descriptor_tag
    bs_write( s, 8, descriptor_length ); // descriptor_length
    while( *format_id != '\0' )
        bs_write( s, 8, *format_id++ );  // format_identifier
}

/* First loop of PMT Descriptors */
static void write_smoothing_buffer_descriptor( bs_t *s, ts_int_program_t *program )
{
    bs_write( s, 8, SMOOTHING_BUFFER_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x4 );                    // descriptor_length

    bs_write( s, 2, 0x3 );                    // reserved
    bs_write( s, 22, program->sb_leak_rate ); // sb_leak_rate
    bs_write( s, 2, 0x3 );                    // reserved
    bs_write( s, 22, program->sb_size );      // sb_size
}

/* Second loop of PMT Descriptors */
static void write_video_stream_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, VIDEO_STREAM_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x04 );                        // descriptor_length

    bs_write1( s, 0 );   // multiple_frame_rate_flag
    bs_write( s, 4, 0 ); // frame_rate_code FIXME
    bs_write1( s, 0 );   // MPEG_1_only_flag
    bs_write1( s, 0 );   // constrained_parameter_flag
    bs_write1( s, 0 );   // still_picture_flag
    bs_write( s, 8, 0 ); // profile_and_level_indication FIXME
    bs_write( s, 2, 0 ); // chroma_format FIXME
    bs_write1( s, 0 );   // frame_rate_extension_flag FIXME
    bs_write( s, 5, 0x1f ); // reserved
}

static void write_avc_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, AVC_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x04 );               // descriptor_length

    bs_write( s, 8, stream->mpegvideo_ctx->profile & 0xff ); // profile_idc

    bs_write1( s, stream->mpegvideo_ctx->profile == H264_BASELINE ); // constraint_set0_flag
    bs_write1( s, stream->mpegvideo_ctx->profile <= H264_MAIN );     // constraint_set1_flag
    bs_write1( s, 0 );                                               // constraint_set2_flag
        if( stream->mpegvideo_ctx->level == 9 && stream->mpegvideo_ctx->profile <= H264_MAIN ) // level 1b
            bs_write1( s, 1 );                                           // constraint_set3_flag
        else if( stream->mpegvideo_ctx->profile == H264_HIGH_10_INTRA   ||
                 stream->mpegvideo_ctx->profile == H264_CAVLC_444_INTRA ||
                 stream->mpegvideo_ctx->profile == H264_HIGH_444_INTRA )
            bs_write1( s, 1 );                                           // constraint_set3_flag
        else
            bs_write1( s, 0 );                                           // constraint_set3_flag
    bs_write1( s, 0 );                                               // constraint_set4_flag
    bs_write1( s, 0 );                                               // constraint_set5_flag

    bs_write( s, 2, 0 );    // reserved
    bs_write( s, 8, stream->mpegvideo_ctx->level & 0xff ); // level_idc
    bs_write( s, 1, 0 );    // AVC_still_present
    bs_write( s, 1, 0 );    // AVC_24_hour_picture_flag
    bs_write( s, 6, 0x3f ); // reserved
}
static void write_iso_lang_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, ISO_693_LANGUAGE_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x04 ); // descriptor_length
    for( int i = 0; i < 3; i++ )
        bs_write( s, 8, stream->lang_code[i] );

    bs_write(s, 8, 0 ); // audio_type
}
static void write_timestamp( bs_t *s, uint64_t timestamp )
{
    bs_write( s, 3, (timestamp >> 30) & 0x07 ); // timestamp [32..30]
    bs_write1( s, 1 );                          // marker_bit
    bs_write( s, 8, (timestamp >> 22) & 0xff ); // timestamp [29..15]
    bs_write( s, 7, (timestamp >> 15) & 0x7f ); // timestamp [29..15]
    bs_write1( s, 1 );                          // marker_bit
    bs_write( s, 8, (timestamp >> 7) & 0xff );  // timestamp [14..0]
    bs_write( s, 7, timestamp & 0x7f );         // timestamp [14..0]
    bs_write1( s, 1 );                          // marker_bit
}

void write_crc( bs_t *s, uint64_t start )
{
    uint8_t *p_start = s->p_start;
    int pos = (bs_pos( s ) - start) / 8;
    uint32_t crc = crc_32( s->p - pos, pos );

    bs_init( s, s->p, s->p_end - s->p );
    s->p_start = p_start;

    bs_write32( s, crc );
}
static void write_null_packet( ts_writer_t *w )
{
    uint64_t start;
    int cc = 0;

    bs_t *s = &w->out.bs;
    start = bs_pos( s );

    write_packet_header( w, 0, NULL_PID, PAYLOAD_ONLY, &cc );
    write_padding( s, start );

    increase_pcr( w, 1 );
}

ts_int_stream_t *find_stream( ts_writer_t *w, int pid )
{
    for( int i = 0; i < w->programs[0]->num_streams; i++ )
    {
        if( pid == w->programs[0]->streams[i]->pid )
            return w->programs[0]->streams[i];
    }
    return NULL;
}
