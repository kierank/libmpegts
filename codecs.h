/*****************************************************************************
 * codecs.h : Codec specific headers
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

#ifndef LIBMPEGTS_CODECS_H
#define LIBMPEGTS_CODECS_H

typedef struct {
    int level_idc;
    int bitrate;      /* max bitrate (kbit/sec) */
    int vbv;          /* max vbv buffer (kbit) */
} mpeg2_level_t;

typedef struct {
    int level_idc;
    int bitrate;     /* max bitrate (kbit/sec) */
    int cpb;         /* max vbv buffer (kbit) */
} h264_level_t;

const h264_level_t h264_levels[];

/* AC3 buffer sizes */
#define AC3_BS_ATSC 2592*8
#define AC3_BS_DVB 5696*8

/* ISO/IEC 13818-7 ADTS audio */
#define MPEG2_AAC_MAX_CHNLS_2_RXN  2000000
#define MPEG2_AAC_MAX_CHNLS_8_RXN  5529600
#define MPEG2_AAC_MAX_CHNLS_12_RXN 8294400
#define MPEG2_AAC_MAX_CHNLS_48_RXN 33177600 

#define MPEG2_AAC_MAX_CHNLS_2_BSN  3584*8
#define MPEG2_AAC_MAX_CHNLS_8_BSN  8976*8
#define MPEG2_AAC_MAX_CHNLS_12_BSN 12804*8
#define MPEG2_AAC_MAX_CHNLS_48_BSN 51216*8 

/* SMPTE 302M */
#define SMPTE_302M_AUDIO_BS 65024*8
#define SMPTE_302M_AUDIO_SR 48000

/* Misc */
#define MISC_AUDIO_BS 3584*8
#define MISC_AUDIO_RXN 2000000

#endif
