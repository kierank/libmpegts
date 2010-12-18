/*****************************************************************************
 * dvb.h : DVB specific headers
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

#ifndef LIBMPEGTS_DVB_H
#define LIBMPEGTS_DVB_H

/* Descriptor Tags */
#define STREAM_IDENTIFIER_DESCRIPTOR_TAG 0x52
#define DVB_AC3_DESCRIPTOR_TAG  0x6a
#define DVB_EAC3_DESCRIPTOR_TAG 0x7a

/* PIDs */
#define SDT_PID         0x0011
#define EIT_PID         0x0012
#define TDT_PID         0x0014

/* TIDs */
#define SDT_TID         0x42
#define EIT_TID         0x4e
#define TDT_TID         0x70
/* Private Data Bytes data_field_tags */
#define ANNOUNCEMENT_SWITCHING_DATA_FIELD 0x01
#define AU_INFORMATION_DATA_FIELD         0x02
#define PVR_ASSIST_INFORMATION_DATA_FIELD 0x03

void write_stream_identifier_descriptor( bs_t *s, uint8_t stream_identifier );
void write_dvb_au_information( ts_writer_t *w, bs_t *s, ts_int_stream_t *stream, ts_int_frame_t *frame );
