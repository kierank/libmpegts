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
#include "smpte/smpte.h"
#include "crc/crc.h"
#include <math.h>

static const int steam_type_table[27][2] =
{
    { LIBMPEGTS_VIDEO_MPEG2, VIDEO_MPEG2 },
    { LIBMPEGTS_VIDEO_AVC,   VIDEO_AVC },
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
    { LIBMPEGTS_DVB_SUB,     PRIVATE_DATA },
    { LIBMPEGTS_DVB_TELETEXT,    PRIVATE_DATA },
    { LIBMPEGTS_DVB_VBI,         PRIVATE_DATA },
    { LIBMPEGTS_ANCILLARY_RDD11, PRIVATE_DATA },
    { LIBMPEGTS_ANCILLARY_2038,  PRIVATE_DATA },
    { 0 },
};

/**** Descriptors ****/
/** MPEG-2 Systems Descriptors **/
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
    bs_write( s, 8, 0x6 );                    // descriptor_length

    bs_write( s, 2, 0x3 );                    // reserved
    bs_write( s, 22, program->sb_leak_rate ); // sb_leak_rate
    bs_write( s, 2, 0x3 );                    // reserved
    bs_write( s, 22, program->sb_size );      // sb_size
}

/* Second loop of PMT Descriptors */
#if 0
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
#endif
static void write_avc_descriptor( bs_t *s, ts_int_program_t *program, ts_int_stream_t *stream )
{
    bs_write( s, 8, AVC_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x04 );               // descriptor_length

    bs_write( s, 8, avc_profiles[stream->mpegvideo_ctx->profile] ); // profile_idc

    bs_write1( s, stream->mpegvideo_ctx->profile == AVC_BASELINE ); // constraint_set0_flag
    bs_write1( s, stream->mpegvideo_ctx->profile <= AVC_MAIN );     // constraint_set1_flag
    bs_write1( s, 0 );                                              // constraint_set2_flag
        if( stream->mpegvideo_ctx->level == 9 && stream->mpegvideo_ctx->profile <= AVC_MAIN ) // level 1b
            bs_write1( s, 1 );                                           // constraint_set3_flag
        else if( stream->mpegvideo_ctx->profile == AVC_HIGH_10_INTRA   ||
                 stream->mpegvideo_ctx->profile == AVC_CAVLC_444_INTRA ||
                 stream->mpegvideo_ctx->profile == AVC_HIGH_444_INTRA )
            bs_write1( s, 1 );                                       // constraint_set3_flag
        else
            bs_write1( s, 0 );                                       // constraint_set3_flag
    bs_write1( s, 0 );                                               // constraint_set4_flag
    bs_write1( s, 0 );                                               // constraint_set5_flag

    bs_write( s, 2, 0 );    // reserved
    bs_write( s, 8, stream->mpegvideo_ctx->level & 0xff ); // level_idc
    bs_write( s, 1, 0 );    // AVC_still_present
    bs_write( s, 1, 0 );    // AVC_24_hour_picture_flag
    bs_write( s, 1, !program->is_3dtv ); // Frame_Packing_SEI_not_present_flag
    bs_write( s, 5, 0x1f ); // reserved
}

static void write_data_stream_alignment_descriptor( bs_t *s )
{
    bs_write( s, 8, DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 1 );               // descriptor_length
    // FIXME make UK-DTG compliant
    bs_write( s, 8, 1 );               // alignment_type
}

static void write_mpeg2_aac_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, MPEG2_AAC_AUDIO_DESCRIPTOR ); // descriptor_tag
    bs_write( s, 8, 0x3 );                        // descriptor_length
    bs_write( s, 8, stream->aac_profile );        // MPEG-2_AAC_profile
    bs_write( s, 8, stream->aac_channel_map );    // MPEG-2_AAC_channel_configuration
    bs_write( s, 8, 0 );                          // MPEG-2_AAC_additional_information (anybody use this?)
}

/* AC-3 Descriptor for DVB and Blu-Ray */
static void write_ac3_descriptor( ts_writer_t *w, bs_t *s, int e_ac3 )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
        bs_write( s, 8, HDMV_AC3_DESCRIPTOR_TAG ); // descriptor_tag
    else if( e_ac3 )
        bs_write( s, 8, DVB_EAC3_DESCRIPTOR_TAG ); // descriptor_tag
    else
        bs_write( s, 8, DVB_AC3_DESCRIPTOR_TAG );  // descriptor_tag

    bs_write( s, 8, 1 );        // descriptor_length

    /* does anything need these set? */
    bs_write1( s, 0 );          // component_type_flag
    bs_write1( s, 0 );          // bsid_flag
    bs_write1( s, 0 );          // mainid_flag
    bs_write1( s, 0 );          // asvc_flag

    if( e_ac3 )
    {
        bs_write1( s, 0 );      // mixinfoexists
        bs_write1( s, 0 );      // substream1_flag
        bs_write1( s, 0 );      // substream2_flag
        bs_write1( s, 0 );      // substream3_flag
    }
    else
        bs_write( s, 4, 0 );    // reserved
}

static void write_iso_lang_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, ISO_693_LANGUAGE_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x4 ); // descriptor_length
    for( int i = 0; i < 3; i++ )
        bs_write( s, 8, stream->lang_code[i] );

    bs_write(s, 8, stream->audio_type ); // audio_type
}

/**** PCR functions ****/
static int check_pcr( ts_writer_t *w, ts_int_program_t *program )
{
    // if the next packet written goes over the max pcr retransmit boundary, write the pcr in the next packet
    double next_pkt_pcr = ((w->packets_written * TS_PACKET_SIZE) + (TS_PACKET_SIZE + 7)) * 8.0 / w->ts_muxrate -
                          (double)program->last_pcr / TS_CLOCK;
    next_pkt_pcr += TS_START;

    if( next_pkt_pcr >= (double)w->pcr_period / 1000 )
    {
        return 1;
    }

    return 0;
}

static int64_t get_pcr_int( ts_writer_t *w, double offset )
{
    return (int64_t)((8.0 * (w->packets_written * TS_PACKET_SIZE + offset) / w->ts_muxrate) * TS_CLOCK + 0.5) + TS_START * TS_CLOCK;
}

static double get_pcr_double( ts_writer_t *w, double offset )
{
    return (8.0 * (w->packets_written * TS_PACKET_SIZE + offset) / w->ts_muxrate) + TS_START;
}

/**** Buffer management ****/
static void add_to_buffer( buffer_t *buffer )
{
    buffer->cur_buf += TS_PACKET_SIZE * 8;
}

static void drip_buffer( ts_writer_t *w, ts_int_program_t *program, int rx, buffer_t *buffer, double next_pcr )
{
    int iters;
    double offset;

    /* Although this uses floating point arithmetic, the values are backed by integers
     * Transport buffer fullness does not need to be exact */
    double cur_pcr = get_pcr_double( w, 0 );
    if( buffer->last_byte_removal_time == 0.0 )
    {
        buffer->last_byte_removal_time = cur_pcr;
        buffer->cur_buf -= 8;
    }

    iters = floor( (next_pcr - buffer->last_byte_removal_time) / (8.0 / rx) );

    buffer->cur_buf -= 8*iters;
    /* Avoid compounded error from floating point addition by making calculations relative to next_pcr */
    offset = next_pcr - (buffer->last_byte_removal_time + 8.0 * iters / rx);
    buffer->last_byte_removal_time = next_pcr - offset;

    buffer->cur_buf = MAX( buffer->cur_buf, 0 );
}

static int write_adaptation_field( ts_writer_t *w, bs_t *s, ts_int_program_t *program, ts_int_pes_t *pes,
                                   int write_pcr, int flags, int stuffing, int discontinuity )
{
    int private_data_flag, write_dvb_au, random_access, priority;
    int start = bs_pos( s );
    uint8_t temp[512], temp2[256];
    bs_t q, r;

    private_data_flag = write_dvb_au = random_access = priority = 0;

    if( pes && ( pes->data == pes->cur_pos ) )
    {
        ts_int_stream_t *stream = pes->stream;
        random_access = pes->random_access;
        if( IS_VIDEO( stream ) )
        {
            if( !write_pcr )
                random_access = 0;

            if( stream->dvb_au )
                private_data_flag = write_dvb_au = 1;
        }

        priority = pes->priority;
        pes->random_access = 0; /* don't write this flag again */
    }

    /* initialise temporary bitstream */
    bs_init( &q, temp, 256 );

    if( flags )
    {
        int64_t pcr = get_pcr_int( w, 7 ); /* 7 bytes until end of PCR field */
        bs_write1( &q, discontinuity ); // discontinuity_indicator
        bs_write1( &q, random_access ); // random_access_indicator
        bs_write1( &q, priority );  // elementary_stream_priority_indicator
        bs_write1( &q, write_pcr ); // PCR_flag
        bs_write1( &q, 0 ); // OPCR_flag
        bs_write1( &q, 0 ); // splicing_point_flag
        bs_write1( &q, private_data_flag ); // transport_private_data_flag
        bs_write1( &q, 0 ); // adaptation_field_extension_flag
        if( write_pcr )
        {
             uint64_t base, extension;
             int64_t mod = (int64_t)1 << 33;

             program->last_pcr = pcr;

             base = (pcr / 300) % mod;
             extension = pcr % 300;

             // program_clock_reference_base
             bs_write32( &q, base >> 1 );
             bs_write1( &q, (base & 1) );
             // reserved
             bs_write( &q, 6, 0x3f );
             // program_clock_reference_extension
             bs_write( &q, 8, (extension >> 1) & 0xff );
             bs_write1( &q, (extension & 1 ) );
        }
    }

    if( private_data_flag )
    {
        /* initialise another temporary bitstream */
        bs_init( &r, temp2, 128 );

        if( write_dvb_au )
            write_dvb_au_information( &r, pes );

        bs_flush ( &r );
        bs_write( &q, 8, bs_pos( &r ) >> 3 ); // transport_private_data_length
        write_bytes( &q, temp2, bs_pos( &r ) >> 3 );
    }

    for( int i = 0; i < stuffing; i++ )
        bs_write( &q, 8, 0xff );

    bs_flush( &q );
    bs_write( s, 8, bs_pos( &q ) >> 3 ); // adaptation_field_length
    write_bytes( s, temp, bs_pos( &q ) >> 3 );

    return (bs_pos( s ) - start) >> 3;
}

static int write_pcr_empty( ts_writer_t *w, ts_int_program_t *program, int first )
{
    bs_t *s = &w->out.bs;

    write_packet_header( w, s, 0, program->pcr_stream->pid, ADAPT_FIELD_ONLY, &program->pcr_stream->cc );
    int stuffing = 184 - 6 - 2; /* pcr, flags and length */
    write_adaptation_field( w, s, program, NULL, 1, 1, stuffing, first );

    add_to_buffer( &program->pcr_stream->tb );
    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    return 0;
}

/**** PSI ****/
static int write_pat( ts_writer_t *w )
{
    int start;
    bs_t *s = &w->out.bs;

    write_packet_header( w, s, 1, PAT_PID, PAYLOAD_ONLY, &w->pat_cc );
    bs_write( s, 8, 0 ); // pointer field

    start = bs_pos( s );
    bs_write( s, 8, PAT_TID ); // table_id
    bs_write1( s, 1 );      // section_syntax_indicator
    bs_write1( s, 0 );      // '0'
    bs_write( s, 2, 0x03 ); // reserved`

    // FIXME when multiple programs are allowed do this properly
    int section_length = w->num_programs * 4 + w->network_pid * 4 + 9;
    bs_write( s, 12, section_length & 0x3ff );

    bs_write( s, 16, w->ts_id & 0xffff ); // transport_stream_id
    bs_write( s, 2, 0x03 ); // reserved
    bs_write( s, 5, w->pat_version ); // version_number
    bs_write1( s, 1 );      // current_next_indicator
    bs_write( s, 8, 0 );    // section_number
    bs_write( s, 8, 0 );    // last_section_number

    if( w->network_pid )
    {
        bs_write( s, 16, 0 );   // program_number
        bs_write( s, 3, 0x07 ); // reserved
        bs_write( s, 13, w->network_pid & 0x1fff ); // network_PID
    }

    for( int i = 0; i < w->num_programs; i++ )
    {
        bs_write( s, 16, w->programs[i]->program_num & 0xffff ); // program_number
        bs_write( s, 3, 0x07 ); // reserved
        bs_write( s, 13, w->programs[i]->pmt.pid & 0x1fff ); // program_map_PID
    }

    bs_flush( s );
    write_crc( s, start );

    // -40 to include header and pointer field
    write_padding( s, start - 40 );
    add_to_buffer( &w->tb );
    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    return 0;
}

static int eject_queued_pmt( ts_writer_t *w, ts_int_program_t *program, bs_t *s )
{
    uint8_t **temp;

    write_bytes( s, program->pmt_packets[0], TS_PACKET_SIZE );

    if( program->num_queued_pmt > 1 )
        memmove( &program->pmt_packets[0], &program->pmt_packets[1], (program->num_queued_pmt-1) * sizeof(uint8_t*) );

    temp = realloc( program->pmt_packets, (program->num_queued_pmt-1) * sizeof(uint8_t*) );
    if( !temp )
    {
        fprintf( stderr, "malloc failed\n" );
        return -1;
    }
    program->pmt_packets = temp;

    program->num_queued_pmt--;

    add_to_buffer( &w->tb );
    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    return 0;
};

static int write_pmt( ts_writer_t *w, ts_int_program_t *program )
{
    int start;
    bs_t *s = &w->out.bs;

    uint8_t pmt_buf[2048] = {0}, temp[2048] = {0}, temp1[2048] = {0};
    bs_t o, p, q;
    int section_length;
    uint8_t **temp2;

    /* this should never happen */
    if( program->num_queued_pmt )
        return eject_queued_pmt( w, program, s );

    start = bs_pos( s );
    write_packet_header( w, s, 1, program->pmt.pid, PAYLOAD_ONLY, &program->pmt.cc );

    bs_write( s, 8, 0 );       // pointer field

    bs_init( &o, pmt_buf, 2048 );

    bs_write( &o, 8, PMT_TID ); // table_id = program_map_section
    bs_write1( &o, 1 );         // section_syntax_indicator
    bs_write1( &o, 0 );         // '0'
    bs_write( &o, 2, 0x3 );     // reserved

    bs_init( &p, temp, 2048 );

    bs_write( &p, 16, program->program_num & 0xffff ); // program_number
    bs_write( &p, 2, 0x3 );  // reserved
    bs_write( &p, 5, program->pmt_version ); // version_number
    bs_write1( &p, 1 );      // current_next_indicator
    bs_write( &p, 8, 0 );    // section_number
    bs_write( &p, 8, 0 );    // last_section_number
    bs_write( &p, 3, 0x7 );  // reserved

    bs_write( &p, 13, program->pcr_stream[0].pid & 0x1fff ); // PCR PID
    bs_write( &p, 4, 0xf );  // reserved

    /* setup temporary bitstream context */
    bs_init( &q, temp1, 2048 );

    if( w->ts_type == TS_TYPE_ATSC )
    {
        write_registration_descriptor( &q, REGISTRATION_DESCRIPTOR_TAG, 4, "GA94" );
        write_smoothing_buffer_descriptor( &q, program );
    }
    else if( w->ts_type == TS_TYPE_CABLELABS )
    {
        write_registration_descriptor( &q, REGISTRATION_DESCRIPTOR_TAG, 4, "SCTE" );
        if( program->is_3dtv )
            write_cablelabs_3d_descriptor( &q );
    }
    else if( w->ts_type == TS_TYPE_BLU_RAY )
        write_registration_descriptor( &q, REGISTRATION_DESCRIPTOR_TAG, 4, "HDMV" );

    /* Optional descriptor(s) here */

    bs_flush( &q );
    bs_write( &p, 12, bs_pos( &q ) >> 3 );   // program_info_length
    write_bytes( &p, temp1, bs_pos( &q ) >> 3 );

    for( int i = 0; i < program->num_streams; i++ )
    {
         ts_int_stream_t *stream = program->streams[i];

         bs_write( &p, 8, stream->stream_type & 0xff ); // stream_type
         bs_write( &p, 3, 0x7 );  // reserved
         bs_write( &p, 13, stream->pid & 0x1fff ); // elementary_PID
         bs_write( &p, 4, 0xf );  // reserved

         /* reset temporary bitstream context for streams loop */
         bs_init( &q, temp1, 512 );

         if( stream->stream_format != LIBMPEGTS_ANCILLARY_RDD11 )
             write_data_stream_alignment_descriptor( &q );

         if( stream->dvb_au )
         {
             if( w->ts_type == TS_TYPE_DVB )
                 write_adaptation_field_data_descriptor( &q, AU_INFORMATION_DATA_FIELD );
             else if( w->ts_type == TS_TYPE_CABLELABS )
                 write_scte_adaptation_descriptor( &q );
         }

         if( stream->write_lang_code )
             write_iso_lang_descriptor( &q, stream );

         if( stream->has_stream_identifier )
             write_stream_identifier_descriptor( &q, stream->stream_identifier );

         if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
         {
             if( w->ts_type == TS_TYPE_BLU_RAY )
                 write_hdmv_video_registration_descriptor( &q, stream );
         }
         else if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
         {
             write_avc_descriptor( &q, program, stream );
             if( w->ts_type == TS_TYPE_BLU_RAY )
                 write_hdmv_video_registration_descriptor( &q, stream );
         }
         else if( stream->stream_format == LIBMPEGTS_AUDIO_MPEG1 ||
                  stream->stream_format == LIBMPEGTS_AUDIO_MPEG2 )
         {
             // TODO
         }
         else if( stream->stream_format == LIBMPEGTS_AUDIO_ADTS || stream->stream_format == LIBMPEGTS_AUDIO_LATM )
         {
             /* strictly speaking in DVB only LATM is allowed for MPEG-4 AAC audio. ADTS is commonly used however */
             if( !stream->aac_is_mpeg4 )
                 write_mpeg2_aac_descriptor( &q, stream );
             else if( w->ts_type == TS_TYPE_DVB )
                 write_aac_descriptor( &q, stream );
         }
         else if( stream->stream_format == LIBMPEGTS_AUDIO_AC3 )
         {
             write_registration_descriptor( &q, REGISTRATION_DESCRIPTOR_TAG, 4, "AC-3" );
             if( stream->atsc_ac3_ctx && ( w->ts_type == TS_TYPE_ATSC || w->ts_type == TS_TYPE_CABLELABS ) )
                 write_atsc_ac3_descriptor( &q, stream->atsc_ac3_ctx );
             else
                 write_ac3_descriptor( w, &q, 0 );
         }
         else if( stream->stream_format == LIBMPEGTS_AUDIO_EAC3 || stream->stream_format == LIBMPEGTS_AUDIO_EAC3_SECONDARY )
             write_ac3_descriptor( w, &q, 1 );
         else if( stream->stream_format == LIBMPEGTS_AUDIO_DTS )
         {
             // TODO
         }
         else if( stream->stream_format == LIBMPEGTS_AUDIO_302M )
             write_registration_descriptor( &q, PRIVATE_DATA_DESCRIPTOR_TAG, 4, "BSSD" );
         else if( stream->stream_format == LIBMPEGTS_DVB_SUB )
             write_dvb_subtitling_descriptor( &q, stream );
         else if( stream->stream_format == LIBMPEGTS_DVB_TELETEXT )
             write_teletext_descriptor( &q, stream, 0 );
         else if( stream->stream_format == LIBMPEGTS_DVB_VBI )
         {
             write_vbi_descriptor( &q, stream );
             if( stream->num_dvb_ttx )
                 write_teletext_descriptor( &q, stream, 1 );
         }
         else if( stream->stream_format == LIBMPEGTS_ANCILLARY_RDD11 )
             write_registration_descriptor( &q, PRIVATE_DATA_DESCRIPTOR_TAG, 4, "LU-A" );
         else if( stream->stream_format == LIBMPEGTS_ANCILLARY_2038 )
         {
             write_registration_descriptor( &q, PRIVATE_DATA_DESCRIPTOR_TAG, 4, "VANC" );
             write_anc_data_descriptor( &q );
         }

         // TODO other stream_type descriptors

         /* Optional descriptor(s) here */

         bs_flush( &q );
         bs_write( &p, 12, bs_pos( &q ) >> 3 );   // ES_info_length
         write_bytes( &p, temp1, bs_pos( &q ) >> 3 );
    }

    /* section length includes crc */
    section_length = (bs_pos( &p ) >> 3) + 4;
    bs_write( &o, 12, section_length & 0x3ff );

    /* write main chunk into pmt array */
    bs_flush( &p );
    write_bytes( &o, temp, bs_pos( &p ) >> 3 );

    /* take crc of the whole program map section */
    bs_flush( &o );
    write_crc( &o, 0 );

    int length = bs_pos( &o ) >> 3;
    int bytes_left = TS_PACKET_SIZE - ((bs_pos( s ) - start) >> 3);

    bs_flush( &o );
    write_bytes( s, pmt_buf, MIN( bytes_left, length ) );
    bs_flush( s );

    write_padding( s, start );
    add_to_buffer( &w->tb );
    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    int pos = MIN( bytes_left, length );
    length -= pos;

    bytes_left = 184;

    /* queue up pmt packets for spaced output */
    while( length > 0 )
    {
        bs_t z;

        temp2 = realloc( program->pmt_packets, (program->num_queued_pmt + 1) * sizeof(uint8_t*));
        if( !temp2 )
        {
            fprintf( stderr, "malloc failed" );
            return -1;
        }
        program->pmt_packets = temp2;

        program->pmt_packets[program->num_queued_pmt] = malloc( TS_PACKET_SIZE );
        if( !program->pmt_packets[program->num_queued_pmt] )
        {
            fprintf( stderr, "malloc failed" );
            return -1;
        }

        bs_init( &z, program->pmt_packets[program->num_queued_pmt], 188 );

        write_packet_header( w, &z, 0, program->pmt.pid, PAYLOAD_ONLY, &program->pmt.cc );
        write_bytes( &z, &temp[pos], MIN( bytes_left, length ) );
        bs_flush( &z );
        write_padding( &z, 0 );
        pos += MIN( bytes_left, length );
        length -= MIN( bytes_left, length );
        program->num_queued_pmt++;
    }

    return 0;
}

static void retransmit_psi_and_si( ts_writer_t *w, ts_int_program_t *program, int first )
{
    // TODO make this work with multiple programs
    int64_t cur_pcr = get_pcr_int( w, 0 );
    if( cur_pcr - w->last_pat >= w->pat_period * 27000LL || first )
    {
        /* Although it is not in line with the mux strategy it is good practice to write PAT and PMT together */
        w->last_pat = cur_pcr;
        write_pat( w );          // FIXME handle failure
        write_pmt( w, program ); // FIXME handle failure
    }

    cur_pcr = get_pcr_int( w, 0 );

    if( w->sdt && ( cur_pcr - w->last_sdt >= w->sdt_period * 27000LL || first ) )
    {
        w->last_sdt = cur_pcr;
        write_sdt( w );
    }
}

/* DVB / Blu-Ray Service Information */
static int write_sit( ts_writer_t *w )
{
    int start;
    int len = 0; // FIXME
    bs_t *s = &w->out.bs;

    write_packet_header( w, s, 1, SIT_PID, PAYLOAD_ONLY, &w->sit->cc );
    bs_write( s, 8, 0 );       // pointer field

    start = bs_pos( s );
    bs_write( s, 8, SIT_TID ); // table_id
    bs_write1( s, 1 );         // section_syntax_indicator
    bs_write1( s, 1 );         // DVB_reserved_future_use
    bs_write( s, 2, 0x03 );    // ISO_reserved

    // FIXME do length properly

    bs_write( s, 12, 11 + len ); // section_length
    bs_write( s, 16, 0xffff ); // DVB_reserved_future_use
    bs_write( s, 2, 0x03 );    // ISO_reserved
    bs_write( s, 5, w->sit->version_number ); // version_number
    bs_write1( s, 1 );         // current_next_indicator
    bs_write( s, 8, 0 );       // section_number
    bs_write( s, 8, 0 );       // last_section_number
    bs_write( s, 4, 0x0f );    // DVB_reserved_for_future_use

    bs_write( s, 12, len );    // transmission_info_loop_length

    if( w->ts_type == TS_TYPE_BLU_RAY )
        write_partial_ts_descriptor( w, s );

    for( int i = 0; i < w->num_programs; i++ )
    {
        bs_write( s, 16, w->programs[i]->program_num & 0xffff ); // service_id (equivalent to program_number)
        bs_write1( s, 1 );      // DVB_reserved_future_use
        bs_write( s, 3, 0x07 ); // running_status
        bs_write( s, 12, 0 );   // service_loop_length
    }

    bs_flush( s );
    write_crc( s, start );

    // -40 to include header and pointer field
    write_padding( s, start - 40 );
    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    return 0;
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

static int write_pes( ts_writer_t *w, ts_int_program_t *program, ts_frame_t *in_frame, ts_int_pes_t *out_pes )
{
    bs_t s, q;
    uint8_t temp[1024];
    int header_size, total_size;
    int64_t mod = (int64_t)1 << 33;

    if( out_pes->dts > out_pes->pts )
        fprintf( stderr, "\nError: DTS > PTS\n" );

    bs_init( &s, out_pes->data, in_frame->size + 200 );

    ts_int_stream_t *stream = out_pes->stream;

    bs_write( &s, 24, 1 );   // packet_start_code_prefix
    bs_write( &s, 8, stream->stream_id ); // stream_id

    /* Initialise temp buffer */
    bs_init( &q, temp, 1024 );

    bs_write( &q, 2, 0x2 );  // '10'
    bs_write( &q, 2, 0 );    // PES_scrambling_control
    bs_write1( &q, 0 );      // PES_priority
    bs_write1( &q, stream->stream_format != LIBMPEGTS_ANCILLARY_RDD11 ); // data_alignment_indicator
    bs_write1( &q, 1 );      // copyright
    bs_write1( &q, 1 );      // original_or_copy

    int same_timestamps = out_pes->dts == out_pes->pts;

    bs_write( &q, 2, 0x02 + !same_timestamps ); // pts_dts_flags

    bs_write1( &q, 0 );      // ESCR_flag
    bs_write1( &q, 0 );      // ES_rate_flag
    bs_write1( &q, 0 );      // DSM_trick_mode_flag
    bs_write1( &q, 0 );      // additional_copy_info_flag
    bs_write1( &q, 0 );      // PES_CRC_flag
    bs_write1( &q, 0 );      // PES_extension_flag

    if( stream->stream_format == LIBMPEGTS_DVB_TELETEXT || stream->stream_format == LIBMPEGTS_DVB_VBI )
        bs_write( &q, 8, 0x24 ); // PES_header_data_length
    else if( same_timestamps )
        bs_write( &q, 8, 0x05 ); // PES_header_data_length (PTS only)
    else
        bs_write( &q, 8, 0x0a ); // PES_header_data_length (PTS and DTS)

    bs_write( &q, 4, 0x02 + !same_timestamps ); // '0010' or '0011'

    write_timestamp( &q, out_pes->pts % mod );  // PTS

    if( !same_timestamps )
    {
        bs_write( &q, 4, 1 );                      // '0001'
        write_timestamp( &q, out_pes->dts % mod ); // DTS
    }

    /* TTX and VBI require extra stuffing */
    if( stream->stream_format == LIBMPEGTS_DVB_TELETEXT || stream->stream_format == LIBMPEGTS_DVB_VBI )
    {
        /* Total PES header is 45 bytes but don't count 6 bytes for the startcode, stream_id and length */
        int num_stuffing = 45 - 6 - (bs_pos( &q ) >> 3);
        for( int i = 0; i < num_stuffing; i++ )
            bs_write( &q, 8, 0xff );
    }

    bs_flush( &q );
    total_size = in_frame->size + (bs_pos( &q ) >> 3);

    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 || stream->stream_format == LIBMPEGTS_VIDEO_AVC )
        bs_write( &s, 16, 0 );          // PES_packet_length
    else
        bs_write( &s, 16, total_size ); // PES_packet_length

    write_bytes( &s, temp, bs_pos( &q ) >> 3 );
    header_size = bs_pos( &s ) >> 3;
    write_bytes( &s, in_frame->data, in_frame->size );

    bs_flush( &s );

    out_pes->size = out_pes->bytes_left = bs_pos( &s ) >> 3;
    out_pes->cur_pos = out_pes->data;

    return header_size;
}

static int write_null_packet( ts_writer_t *w )
{
    int start;
    int cc = 0;

    bs_t *s = &w->out.bs;
    start = bs_pos( s );

    write_packet_header( w, s, 0, NULL_PID, PAYLOAD_ONLY, &cc );
    write_padding( s, start );

    if( increase_pcr( w, 1, 0 ) < 0 )
        return -1;

    return 0;
}

static int check_bitstream( ts_writer_t *w )
{
    if( w->out.bs.p_end - w->out.bs.p < 18800 )
    {
        bs_flush( &w->out.bs );
        uint8_t *bs_bak = w->out.p_bitstream;
        w->out.i_bitstream += 100000;
        uint8_t *temp2 = realloc( w->out.p_bitstream, w->out.i_bitstream );

        if( !temp2 )
        {
            fprintf( stderr, "realloc failed\n" );
            return -1;
        }
        w->out.p_bitstream = temp2;

        intptr_t delta = w->out.p_bitstream - bs_bak;

        w->out.bs.p_start += delta;
        w->out.bs.p += delta;
        w->out.bs.p_end = w->out.p_bitstream + w->out.i_bitstream;
        bs_realign( &w->out.bs );
    }

    return 0;
}

ts_writer_t *ts_create_writer( void )
{
    ts_writer_t *w = calloc( 1, sizeof(*w) );

    if( !w )
    {
        fprintf( stderr, "Malloc failed\n" );
        return NULL;
    }

    return w;
}

int ts_setup_transport_stream( ts_writer_t *w, ts_main_t *params )
{
    // TODO check for PID collisions, add MPTS support
    if( params->ts_type < TS_TYPE_GENERIC || params->ts_type > TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "Invalid Transport Stream type.\n" );
        return -1;
    }

    if( params->num_programs > 1 )
    {
        fprintf( stderr, "Multiple program transport streams are not yet supported.\n" );
        return -1;
    }

    if( !params->cbr && params->num_programs > 1 )
    {
        fprintf( stderr, "Multiple program transport streams cannot be variable bitrate.\n" );
        return -1;
    }

    if( params->network_pid && ( params->network_pid < 0x10 || params->network_pid == 0x1fff ) )
    {
        fprintf( stderr, "Invalid network_PID.\n" );
        return -1;
    }

    if( !params->muxrate )
    {
        fprintf( stderr, "Muxrate must be nonzero\n" );
        return -1;
    }

    BOOLIFY( params->cbr );
    BOOLIFY( params->legacy_constraints );

    int internal_pcr_pid, video_stream;
    internal_pcr_pid = video_stream = 0;
    ts_int_program_t *cur_program = calloc( 1, sizeof(*cur_program) );
    if( !cur_program )
    {
        fprintf( stderr, "Malloc failed\n" );
        return -1;
    }

    w->num_programs = 1;
    w->programs[0] = cur_program;

    cur_program->pmt.pid = params->programs[0].pmt_pid;
    cur_program->program_num = params->programs[0].program_num;

    cur_program->is_3dtv = params->programs[0].is_3dtv;
    cur_program->sb_leak_rate = params->programs[0].sb_leak_rate;
    cur_program->sb_size = params->programs[0].sb_size;
    cur_program->video_dts = -1;

    cur_program->sdt_ctx.service_type = params->programs[0].sdt.service_type;
    if( params->programs[0].sdt.service_name )
    {
        cur_program->sdt_ctx.service_name = malloc( strlen( params->programs[0].sdt.service_name ) + 1 );
        if( !cur_program->sdt_ctx.service_name )
        {
            fprintf( stderr, "Malloc failed\n" );
            return -1;
        }
        strcpy( cur_program->sdt_ctx.service_name, params->programs[0].sdt.service_name );
    }
    if( params->programs[0].sdt.provider_name )
    {
        cur_program->sdt_ctx.provider_name = malloc( strlen( params->programs[0].sdt.provider_name ) + 1 );
        if( !cur_program->sdt_ctx.provider_name )
        {
            fprintf( stderr, "Malloc failed\n" );
            return -1;
        }
        strcpy( cur_program->sdt_ctx.provider_name, params->programs[0].sdt.provider_name );
    }

    for( int i = 0; i < params->programs[0].num_streams; i++ )
    {
        ts_stream_t *stream_in = &params->programs[0].streams[i];

        if( stream_in->stream_format == LIBMPEGTS_VIDEO_MPEG2 || stream_in->stream_format == LIBMPEGTS_VIDEO_AVC )
        {
            if( !video_stream )
                video_stream = 1;
            else
            {
                fprintf( stderr, "Multiple video streams not allowed\n" );
                return -1;
            }
        }

        ts_int_stream_t *cur_stream = calloc( 1, sizeof(*cur_stream) );
        if( !cur_stream )
        {
            fprintf( stderr, "Malloc failed\n" );
            return -1;
        }

        cur_stream->pid = stream_in->pid;
        cur_stream->stream_format = stream_in->stream_format;
        for( int j = 0; steam_type_table[j][0] != 0; j++ )
        {
            if( cur_stream->stream_format == steam_type_table[j][0] )
            {
                /* DVB AC-3 and EAC-3 are different */
                if( w->ts_type == TS_TYPE_DVB &&
                    ( cur_stream->stream_format == LIBMPEGTS_AUDIO_AC3 || cur_stream->stream_format == LIBMPEGTS_AUDIO_EAC3 ) )
                    j++;

                cur_stream->stream_type = steam_type_table[j][1];
                break;
            }
        }

        if( !cur_stream->stream_type )
        {
            fprintf( stderr, "Unsupported Stream Format\n" );
            free( cur_stream );
            return -1;
        }

        if( stream_in->write_lang_code )
        {
            cur_stream->write_lang_code = 1;
            memcpy( cur_stream->lang_code, stream_in->lang_code, 4 );
        }

        cur_stream->audio_type = stream_in->audio_type;

        if( cur_stream->pid == params->programs[0].pcr_pid )
        {
            cur_program->pcr_stream = cur_stream;
            internal_pcr_pid = 1;
        }

        cur_stream->stream_id = stream_in->stream_id;
        /* Ignored in video streams  */
        cur_stream->max_frame_size = stream_in->audio_frame_size;

        if( stream_in->has_stream_identifier )
        {
            cur_stream->has_stream_identifier = 1;
            cur_stream->stream_identifier = stream_in->stream_identifier & 0xff;
        }

        cur_stream->dvb_au = stream_in->dvb_au;
        cur_stream->dvb_au_frame_rate = stream_in->dvb_au_frame_rate;

        cur_stream->hdmv_frame_rate   = stream_in->hdmv_frame_rate;
        cur_stream->hdmv_aspect_ratio = stream_in->hdmv_aspect_ratio;
        cur_stream->hdmv_video_format = stream_in->hdmv_video_format;

        cur_stream->tb.buf_size = TB_SIZE;

        /* setup T-STD buffers when audio buffers sizes are independent of number of channels */
        if( cur_stream->stream_format == LIBMPEGTS_AUDIO_MPEG1 || cur_stream->stream_format == LIBMPEGTS_AUDIO_MPEG2 )
        {
            /* use the defaults */
            cur_stream->rx = MISC_AUDIO_RXN;
            cur_stream->mb.buf_size = MISC_AUDIO_BS;
        }
        else if( cur_stream->stream_format == LIBMPEGTS_AUDIO_AC3 || cur_stream->stream_format == LIBMPEGTS_AUDIO_EAC3 )
        {
            cur_stream->rx = MISC_AUDIO_RXN;
            cur_stream->mb.buf_size = w->ts_type == TS_TYPE_ATSC || w->ts_type == TS_TYPE_CABLELABS ? AC3_BS_ATSC : AC3_BS_DVB;
        }

        cur_program->streams[cur_program->num_streams] = cur_stream;
        cur_program->num_streams++;
    }

    /* create separate PCR stream if necessary */
    if( !internal_pcr_pid )
    {
        ts_int_stream_t *pcr_stream = calloc( 1, sizeof(*pcr_stream) );
        if( !pcr_stream )
            return -1;
        pcr_stream->pid = params->programs[0].pcr_pid;
        cur_program->pcr_stream = pcr_stream;
    }

    w->ts_id = params->ts_id;
    w->ts_muxrate = params->muxrate;
    w->cbr = params->cbr;
    w->ts_type = params->ts_type;
    w->network_pid = params->network_pid;
    w->legacy_constraints = params->legacy_constraints;

    w->pcr_period = params->pcr_period ? params->pcr_period : PCR_MAX_RETRANS_TIME;
    w->pat_period = params->pat_period ? params->pat_period : PAT_MAX_RETRANS_TIME;
    w->sdt_period = params->sdt_period ? params->sdt_period : SDT_MAX_RETRANS_TIME;

    w->network_id = params->network_id ? params->network_id : DEFAULT_NID;

    w->tb.buf_size = TB_SIZE;
    w->rx_sys = RX_SYS;
    w->r_sys = MAX( R_SYS_DEFAULT, (double)w->ts_muxrate / 500 );

    // FIXME realloc if necessary

    w->out.i_bitstream = w->ts_muxrate >> 3;
    w->out.p_bitstream = calloc( 1, w->out.i_bitstream );

    if( !w->out.p_bitstream )
        return -1;

    w->pcr_list_alloced = 50000;
    w->pcr_list = malloc( w->pcr_list_alloced * sizeof(int64_t) );
    if( !w->pcr_list )
        return -1;

    return 0;
}

/* Codec-specific features */

int ts_setup_mpegvideo_stream( ts_writer_t *w, int pid, int level, int profile, int vbv_maxrate, int vbv_bufsize, int frame_rate )
{
    int bs_mux, bs_oh;
    int level_idx = -1;

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    if( !( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 || stream->stream_format == LIBMPEGTS_VIDEO_AVC ) )
    {
        fprintf( stderr, "PID is not an MPEG video stream\n" );
        return -1;
    }

    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
    {
        if( level < LIBMPEGTS_MPEG2_LEVEL_LOW || level > LIBMPEGTS_MPEG2_LEVEL_HIGHP )
        {
            fprintf( stderr, "Invalid MPEG-2 Level\n" );
            return -1;
        }
        if( profile < LIBMPEGTS_MPEG2_PROFILE_SIMPLE || profile > LIBMPEGTS_MPEG2_PROFILE_422 )
        {
            fprintf( stderr, "Invalid MPEG-2 Profile\n" );
            return -1;
        }

        for( int i = 0; mpeg2_levels[i].level != 0; i++ )
            if( level == mpeg2_levels[i].level && profile == mpeg2_levels[i].profile )
            {
                level_idx = i;
                break;
            }

        if( level_idx == -1 )
        {
            fprintf( stderr, "Invalid MPEG-2 Level/Profile combination.\n" );
            return -1;
        }
    }
    else if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
    {
        for( int i = 0; avc_levels[i].level_idc != 0; i++ )
            if( level == avc_levels[i].level_idc )
            {
                level_idx = i;
                break;
            }

        if( level_idx == -1 )
        {
            fprintf( stderr, "Invalid AVC Level\n" );
            return -1;
        }
        if( profile < AVC_BASELINE || profile > AVC_CAVLC_444_INTRA )
        {
            fprintf( stderr, "Invalid AVC Profile\n" );
            return -1;
        }
    }

    if( !stream->mpegvideo_ctx )
    {
        stream->mpegvideo_ctx = calloc( 1, sizeof(mpegvideo_stream_ctx_t) );
        if( !stream->mpegvideo_ctx )
        {
            fprintf( stderr, "Malloc failed\n" );
            return -1;
        }
    }

    stream->mpegvideo_ctx->level = level;
    stream->mpegvideo_ctx->profile = profile;

    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
    {
        bs_mux = 0.004 * mpeg2_levels[level_idx].bitrate;
        bs_oh = 1.0 * mpeg2_levels[level_idx].bitrate/750.0;

        stream->rx = 1.2 * mpeg2_levels[level_idx].bitrate;
        stream->eb.buf_size = vbv_bufsize;

        if( level == LIBMPEGTS_MPEG2_LEVEL_LOW || level == LIBMPEGTS_MPEG2_LEVEL_MAIN )
        {
            stream->mb.buf_size = bs_mux + bs_oh + mpeg2_levels[level_idx].vbv - vbv_bufsize;
            stream->rbx = mpeg2_levels[level_idx].bitrate;
        }
        else
        {
            stream->mb.buf_size = bs_mux + bs_oh;
            stream->rbx = MIN( 1.05 * vbv_maxrate, mpeg2_levels[level_idx].bitrate );
        }
    }
    else if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
    {
        int factor = (float)nal_factor[stream->mpegvideo_ctx->profile] * 1.2;
        int bitrate = avc_levels[level_idx].bitrate * factor;
        bs_mux = 0.004 * MAX( bitrate, 2000000 );
        bs_oh = 1.0 * MAX( bitrate, 2000000 )/750.0;

        stream->mb.buf_size = bs_mux + bs_oh;
        stream->eb.buf_size = avc_levels[level_idx].cpb * factor;

        stream->rx = bitrate;
        stream->rbx = bitrate;
    }

    return 0;
}

int ts_setup_mpeg2_aac_stream( ts_writer_t *w, int pid, int profile, int channel_map )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "AAC not allowed in Blu-Ray\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    if( profile < 0 || profile > 1 )
    {
        fprintf( stderr, "Invalid AAC profile\n" );
        return -1;
    }

    if( channel_map < 0 || channel_map > 7 )
    {
        fprintf( stderr, "Invalid AAC channel map\n" );
        return -1;
    }

    stream->aac_profile = profile;
    stream->aac_is_mpeg4    = 0;
    stream->aac_channel_map = channel_map;

    /* channel map index is pretty much correct so there's no point in writing a LUT */
    int num_channels = channel_map == LIBMPEGTS_MPEG2_AAC_5_POINT_1_CHANNEL ? 5 : channel_map;

    for( int i = 0; aac_buffers[i].max_channels != 0; i++ )
    {
        if( num_channels <= aac_buffers[i].max_channels )
        {
            stream->rx = aac_buffers[i].rxn;
            stream->mb.buf_size = aac_buffers[i].bsn;
        }
    }
    return 0;
}

int ts_setup_mpeg4_aac_stream( ts_writer_t *w, int pid, int profile_and_level, int num_channels )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "AAC not allowed in Blu-Ray\n" );
        return -1;
    }

    if( profile_and_level <= 0 )
    {
        fprintf( stderr, "Invalid Profile and Level value\n" );
        return -1;
    }

    if( num_channels <= 0 || num_channels > 48 )
    {
        fprintf( stderr, "Invalid number of channels\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    stream->aac_profile = profile_and_level;
    stream->aac_is_mpeg4    = 1;

    for( int i = 0; aac_buffers[i].max_channels != 0; i++ )
    {
        if( num_channels <= aac_buffers[i].max_channels )
        {
            stream->rx = aac_buffers[i].rxn;
            stream->mb.buf_size = aac_buffers[i].bsn;
        }
    }

    return 0;
};

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

    if( stream->lpcm_ctx )
        free( stream->lpcm_ctx );

    stream->lpcm_ctx = calloc( 1, sizeof(lpcm_stream_ctx_t) );
    if( !stream->lpcm_ctx )
        return -1;

    stream->lpcm_ctx->bits_per_sample = bit_depth;
    stream->lpcm_ctx->num_channels = num_channels;

    stream->mb.buf_size = SMPTE_302M_AUDIO_BS;

    /* 302M frame size is bit_depth / 4 + 1 */
    stream->rx = 1.2 * ((bit_depth >> 2) + 1) * SMPTE_302M_AUDIO_SR * 8;

    return 0;
}

int ts_setup_dvb_subtitles( ts_writer_t *w, int pid, int has_dds, int num_subtitles, ts_dvb_sub_t *subtitles )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "DVB Subtitles not allowed in Blu-Ray\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    if( !subtitles || !num_subtitles )
    {
        fprintf( stderr, "Invalid Number of subtitles\n" );
        return -1;
    }

    if( stream->dvb_sub_ctx )
        free( stream->dvb_sub_ctx );

    stream->dvb_sub_ctx = calloc( 1, num_subtitles * sizeof(ts_dvb_sub_t) );
    if( !stream->dvb_sub_ctx )
        return -1;

    stream->num_dvb_sub = num_subtitles;
    memcpy( stream->dvb_sub_ctx, subtitles, num_subtitles * sizeof(ts_dvb_sub_t) );

    /* Display Definition Segment has different buffer sizes */
    if( has_dds )
    {
        stream->tb.buf_size = DVB_SUB_DDS_TB_SIZE;
        stream->rx = DVB_SUB_DDS_RXN;
        stream->mb.buf_size = DVB_SUB_DDS_MB_SIZE;
    }
    else
    {
        stream->rx = DVB_SUB_RXN;
        stream->mb.buf_size = DVB_SUB_MB_SIZE;
    }

    return 0;
}

int ts_setup_dvb_teletext( ts_writer_t *w, int pid, int num_teletexts, ts_dvb_ttx_t *teletexts )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "Teletext not allowed in Blu-Ray\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    if( !teletexts || !num_teletexts )
    {
        fprintf( stderr, "Invalid Number of teletexts\n" );
        return -1;
    }

    if( stream->dvb_ttx_ctx )
        free( stream->dvb_ttx_ctx );

    stream->dvb_ttx_ctx = calloc( 1, num_teletexts * sizeof(ts_dvb_ttx_t) );
    if( !stream->dvb_ttx_ctx )
        return -1;

    stream->num_dvb_ttx = num_teletexts;
    memcpy( stream->dvb_ttx_ctx, teletexts, num_teletexts * sizeof(ts_dvb_ttx_t) );

    stream->tb.buf_size = TELETEXT_T_BS;
    stream->rx = TELETEXT_RXN;
    stream->mb.buf_size = TELETEXT_BTTX;

    return 0;
}

int ts_setup_dvb_vbi( ts_writer_t *w, int pid, int num_vbis, ts_dvb_vbi_t *vbis )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        fprintf( stderr, "VBI not allowed in Blu-Ray\n" );
        return -1;
    }

    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    if( !vbis || !num_vbis )
    {
        fprintf( stderr, "Invalid Number of VBI services\n" );
        return -1;
    }

    if( stream->dvb_vbi_ctx )
        free( stream->dvb_vbi_ctx );

    stream->dvb_vbi_ctx = calloc( 1, num_vbis * sizeof(ts_dvb_vbi_t) );
    if( !stream->dvb_vbi_ctx )
        return -1;

    stream->num_dvb_vbi = num_vbis;
    memcpy( stream->dvb_vbi_ctx, vbis, num_vbis * sizeof(ts_dvb_vbi_t) );

    for( int i = 0; i < stream->num_dvb_vbi; i++ )
    {
        stream->dvb_vbi_ctx[i].lines = calloc( 1, vbis[i].num_lines * sizeof(ts_dvb_vbi_line_t) );
        if( !stream->dvb_vbi_ctx[i].lines )
        {
            fprintf( stderr, "Malloc failed\n" );

            for( int j = 0; i < stream->num_dvb_vbi; j++ )
            {
                if( stream->dvb_vbi_ctx[j].lines )
                    free( stream->dvb_vbi_ctx[j].lines );
            }

            free( stream->dvb_vbi_ctx );

            return -1;
        }
        memcpy( stream->dvb_vbi_ctx[i].lines, vbis[i].lines, vbis[i].num_lines * sizeof(ts_dvb_vbi_line_t) );
    }

    if( w->ts_type == TS_TYPE_CABLELABS || w->ts_type == TS_TYPE_ATSC )
    {
        stream->rx = SCTE_VBI_RXN;
        stream->mb.buf_size = SCTE_VBI_MB_SIZE;
    }
    else
    {
        /* DVB-VBI uses teletext T-STD */
        stream->tb.buf_size = TELETEXT_T_BS;
        stream->rx = TELETEXT_RXN;
        stream->mb.buf_size = TELETEXT_BTTX;
    }

    return 0;
}

int ts_setup_sdt( ts_writer_t *w )
{
    w->sdt = calloc( 1, sizeof(*w->sdt) );
    if( !w->sdt )
    {
        fprintf( stderr, "malloc failed\n" );
        return -1;
    }

    return 0;
}

void ts_remove_sdt( ts_writer_t *w )
{
    free( w->sdt );
    w->sdt = NULL;
}

int ts_write_frames( ts_writer_t *w, ts_frame_t *frames, int num_frames, uint8_t **out, int *len, int64_t **pcr_list )
{
    ts_int_program_t *program = w->programs[0];
    ts_int_stream_t *stream;

    int initial_queued_pes = w->num_buffered_frames;
    ts_int_pes_t **queued_pes;
    ts_int_pes_t **new_pes;

    int stuffing, flags, pkt_bytes_left, write_pcr, write_adapt_field, adapt_field_len, pes_start, running;
    uint8_t temp[200];
    bs_t q;
    bs_t *s = &w->out.bs;
    /* earliest arrival time that the pes packet can arrive */
    int64_t cur_pcr = 0;

    w->num_pcrs = 0;

    bs_init( s, w->out.p_bitstream, w->out.i_bitstream );

    if( num_frames < 0 )
    {
        fprintf( stderr, "Invalid number of frames\n" );
        return -1;
    }

    if( num_frames )
    {
        ts_int_pes_t **tmp = realloc( w->buffered_frames, (w->num_buffered_frames+num_frames) * sizeof(w->buffered_frames[0]) );
        if( !tmp )
        {
           fprintf( stderr, "Malloc failed\n" );
           return -1;
        }
        w->buffered_frames = tmp;
        new_pes = &w->buffered_frames[w->num_buffered_frames];
        w->num_buffered_frames += num_frames;
    }

    queued_pes = w->buffered_frames;

    for( int i = 0; i < num_frames; i++ )
    {
        stream = find_stream( w, frames[i].pid );

        if( !stream )
        {
            fprintf( stderr, "PID %i not found for frame %i\n", frames[i].pid, i );
            return -1;
        }

        /* Codec specific parameters */
        if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 || stream->stream_format == LIBMPEGTS_VIDEO_AVC )
        {
            if( !stream->mpegvideo_ctx )
            {
               fprintf( stderr, "MPEG video stream needs additional information. Call ts_setup_mpegvideo_stream \n" );
               return -1;
            }
            program->video_dts = frames[i].dts;
        }
        else if( stream->stream_format == LIBMPEGTS_DVB_SUB )
        {
            if( !stream->dvb_sub_ctx )
            {
               fprintf( stderr, "DVB subtitle stream needs additional information. Call ts_setup_dvb_subtitles \n" );
               return -1;
            }
        }
        else if( stream->stream_format == LIBMPEGTS_DVB_TELETEXT )
        {
            if( !stream->dvb_ttx_ctx )
            {
               fprintf( stderr, "DVB Teletext stream needs additional information. Call ts_setup_dvb_teletext \n" );
               return -1;
            }
        }
        else if( stream->stream_format == LIBMPEGTS_DVB_VBI )
        {
            if( !stream->dvb_vbi_ctx )
            {
               fprintf( stderr, "DVB VBI stream needs additional information. Call ts_setup_dvb_vbi \n" );
               return -1;
            }
        }
        // TODO more

        new_pes[i] = calloc( 1, sizeof(ts_int_pes_t) );
        if( !new_pes[i] )
        {
           fprintf( stderr, "Malloc failed\n" );
           return -1;
        }

        new_pes[i]->stream = stream;
        new_pes[i]->random_access = !!frames[i].random_access;
        new_pes[i]->priority = !!frames[i].priority;
        new_pes[i]->dts = frames[i].dts + TS_START * 90000LL;
        new_pes[i]->pts = frames[i].pts + TS_START * 90000LL;

        if( IS_VIDEO( stream ) )
        {
            new_pes[i]->frame_type = frames[i].frame_type;
            new_pes[i]->initial_arrival_time = frames[i].cpb_initial_arrival_time + TS_START * 27000000LL;
            new_pes[i]->final_arrival_time = frames[i].cpb_final_arrival_time + TS_START * 27000000LL;
            new_pes[i]->ref_pic_idc = frames[i].ref_pic_idc;
            new_pes[i]->write_pulldown_info = frames[i].write_pulldown_info;
            new_pes[i]->pic_struct = frames[i].pic_struct;
        }
        else if( stream->stream_format == LIBMPEGTS_DVB_TELETEXT )
            new_pes[i]->initial_arrival_time = (new_pes[i]->dts - 3600) * 300; /* Teletext is special because data can only stay in the buffer for 40ms */
        else if( stream->stream_format == LIBMPEGTS_DVB_SUB )
            new_pes[i]->initial_arrival_time = 0; /* FIXME: is this right? */
        else if( stream->stream_format == LIBMPEGTS_DVB_VBI && ( w->ts_type == TS_TYPE_CABLELABS || w->ts_type == TS_TYPE_ATSC ) )
            new_pes[i]->initial_arrival_time = (new_pes[i]->dts - 3003) * 300; /* SCTE-127 VBI is always in terms of NTSC */
        else if( stream->stream_format == LIBMPEGTS_DVB_VBI )
            new_pes[i]->initial_arrival_time = (new_pes[i]->dts - 3600) * 300;
        else
            new_pes[i]->initial_arrival_time = (new_pes[i]->dts - stream->max_frame_size) * 300; /* earliest that a frame can arrive */

        if( !IS_VIDEO( stream ) )
            new_pes[i]->final_arrival_time = new_pes[i]->dts * 300;

        /* probe the first normal looking ac3 frame if extra data is needed */
        if( !stream->atsc_ac3_ctx && stream->stream_format == LIBMPEGTS_AUDIO_AC3 &&
            ( w->ts_type == TS_TYPE_CABLELABS || w->ts_type == TS_TYPE_ATSC ) &&
            frames[i].size > 100 && frames[i].data[0] == 0xb && frames[i].data[1] == 0x77 )
        {
            stream->atsc_ac3_ctx = calloc( 1, sizeof(ts_atsc_ac3_info) );
            if( !stream->atsc_ac3_ctx  )
            {
               fprintf( stderr, "Malloc failed\n" );
               return -1;
            }
            parse_ac3_frame( stream->atsc_ac3_ctx, frames[i].data );
        }

        /* 512 bytes is more than enough for pes overhead */
        new_pes[i]->data = malloc( frames[i].size + 512 );
        if( !new_pes[i]->data )
        {
           fprintf( stderr, "Malloc failed\n" );
           return -1;
        }

        new_pes[i]->header_size = write_pes( w, program, &frames[i], new_pes[i] );
    }

    if( !initial_queued_pes )
    {
        out = NULL;
        *len = 0;
        *pcr_list = NULL;
        return 0;
    }

    write_pcr = 0;
    running = 1;

    if( !w->first_input )
    {
        if( write_pcr_empty( w, program, 1 ) < 0 )
            return -1;
        retransmit_psi_and_si( w, program, 1 );
        w->first_input = 1;
    }

    /* loop through and find the time when the second video packet in the queue can arrive */
    int video_found = 0;
    int64_t pcr_stop = 0;

    cur_pcr = get_pcr_int( w, 0 );

    for( int i = 0; i < w->num_buffered_frames; i++ )
    {
        stream = queued_pes[i]->stream;
        if( IS_VIDEO( stream ) )
        {
            /* last frame is a special case - FIXME: is this acceptable in all use-cases? */
            if( !num_frames )
                pcr_stop = queued_pes[i]->dts;
            else if( !video_found )
                video_found = 1;
            else
                pcr_stop = queued_pes[i]->initial_arrival_time; /* earliest that a frame can arrive */
        }
    }

    while( cur_pcr < pcr_stop )
    {
        //printf("\n pcr_stop %"PRIi64" cur_pcr %"PRIi64" \n", pcr_stop, cur_pcr );

        ts_int_pes_t *pes = NULL;
        write_adapt_field = adapt_field_len = write_pcr = 0;
        pkt_bytes_left = 184;

        if( check_bitstream( w ) < 0 )
            return -1;

        /* write any queued PMT packets */
        if( program->num_queued_pmt && w->tb.cur_buf == 0.0 )
        {
            eject_queued_pmt( w, program, s );
            cur_pcr = get_pcr_int( w, 0 );
            continue;
        }

        // FIXME at low bitrates this might need tweaking
        int need_pcr = check_pcr( w, program );

        /* Check all the non-video packets first */
        if( !need_pcr )
        {
            for( int i = 0; i < w->num_buffered_frames; i++ )
            {
                stream = queued_pes[i]->stream;
                if( (!pes || queued_pes[i]->dts < pes->dts) && !IS_VIDEO( stream ) )
                {
                    int total_packets = (queued_pes[i]->size + 183) / 184;
                    int packets_left = (queued_pes[i]->bytes_left + 183) / 184;
                    double drip_rate = (double)total_packets/ ( queued_pes[i]->final_arrival_time - queued_pes[i]->initial_arrival_time );
                    double remaining_drip_rate = (double)packets_left / (queued_pes[i]->final_arrival_time - cur_pcr);

                    /* exclude video packets */
                    if( cur_pcr >= queued_pes[i]->initial_arrival_time && stream->tb.cur_buf == 0.0 &&
                        ( drip_rate < remaining_drip_rate || queued_pes[i]->final_arrival_time < cur_pcr ) )
                    {
                        pes = queued_pes[i];
                    }
                }
            }
        }

        /* See if we can write a video packet if non-audio packets can't be written. */
        if( !pes || need_pcr )
        {
            for( int i = 0; i < w->num_buffered_frames; i++ )
            {
                stream = queued_pes[i]->stream;
                if( IS_VIDEO( stream ) )
                {
                    int total_packets = (queued_pes[i]->size + 183) / 184;
                    int packets_left = (queued_pes[i]->bytes_left + 183) / 184;
                    double drip_rate = (double)total_packets / ( queued_pes[i]->final_arrival_time - queued_pes[i]->initial_arrival_time );
                    double remaining_drip_rate = (double)packets_left / (queued_pes[i]->final_arrival_time - cur_pcr );

                    /* Write a video packet anyway if we can put a PCR on it */
                    if( cur_pcr >= queued_pes[i]->initial_arrival_time && stream->tb.cur_buf == 0.0 &&
                        ( drip_rate < remaining_drip_rate || queued_pes[i]->final_arrival_time < cur_pcr || need_pcr ) )
                    {
                        pes = queued_pes[i];
                        break;
                    }
                }
            }
        }

        if( pes )
        {
            stream = pes->stream;
            pes_start = pes->data == pes->cur_pos; /* flag if packet contains pes header */

            if( pcr_stop < cur_pcr )
                fprintf( stderr, "\n pcr_stop is less than pcr pid: %i pcr_stop: %"PRIi64" pcr: %"PRIi64" \n", pes->stream->pid, pcr_stop, cur_pcr );

            // FIXME complain less
            if( pes->dts * 300 < cur_pcr )
                fprintf( stderr, "\n dts is less than pcr pid: %i dts: %"PRIi64" pcr: %"PRIi64" \n", pes->stream->pid, pes->dts*300, cur_pcr );

            bs_init( &q, temp, 150 );

            if( program->pcr_stream == stream && pes_start )
                write_adapt_field = 1;

            if( check_pcr( w, program ) )
            {
                if( program->pcr_stream == stream )
                {
                    /* piggyback pcr on this stream */
                    write_adapt_field = write_pcr = 1;
                }
                else if( write_pcr_empty( w, program, 0 ) < 0 )
                    return -1;
            }

#if 0
            if( IS_VIDEO( stream ) && pes_start )
            {
                printf("\n last pcr delta %"PRIi64" \n", get_pcr( w, 0 ) - stream->last_pkt_pcr );
            }
#endif

            stream->last_pkt_pcr = cur_pcr;

            if( write_adapt_field )
            {
                adapt_field_len = write_adaptation_field( w, &q, program, pes, write_pcr, 1, 0, 0 );
                pkt_bytes_left -= adapt_field_len;
            }

            /* DVB AU_Information is large so consider this case */
            // FIXME consider cablelabs legacy
            if( !adapt_field_len && pes_start && stream->dvb_au )
            {
                adapt_field_len = write_adaptation_field( w, &q, program, pes, 0, 1, 0, 0 );
                pkt_bytes_left -= adapt_field_len;
            }

            // TODO CableLabs legacy
            if( pes->bytes_left >= pkt_bytes_left )
            {
                write_packet_header( w, s, pes_start, stream->pid, PAYLOAD_ONLY + ((!!adapt_field_len)<<1), &stream->cc );
                if( adapt_field_len )
                    write_adaptation_field( w, s, program, pes, write_pcr, 1, 0, 0 );

                write_bytes( s, pes->cur_pos, pkt_bytes_left );
                pes->cur_pos += pkt_bytes_left;
                pes->bytes_left -= pkt_bytes_left;
                add_to_buffer( &stream->tb );
                if( increase_pcr( w, 1, 0 ) < 0 )
                    return -1;
            }
            else
            {
                /* stuff the last packet with an oversized adaptation field */
                stuffing = pkt_bytes_left - pes->bytes_left;
                flags = 1;

                /* special case where the adaptation_field_length byte is the stuffing */
                // FIXME except for cablelabs legacy
                if( stuffing == 1 && !adapt_field_len )
                {
                    stuffing = flags = 0;
                    adapt_field_len = 1;
                }
                else if( stuffing && !adapt_field_len )
                {
                    adapt_field_len = 2;
                    stuffing -= 2;  /* 2 bytes for adaptation field in this case. NOTE: needs fixing if more private data added */
                }

                write_packet_header( w, s, pes_start, stream->pid, PAYLOAD_ONLY + ((!!adapt_field_len)<<1), &stream->cc );
                if( adapt_field_len )
                    write_adaptation_field( w, s, program, pes, write_pcr, flags, stuffing, 0 );

                write_bytes(s, pes->cur_pos, pes->bytes_left );
                pes->bytes_left = 0;
                add_to_buffer( &stream->tb );
                if( increase_pcr( w, 1, 0 ) < 0 )
                    return -1;
            }

            if( pes->bytes_left == 0 )
            {
                /* eject the current pes from the queue */
                for( int i = 0; i < w->num_buffered_frames; i++ )
                {
                    if( queued_pes[i] == pes )
                    {
                        w->num_buffered_frames--;
                        memmove( &queued_pes[i], &queued_pes[i+1], (w->num_buffered_frames-i) * sizeof(queued_pes) );
                        break;
                    }
                }

                free( pes->data );
                free( pes );
            }

            retransmit_psi_and_si( w, program, 0 );
        }
        else /* no packets can be written */
        {
            if( check_pcr( w, program ) )
            {
                if( write_pcr_empty( w, program, 0 ) < 0 )
                    return -1;
            }
            else if( w->cbr )
            {
                if( write_null_packet( w ) < 0 )
                    return -1;
            }
            else if( increase_pcr( w, 1, 1 ) < 0 )
                return -1; /* write imaginary packet in capped vbr mode */
        }
        cur_pcr = get_pcr_int( w, 0 );
    }

    bs_flush( s );

    *out = w->out.p_bitstream;
    *len = bs_pos( s ) >> 3;
    *pcr_list = w->pcr_list;

    // TODO if it's the final packet write blu-ray overflows
    // TODO count bits here

    return 0;
}

int ts_delete_stream( ts_writer_t *w, int pid )
{
    // TODO

    return 0;
}

int ts_close_writer( ts_writer_t *w )
{
    for( int i = 0; i < w->num_programs; i++ )
    {
        for( int j = 0; j < w->programs[i]->num_streams; j++ )
        {
            // TODO free other stuff
            if( w->programs[i]->streams[j]->mpegvideo_ctx )
                free( w->programs[i]->streams[j]->mpegvideo_ctx );
            if( w->programs[i]->streams[j]->lpcm_ctx )
                free( w->programs[i]->streams[j]->lpcm_ctx );
            if( w->programs[i]->streams[j]->atsc_ac3_ctx )
                free( w->programs[i]->streams[j]->atsc_ac3_ctx );
            if( w->programs[i]->streams[j]->dvb_sub_ctx )
                free( w->programs[i]->streams[j]->dvb_sub_ctx );
            if( w->programs[i]->streams[j]->dvb_ttx_ctx )
                free( w->programs[i]->streams[j]->dvb_ttx_ctx );
            if( w->programs[i]->streams[j]->dvb_vbi_ctx )
                free( w->programs[i]->streams[j]->dvb_vbi_ctx );

            free( w->programs[i]->streams[j] );
        }

        for( int j = 0; j < w->programs[i]->num_queued_pmt; j++ )
            free( w->programs[i]->pmt_packets[j] );
        if( w->programs[i]->pmt_packets )
            free( w->programs[i]->pmt_packets );
        if( w->programs[i]->sdt_ctx.service_name )
            free( w->programs[i]->sdt_ctx.service_name );
        if( w->programs[i]->sdt_ctx.provider_name )
            free( w->programs[i]->sdt_ctx.provider_name );

        free( w->programs[i] );
    }

    for( int i = 0; i < w->num_buffered_frames; i++ )
    {
        free( w->buffered_frames[i]->data );
        free( w->buffered_frames[i] );
    }

    free( w->buffered_frames );

    if( w->sdt )
        free( w->sdt );

    if( w->pcr_list )
        free( w->pcr_list );

    if( w->out.p_bitstream )
        free( w->out.p_bitstream );
    free( w );

    return 0;
}

void write_packet_header( ts_writer_t *w, bs_t *s, int start, int pid, int adapt_field, int *cc )
{
    if( w->ts_type == TS_TYPE_BLU_RAY )
    {
        // tp_extra_header
        bs_write( s, 2, 0 ); // copy_permission_indicator
        bs_write( s, 30, 0 ); // arrival_time_stamp FIXME
    }

    bs_write( s, 8, 0x47 ); // sync byte
    bs_write1( s, 0 );      // transport_error_indicator
    bs_write1( s, start );  // payload_unit_start_indicator
    bs_write1( s, 0 );      // transport_priority (not usually used)
    bs_write( s, 5, (pid >> 8) & 0x1f ); // PID
    bs_write( s, 8, pid & 0xff );        // PID
    bs_write( s, 2, 0 );    // transport_scrambling_control
    bs_write( s, 2, adapt_field & 0x03 );

    if( adapt_field == ADAPT_FIELD_ONLY )
        bs_write( s, 4, (*cc - 1) & 0xf ); // continuity counter
    else
        bs_write( s, 4, (*cc)++ & 0xf );   // continuity counter
}

int write_padding( bs_t *s, int start )
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

int increase_pcr( ts_writer_t *w, int num_packets, int imaginary )
{
    int64_t *temp;
    int64_t pcr;

    // TODO do this for all programs
    ts_int_program_t *program = w->programs[0];
    double next_pcr = TS_START + (w->packets_written + num_packets) * 8.0 * TS_PACKET_SIZE / w->ts_muxrate;
    /* buffer drip (TODO: all buffers?) */
    drip_buffer( w, program, w->rx_sys, &w->tb, next_pcr );
    for( int i = 0; i < program->num_streams; i++ )
    {
        drip_buffer( w, program, program->streams[i]->rx, &program->streams[i]->tb, next_pcr );
    }

    w->packets_written += num_packets;

    if( !imaginary )
    {
        if( w->num_pcrs >= w->pcr_list_alloced )
        {
            temp = realloc( w->pcr_list, w->pcr_list_alloced * 2 * sizeof(int64_t) );
            if( !temp )
               return -1;
            w->pcr_list_alloced <<= 1;
            w->pcr_list = temp;
        }

        pcr = get_pcr_int( w, 0 );

        w->pcr_list[w->num_pcrs++] = pcr;
    }

    return 0;
}

void write_crc( bs_t *s, int start )
{
    uint8_t *p_start = s->p_start;
    int pos = (bs_pos( s ) - start) >> 3;
    uint32_t crc = crc_32( s->p - pos, pos );

    bs_init( s, s->p, s->p_end - s->p );
    s->p_start = p_start;

    bs_write32( s, crc );
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
