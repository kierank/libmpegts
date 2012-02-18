/*****************************************************************************
 * smpte.c : SMPTE specific functions
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
#include "smpte.h"

void write_anc_data_descriptor( bs_t *s )
{
    bs_write( s, 8, USER_DEFINED_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0 );                           // descriptor_length

    /* Optional descriptors here */
}
