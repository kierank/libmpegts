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
typedef struct
{
    int level;
    int profile;
    int bitrate;      /* max bitrate (kbit/sec) */
    int vbv;          /* max vbv buffer (kbit) */
} mpeg2_level_t;

const mpeg2_level_t mpeg2_levels[] =
{
    { MPEG2_LEVEL_LOW,      MPEG2_PROFILE_MAIN,   4000000,  475136 },
    { MPEG2_LEVEL_MAIN,     MPEG2_PROFILE_SIMPLE, 15000000, 1835008 },
    { MPEG2_LEVEL_MAIN,     MPEG2_PROFILE_MAIN,   15000000, 1835008 },
    { MPEG2_LEVEL_MAIN,     MPEG2_PROFILE_422,    50000000, 9437184 },
    { MPEG2_LEVEL_HIGH_1440,MPEG2_PROFILE_MAIN,   60000000, 7340732 },
    { MPEG2_LEVEL_HIGH,     MPEG2_PROFILE_MAIN,   80000000, 9781248 },
    { MPEG2_LEVEL_HIGHP,    MPEG2_PROFILE_MAIN,   80000000, 9781248 },
    { 0 }
};

typedef struct
{
    int level_idc;
    int bitrate;     /* max bitrate (kbit/sec) */
    int cpb;         /* max vbv buffer (kbit) */
} avc_level_t;

const avc_level_t avc_levels[] =
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

const uint8_t avc_profiles[] =
{
    [AVC_BASELINE] = 66,
    [AVC_MAIN]     = 77,
    [AVC_HIGH]     = 100,
    [AVC_HIGH_10]  = 110,
    [AVC_HIGH_422] = 122,
    [AVC_HIGH_444_PRED]   = 244,
    [AVC_HIGH_10_INTRA]   = 110,
    [AVC_HIGH_422_INTRA]  = 122,
    [AVC_HIGH_444_INTRA]  = 244,
    [AVC_CAVLC_444_INTRA] = 44,
};

/* Audio */
typedef struct
{
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

/** AC3 **/
/* AC3 buffer sizes */
#define AC3_BS_ATSC         2592*8
#define AC3_BS_DVB          5696*8

/* SMPTE 302M */
#define SMPTE_302M_AUDIO_BS 65024*8
#define SMPTE_302M_AUDIO_SR 48000

/* Misc Audio */
#define MISC_AUDIO_BS       3584*8
#define MISC_AUDIO_RXN      2000000

/* DVB Subtitles */
#define DVB_SUB_RXN         192000
#define DVB_SUB_MB_SIZE     24000
#define DVB_SUB_DDS_TB_SIZE TB_SIZE*2*8
#define DVB_SUB_DDS_RXN     400000
#define DVB_SUB_DDS_MB_SIZE 100000

/* Teletext */
#define TELETEXT_T_BS       480*8 /* Seems to have an odd Transport Buffer size */
#define TELETEXT_RXN        6750000
#define TELETEXT_BTTX       1504*8

/* SCTE VBI */
#define SCTE_VBI_RXN        324539
#define SCTE_VBI_MB_SIZE    2256*8

#endif
