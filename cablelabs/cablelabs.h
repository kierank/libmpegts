/*****************************************************************************
 * cablelabs.h : CableLabs (includes SCTE) specific headers
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

#ifndef LIBMPEGTS_CABLELABS_H
#define LIBMPEGTS_CABLELABS_H

/* Descriptors */
#define CABLELABS_3D_MPEG2_DESCRIPTOR_TAG    0xe8

/* Private Data Bytes data_field_tags */
#define SCTE_ADAPTATION_FIELD_DESCRIPTOR_TAG 0x97

void write_cablelabs_3d_descriptor( bs_t *s );
void write_scte_adaptation_descriptor( bs_t *s );

#endif
