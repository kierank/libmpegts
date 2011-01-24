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

/**** PMT Second Loop Descriptors ****/
void write_aac_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    bs_write( s, 8, DVB_AAC_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 1 );                      // descriptor_length
    bs_write( s, 8, stream->aac_profile );    // profile_and_level
}

void write_adaptation_field_data_descriptor( bs_t *s, uint8_t identifier )
{
    bs_write( s, 8, DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR ); // descriptor_tag
    bs_write( s, 8, 1 );                      // descriptor_length
    bs_write( s, 8, identifier );             // adaptation_field_data_identifier
}

void write_dvb_subtitling_descriptor( bs_t *s )
{
    // FIXME
    bs_write( s, 8, DVB_SUBTITLING_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0 );                             // descriptor_length
    // FIXME multiple subtitles
    for( int j = 0; j < 3; j++ )
        bs_write( s, 8, 0 );                         // ISO_639_language_code
    bs_write( s, 8, 0 );                             // subtitling_type
    bs_write( s, 16, 0 );                            // composition_page_id
    bs_write( s, 16, 0 );                            // ancillary_page_id
}

void write_stream_identifier_descriptor( bs_t *s, uint8_t stream_identifier )
{
    bs_write( s, 8, DVB_STREAM_IDENTIFIER_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 1 );                 // descriptor_length
    bs_write( s, 8, stream_identifier ); // component_tag
}

void write_teletext_descriptor( bs_t *s, ts_int_stream_t *stream )
{
    // FIXME
    bs_write( s, 8, DVB_TELETEXT_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 5 );                           // descriptor_length
    // FIXME multiple TTX
    for( int j = 0; j < 3; j++ )
        bs_write( s, 8, 0 );                       // ISO_639_language_code
    bs_write( s, 5, 0 );                           // teletext_type
    bs_write( s, 3, 0 );                           // teletext_magazine_number
    bs_write( s, 8, 0 );                           // teletext_page_number
}

/*
static void write_service_descriptor( bs_t *s )
{
    bs_write( s, 8, DVB_SERVICE_DESCRIPTOR_TAG );              // descriptor_tag
    bs_write( s, 8, 0 );   // descriptor_length
    bs_write( s, 8, 0 );              // service_type
    bs_write( s, 8, 0 ); // service_provider_name_length

    // TODO support more character codes
    while( *provider_name != '\0' )
        bs_write( s, 8, *provider_name++ );

    bs_write( s, 8, name_len ); // service_name_length

    while( *name != '\0' )
       bs_write( s, 8, *name++ );
}
*/

/* DVB Service Information */
void write_nit( ts_writer_t *w )
{
    int start;

    bs_t *s = &w->out.bs;

    write_packet_header( w, 1, w->network_pid, PAYLOAD_ONLY, &w->nit->cc );

    bs_write( s, 8, 0 );       // pointer field

    start = bs_pos( s );
    bs_write( s, 8, NIT_TID ); // table_id = network_information_section
    bs_write1( s, 1 );         // section_syntax_indicator
    bs_write1( s, 1 );         // reserved_future_use
    bs_write( s, 2, 0x03 );    // reserved
    bs_write( s, 12, 0x13 );   // section_length
    bs_write( s, 16, w->network_id ); // network_id
    bs_write( s, 2, 0x02 );    // reserved
    bs_write( s, 5, 0 );       // version_number
    bs_write1( s, 1 );         // current_next_indicator
    bs_write(s, 8, 0 );        // section_number
    bs_write(s, 8, 0 );        // last_section_number
    bs_write(s, 4, 0xf );      // reserved_future_use
    bs_write(s, 12, 0 );       // network_descriptors_length

    // network descriptor(s) here

    bs_write(s, 4, 0xf );        // reserved_future_use
    bs_write(s, 12, 0 );         // transport_stream_loop_length

    bs_write( s, 16, w->ts_id ); // transport_stream_id
    bs_write( s, 16, w->network_id );   // original_network_id
    bs_write( s, 4, 0xf );       // reserved_future_use
    bs_write(s, 12, 0 );         // transport_descriptors_length

    // transport descriptor(s) here

    bs_flush( s );
    write_crc( s, start );

    // -40 to include header and pointer field
    write_padding( s, start - 40 );
    increase_pcr( w, 1 );
}
#if 0
/* "The SDT contains data describing the services in the system e.g. names of services, the service provider, etc" */
void write_sdt( ts_writer_t *w )
{
    uint64_t start;
    int i;

    bs_t *s = &w->out.bs;

    write_packet_header( w, 1, SDT_PID, PAYLOAD_ONLY, &w->sdt->cc );
    bs_write( s, 8, 0 );         // pointer field

    start = bs_pos( s );
    bs_write( s, 8, SDT_TID );   // table_id
    bs_write1( s, 1 );           // section_syntax_indicator
    bs_write1( s, 1 );           // reserved_future_use
    bs_write1( s, 1 );           // reserved

// TODO temp

    bs_write( s, 12, len );      // section_length
    bs_write( s, 16, w->ts_id ); // transport_stream_id
    bs_write( s, 2, 0x03 );      // reserved
    bs_write( s, 5, 0 );         // version_number
    bs_write1( s, 1 );           // current_next_indicator
    bs_write( s, 8, 0 );         // section_number
    bs_write( s, 8, 0 );         // last_section_number
    bs_write( s, 8, w->nid );    // original_network_id
    bs_write( s, 8, 0xff );      // reserved_future_use

    for( i = 0; i < w->num_programs; i++ )
    {
        bs_write( s, 16, w->programs[i]->program_num & 0xffff ); // service_id (equivalent to program_number)
        bs_write( s, 6, 0x7f ); // reserved_future_use
        bs_write1( s, 0 );      // EIT_schedule_flag
        bs_write1( s, 1 );      // EIT_present_following_flag
        bs_write( s, 3, 0 );    // running_status
        bs_write1( s, 1 );      // free_CA_mode

        int provider_name_len = strlen( w->programs[i]->sdt_ctx->provider_name );
        int name_len = strlen( w->programs[i]->sdt_ctx->service_name );

        char *provider_name = w->programs[i]->sdt_ctx->provider_name;
        char *name = w->programs[i]->sdt_ctx->service_name;

        int descriptors_len = 5 + provider_name_len + name_len;
        bs_write( s, 12, descriptors_len ); // descriptors_loop_length

        // service descriptor (mandatory for DVB)


        // other descriptor(s) here
    }

    bs_flush( s );
    write_crc( s, start );

    // -40 to include header and pointer field
    write_padding( s, start - 40 );
    increase_pcr( w, 1 );
}

// FIXME

// "the EIT contains data concerning events or programmes such as event name, start time, duration, etc.; "
void write_eit( ts_writer_t *w )
{
    uint64_t start;

    bs_t *s = &w->out.bs;

    write_packet_header( w, 1, EIT_PID, PAYLOAD_ONLY, &w->eit->cc );
    bs_write( s, 8, 0 );       // pointer field

    start = bs_pos( s );
    bs_write( s, 8, EIT_TID ); // table_id
    bs_write1( s, 0 );         // section_syntax_indicator CHECKME
    bs_write1( s, 1 );         // reserved_future_use
    bs_write( s, 2, 0x03);     // reserved


    bs_write( s, 12, len );    // section_length

}
// "the TDT gives information relating to the present time and date. This information is given in a separate
// table due to the frequent updating of this information. "
void write_tdt( ts_writer_t *w )
{
    uint64_t start;

    bs_t *s = &w->out.bs;

    write_packet_header( w, 1, TDT_PID, PAYLOAD_ONLY, &w->tdt->cc );
    bs_write( s, 8, 0 );       // pointer field

    start = bs_pos( s );
    bs_write( s, 8, TDT_TID ); // table_id
    bs_write1( s, 0 );         // section_syntax_indicator
    bs_write1( s, 1 );         // reserved_future_use
    bs_write( s, 2, 0x03);     // reserved
    bs_write( s, 12, 0x05);    // section_length

    // FIXME

    bs_write( s, 4, & 0x0f ); //

    increase_pcr( w, 1 );
}
#endif
// TODO TOT

void write_dvb_au_information( bs_t *s, ts_int_pes_t *pes )
{
    bs_t q;
    uint8_t temp[128];

    ts_int_stream_t *stream = pes->stream;

    bs_write( s, 8, AU_INFORMATION_DATA_FIELD ); // data_field_tag
    bs_init( &q, temp, 128 );

    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
        bs_write( &q, 4, 1 );   // AU_coding_format
    else if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
        bs_write( &q, 4, 0x2 ); // AU_coding_format

    bs_write( &q, 4, pes->frame_type );  // AU_coding_type_information
    bs_write( &q, 2, pes->ref_pic_idc ); // AU_ref_pic_idc
    if( stream->stream_format == LIBMPEGTS_VIDEO_MPEG2 )
        bs_write( &q, 2, pes->pic_struct );  // AU_pic_struct
    else if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
        bs_write( &q, 2, 0 ); // AU_coding_format

    bs_write1( &q, 1 ); // AU_PTS_present_flag
    bs_write1( &q, 1 ); // AU_profile_info_present_flag
    bs_write1( &q, 1 ); // AU_stream_info_present_flag
    bs_write1( &q, 0 ); // AU_trick_mode_info_present_flag

    bs_write32( &q, (pes->pts * 300) & 0xffffffff ); // AU_PTS_32

    bs_write( &q, 4, 0 ); // reserved
    bs_write( &q, 4, stream->dvb_au_frame_rate ); // AU_frame_rate_code

    bs_write( &q, 8, stream->mpegvideo_ctx->profile & 0xff ); // profile_idc

    if( stream->stream_format == LIBMPEGTS_VIDEO_AVC )
    {
        bs_write1( &q, stream->mpegvideo_ctx->profile == AVC_BASELINE ); // constraint_set0_flag
        bs_write1( &q, stream->mpegvideo_ctx->profile <= AVC_MAIN );     // constraint_set1_flag
        bs_write1( &q, 0 );                                              // constraint_set2_flag
        if( stream->mpegvideo_ctx->level == 9 && stream->mpegvideo_ctx->profile <= AVC_MAIN ) // level 1b
            bs_write1( &q, 1 );                                           // constraint_set3_flag
        else if( stream->mpegvideo_ctx->profile == AVC_HIGH_10_INTRA   ||
                 stream->mpegvideo_ctx->profile == AVC_CAVLC_444_INTRA ||
                 stream->mpegvideo_ctx->profile == AVC_HIGH_444_INTRA )
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


