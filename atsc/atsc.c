/*****************************************************************************
 * atsc.c : ATSC specific functions
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
#include "atsc.h"

/* Second Loop of PMT */
void write_atsc_ac3_descriptor( bs_t *s )
{
    bs_write( s, 8, ATSC_AC3_DESCRIPTOR_TAG );
}

/* EIT */
// FIXME fill in the blanks
void write_caption_service_descriptor( bs_t *s )
{
    bs_write( s, 8, ATSC_CAPTION_SERVICE_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0 ); // descriptor_length
    bs_write( s, 3, 0x7 ); // reserved

    bs_write( s, 5, 0 ); // number_of_services

    for( int i = 0; i < 0; i++ )
    {
        bs_write( s, 8, 0 ); // language[0]
        bs_write( s, 8, 0 ); // language[1]
        bs_write( s, 8, 0 ); // language[2]
        bs_write1( s, 0 );   // digital_cc
        bs_write1( s, 1 );   // reserved

        if( 0 )
            bs_write( s, 6, 0x3f ); // reserved
        else
            bs_write( s, 6, 0 ); // caption_service_number

        bs_write1( s, 0 ); // easy_reader
        bs_write1( s, 0 ); // wide_aspect_ratio
        bs_write( s, 13, 0x3fff ); // reserved
    }
}
