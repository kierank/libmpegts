/*****************************************************************************
 * atsc.h : ATSC specific headers
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

#ifndef LIBMPEGTS_ATSC_H
#define LIBMPEGTS_ATSC_H

/* ATSC stream_types */
#define AUDIO_AC3         0x81
#define AUDIO_EAC3        0x84

/* Descriptors */
#define ATSC_AC3_DESCRIPTOR_TAG              0x81
#define ATSC_CAPTION_SERVICE_DESCRIPTOR_TAG  0x86
#define ATSC_EXTENDED_CHANNEL_NAME_DESCRIPTOR_TAG 0xa0

void write_atsc_ac3_descriptor( bs_t *s );
void write_caption_service_descriptor( bs_t *s );

#endif
