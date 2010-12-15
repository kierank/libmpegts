/*****************************************************************************
 * cablelabs.c : CableLabs specific functions
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
#include "cablelabs.h"

/* First Loop of PMT */

/* 3D descriptor */
void write_cablelabs_3d_descriptor( bs_t *s )
{
    bs_write( s, 8, CABLELABS_3D_MPEG2_DESCRIPTOR ); // descriptor_tag
    bs_write( s, 8, 1 ); // descriptor_length
    bs_write1( s, 1 );   // 3d_frame_packing_data_present
    bs_write( s, 7, 0 ); // reserved
}

/* Adaptation Descriptor */
void write_scte_adaptation_descriptor( bs_t *s )
{
    bs_write( s, 8, SCTE_ADAPTATION_FIELD_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0 ); // descriptor_length
}
