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

/* Video */
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

const h264_level_t h264_levels[] =
{
    { 10,     64,     64 },
    {  9,    128,    350 }, /* 1b */
    { 11,    192,    500 },
    { 12,    384,   1000 },
    { 13,    768,   2000 },
    { 20,   2000,   2000 },
    { 21,   4000,   4000 },
    { 22,   4000,   4000 },
    { 30,  10000,  10000 },
    { 31,  14000,  14000 },
    { 32,  20000,  20000 },
    { 40,  20000,  25000 },
    { 41,  50000,  62500 },
    { 42,  50000,  62500 },
    { 50, 135000, 135000 },
    { 51, 240000, 240000 },
    { 0 }
};

/* Audio */
typedef struct {
    int max_channels;
    int rxn;         /* Leak rate from Transport Buffer */
    int bsn;         /* Size of Main buffer */
} aac_buffer_t;

const aac_buffer_t aac_buffers[] =
{
    { 2,  2000000,  3584*8 },
    { 8,  5529600,  8976*8 },
    { 12, 8294400,  12804*8 },
    { 48, 33177600, 51216*8 },
    { 0 },
};

/* AC3 buffer sizes */
#define AC3_BS_ATSC 2592*8
#define AC3_BS_DVB 5696*8

/* SMPTE 302M */
#define SMPTE_302M_AUDIO_BS 65024*8
#define SMPTE_302M_AUDIO_SR 48000

/* Misc */
#define MISC_AUDIO_BS 3584*8
#define MISC_AUDIO_RXN 2000000

#endif
