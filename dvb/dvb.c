/*****************************************************************************
 * dvb.c : DVB functions
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

#include "../common.h"
#include "dvb.h"

/* Descriptors */
void write_stream_identifier_descriptor( bs_t *s, uint8_t stream_identifier )
{
    bs_write( s, 8, STREAM_IDENTIFIER_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 1 );                 // descriptor_length
    bs_write( s, 8, stream_identifier ); // component_tag
}

void write_dvb_au_information( ts_writer_t *w, bs_t *s, ts_int_stream_t *stream, ts_int_frame_t *frame )
void write_dvb_au_information( bs_t *s, ts_int_pes_t *pes )
{
    bs_t q;
    uint8_t temp[128];

    ts_int_stream_t *stream = pes->stream;

    bs_write( s, 8, AU_INFORMATION_DATA_FIELD ); // data_field_tag
    bs_init( &q, temp, 128 );

    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
        bs_write( &q, 4, 1 );   // AU_coding_format
    else if( stream->stream_format == LIBMPEGTS_VIDEO_H264 )
        bs_write( &q, 4, 0x2 ); // AU_coding_format

    bs_write( &q, 4, pes->frame_type );  // AU_coding_type_information
    bs_write( &q, 2, pes->ref_pic_idc ); // AU_ref_pic_idc
    bs_write( &q, 2, pes->pic_struct );  // AU_pic_struct

    bs_write1( &q, 1 ); // AU_PTS_present_flag
    bs_write1( &q, 1 ); // AU_profile_info_present_flag
    bs_write1( &q, 1 ); // AU_stream_info_present_flag
    bs_write1( &q, 0 ); // AU_trick_mode_info_present_flag

    bs_write32( &q, (pes->pts * 300) & 0xffffffff ); // AU_PTS_32

    bs_write( &q, 4, 0 ); // reserved
    bs_write( &q, 4, stream->dvb_au_frame_rate ); // AU_frame_rate_code

    bs_write( &q, 8, stream->mpegvideo_ctx->profile & 0xff ); // profile_idc

    if( stream->stream_format == LIBMPEGTS_VIDEO_H264 )
    {
        bs_write1( &q, stream->mpegvideo_ctx->profile == H264_BASELINE ); // constraint_set0_flag
        bs_write1( &q, stream->mpegvideo_ctx->profile <= H264_MAIN );     // constraint_set1_flag
        bs_write1( &q, 0 );                                               // constraint_set2_flag
        if( stream->mpegvideo_ctx->level == 9 && stream->mpegvideo_ctx->profile <= H264_MAIN ) // level 1b
            bs_write1( &q, 1 );                                           // constraint_set3_flag
        else if( stream->mpegvideo_ctx->profile == H264_HIGH_10_INTRA   ||
                 stream->mpegvideo_ctx->profile == H264_CAVLC_444_INTRA ||
                 stream->mpegvideo_ctx->profile == H264_HIGH_444_INTRA )
            bs_write1( &q, 1 );                                           // constraint_set3_flag
        else
            bs_write1( &q, 0 );                                           // constraint_set3_flag
        bs_write1( &q, 0 );                                               // constraint_set4_flag
        bs_write1( &q, 0 );                                               // constraint_set5_flag
    }
    else
        bs_write( &q, 5, 0 );

    bs_write( &q, 2, 0 );                                                 // AU_AVC_compatible_flags
    bs_write( &q, 8, stream->mpegvideo_ctx->level & 0xff );               // level_idc

    if( pes->write_pulldown_info )
    {
        bs_write1( &q, 1 );   // AU_Pulldown_info_present_flag
        bs_write( &q, 6, 0 ); // AU_reserved_zero
        bs_write1( &q, 0 );   // AU_flags_extension_1

        bs_write( &q, 4, 0 ); // AU_reserved_zero
        bs_write( &q, 4, pes->pic_struct & 0xf ); // AU_Pulldown_info
    }

    /* reserved bytes */

    bs_flush( &q );
    bs_write( s, 8, bs_pos( &q ) >> 3 ); // data_field_length
    write_bytes( s, temp, bs_pos( &q ) >> 3 );
}
