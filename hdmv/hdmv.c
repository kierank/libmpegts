/*****************************************************************************
 * hdmv.c : HDMV specific functions
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
#include "hdmv.h"

int ts_setup_hdmv_lpcm_stream( ts_writer_t *w, int pid, int num_channels, int sample_rate, int bits_per_sample )
{
    ts_int_stream_t *stream = find_stream( w, pid );

    if( !stream )
    {
        fprintf( stderr, "Invalid PID\n" );
        return -1;
    }

    stream->lpcm_ctx = calloc( 1, sizeof(stream->lpcm_ctx) );
    if( !stream->lpcm_ctx )
        return -1;

    stream->lpcm_ctx->num_channels = num_channels;
    stream->lpcm_ctx->sample_rate = sample_rate;
    stream->lpcm_ctx->bits_per_sample = bits_per_sample;

    return 0;
}

int ts_setup_dtcp( ts_writer_t *w, uint8_t byte_1, uint8_t byte_2 )
{
    w->dtcp_ctx = calloc( 1, sizeof(w->dtcp_ctx) );
    if( !w->dtcp_ctx )
        return -1;

    w->dtcp_ctx->byte_1 = byte_1;
    w->dtcp_ctx->byte_2 = byte_2;

    return 0;
}

/* First loop of PMT Descriptors */
void write_hdmv_copy_control_descriptor( ts_writer_t *w, bs_t *s )
{
    bs_write( s, 8, HDMV_COPY_CTRL_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x04 );           // descriptor_length
    bs_write( s, 16, 0x0fff );        // CA_System_ID
    bs_write( s, 8, w->dtcp_ctx->byte_1 ); // private_data_byte
    bs_write( s, 8, w->dtcp_ctx->byte_2 ); // private_data_byte
}

/* Second loop of PMT Descriptor */
void write_hdmv_video_registration_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    char *format_id = "HDMV";

    bs_write( s, 8, REGISTRATION_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x8 ); // descriptor_length
    while( *format_id != '\0' )
        bs_write( s, 8, *format_id++ );  // format_identifier

    bs_write( s, 8, 0xff ); // stuffing_bits
    bs_write( s, 8, stream->stream_id ); // stream_coding_type
    bs_write( s, 4, stream->hdmv_video_format ); // video_format
    bs_write( s, 4, stream->hdmv_frame_rate );   // frame_rate
    bs_write( s, 4, stream->hdmv_aspect_ratio ); // aspect_ratio
    bs_write( s, 4, 0xf ); // stuffing_bits
}

void write_hdmv_lpcm_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    write_registration_descriptor( s, REGISTRATION_DESCRIPTOR_TAG, 8, "HDMV" );
    bs_write( s, 8, 0xff );                // stuffing_bits
    bs_write( s, 8, stream->stream_type ); // stream_coding_type

    if( stream->lpcm_ctx->num_channels == 1 )
        bs_write( s, 4, 1 );    // audio_presentation_type
    else if( stream->lpcm_ctx->num_channels == 2 )
        bs_write( s, 4, 0x03 ); // audio_presentation_type
    else
        bs_write( s, 4, 0x06 ); // audio_presentation_type

    if( stream->lpcm_ctx->sample_rate == 48 )
        bs_write( s, 4, 1 );    // sampling_frequency
    else if( stream->lpcm_ctx->sample_rate == 96 )
        bs_write( s, 4, 0x04 ); // sampling_frequency
    else
        bs_write( s, 4, 0x05 ); // sampling_frequency

    if( stream->lpcm_ctx->bits_per_sample == 16 )
        bs_write( s, 2, 1 );    // bits_per_sample
    else if( stream->lpcm_ctx->bits_per_sample == 20 )
        bs_write( s, 2, 0x02 ); // bits_per_sample
    else
        bs_write( s, 2, 0x03 ); // bits_per_sample

    bs_write( s, 6, 0x3f );     // stuffing_bits

}

/* In loop of SIT */
void write_partial_ts_descriptor( ts_writer_t *w, bs_t *s )
{
    bs_write( s, 8, HDMV_PARTIAL_TS_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0x08 );      // descriptor_length
    bs_write( s, 2, 0x03 );      // DVB_reserved_future_use
    bs_write( s, 22, w->ts_muxrate / 400 ); // peak_rate
    bs_write( s, 2, 0x03 );      // DVB_reserved_future_use
    bs_write( s, 22, 0x3fffff ); // minimum_overall_smoothing_rate
    bs_write( s, 2, 0x03 );      // DVB_reserved_future_use
    bs_write( s, 14, 0x3fff );   // maximum_overall_smoothing_buffer
}
