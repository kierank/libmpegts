/*****************************************************************************
 * common.h : common data
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

#ifndef LIBMPEGTS_COMMON_H
#define LIBMPEGTS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#if HAVE_STDINT_H
#include <stdint.h>
#else
#include <inttypes.h>
#endif
#include "bitstream.h"
#include "libmpegts.h"
#include <string.h>

/* Standardised Audio/Video stream_types */
#define VIDEO_MPEG2       0x02
#define VIDEO_AVC         0x1b

#define AUDIO_MPEG1       0x03
#define AUDIO_MPEG2       0x04
#define AUDIO_ADTS        0x0f
#define AUDIO_LATM        0x11

#define PRIVATE_SECTION   0x05
#define PRIVATE_DATA      0x06

#define TS_HEADER_SIZE 4
#define TS_PACKET_SIZE 188
#define TS_CLOCK       27000000LL
#define TS_START       10
#define TIMESTAMP_CLOCK 90000LL

// arbitrary
#define MAX_PROGRAMS   100
#define MAX_STREAMS    100

/* DVB 40ms recommendation */
#define PCR_MAX_RETRANS_TIME 40
#define PAT_MAX_RETRANS_TIME 100

/* PIDs */
#define PAT_PID         0x0000
#define NIT_PID         0x0010
#define SIT_PID         0x001f
#define NULL_PID        0xffff

/* TIDs */
#define PAT_TID         0x00
#define PMT_TID         0x02
#define NIT_TID         0x40
#define SIT_TID         0x7f

/* NIT */
/* ETSI TS 101 162 - Temporary Private Use
 * Used for both "Original Network ID" and "Network ID" */
#define DEFAULT_NID     0xff01

/* Program and Program Element Descriptor Tags */
#define VIDEO_STREAM_DESCRIPTOR_TAG          0x2
#define AUDIO_STREAM_DESCRIPTOR_TAG          0x3
#define REGISTRATION_DESCRIPTOR_TAG          0x5
#define DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG 0x6
#define ISO_693_LANGUAGE_DESCRIPTOR_TAG      0xa
#define PRIVATE_DATA_DESCRIPTOR_TAG          0xe
#define SMOOTHING_BUFFER_DESCRIPTOR_TAG      0x10
#define AVC_DESCRIPTOR_TAG                   0x28
#define MPEG2_AAC_AUDIO_DESCRIPTOR           0x2b
#define SVC_EXTENSION_DESCRIPTOR_TAG         0x30
#define MVC_EXTENSION_DESCRIPTOR_TAG         0x31
#define USER_DEFINED_DESCRIPTOR_TAG          0xc4

#define TB_SIZE       4096
#define RX_SYS        1000000
#define R_SYS_DEFAULT 80000

/* Macros */
#define BOOLIFY(x) x = !!x
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )

#define IS_VIDEO(x) ( x->stream_format == LIBMPEGTS_VIDEO_MPEG2 || x->stream_format == LIBMPEGTS_VIDEO_AVC )

/* Internal Program & Stream Structures */
typedef struct
{
    int level;
    int profile;
    int frame_rate;
} mpegvideo_stream_ctx_t;

typedef struct
{
    int num_channels;
    int sample_rate;
    int bits_per_sample;
} lpcm_stream_ctx_t;

typedef struct
{
    int frame_rate;
    int aspect_ratio;
} hdmv_video_stream_ctx_t;

typedef struct
{
    int sample_rate_code;
    int bsid;
    int bit_rate_code;
    int surround_mode;
    int bsmod;
    int num_channels;
} ts_atsc_ac3_info;

/* Blu-Ray DTCP */
typedef struct
{
    uint8_t byte_1;
    uint8_t byte_2;
} ts_dtcp_t;

typedef struct
{
    /* in bytes */
    int adapt_field_size;
    int pes_header_size;
    int cur_pos;
} buffer_queue_t;

typedef struct
{
    int buf_size; /* size of buffer */
    int cur_buf;  /* current buffer fill */

    double last_byte_removal_time;

    buffer_queue_t queued_packets[10];
} buffer_t;

typedef struct
{
    int pid;
    int cc;
    int stream_format; /* internal stream format type */
    int stream_type;   /* stream_type syntax element */
    int stream_id;

    int version_number;

    int64_t last_pkt_pcr;

    /* Stream contexts */
    mpegvideo_stream_ctx_t  *mpegvideo_ctx;
    lpcm_stream_ctx_t       *lpcm_ctx;
    ts_atsc_ac3_info        *atsc_ac3_ctx;

    int                     num_dvb_sub;
    ts_dvb_sub_t            *dvb_sub_ctx;

    int                     num_dvb_ttx;
    ts_dvb_ttx_t            *dvb_ttx_ctx;

    int                     num_dvb_vbi;
    ts_dvb_vbi_t            *dvb_vbi_ctx;

    int num_channels;
    int max_frame_size;

    /* T_STD */
    buffer_t tb; /* transport buffer */
    int rx;      /* flow from transport to multiplex buffer (video) or main buffer (audio) */
    buffer_t mb; /* multiplex buffer (video) or main buffer (audio) */
    buffer_t eb; /* elementary buffer */
    int rbx;     /* flow from multiplex to elementary buffer (video) */

    /* Language Codes */
    int write_lang_code;
    char lang_code[4];
    int audio_type;

    /* AAC */
    int aac_is_mpeg4;
    int aac_profile;
    int aac_channel_map;

    /* ATSC */

    /* DVB */
    /* Stream Identifier */
    int has_stream_identifier;
    int stream_identifier;

    /* DVB AU_Information */
    int dvb_au;
    int dvb_au_frame_rate;

    /* ISDB */

    /* CableLabs */

    /* Blu-Ray */
    int hdmv_video_format;
    int hdmv_frame_rate;
    int hdmv_aspect_ratio;
} ts_int_stream_t;

typedef struct
{
    uint8_t *data;
    int size;
    uint8_t *cur_pos;
    int bytes_left;

    /* stream context associated with pes */
    ts_int_stream_t *stream;

    int header_size;
    int random_access;
    int priority;

    int64_t initial_arrival_time;
    int64_t final_arrival_time;
    int64_t dts;
    int64_t pts;

    /* DVB AU_Information specific fields */
    uint8_t frame_type;
    int ref_pic_idc;
    int write_pulldown_info;
    int pic_struct;
} ts_int_pes_t;

typedef struct
{
    ts_int_stream_t pmt;
    int program_num;

    int num_queued_pmt;
    uint8_t **pmt_packets;

    int num_streams;
    ts_int_stream_t *streams[MAX_STREAMS];
    ts_int_stream_t *pcr_stream;

    int pmt_version;

    uint64_t last_pcr;

    int64_t video_dts;

    sdt_program_ctx_t sdt_ctx;
    int is_3dtv;

    int sb_leak_rate;
    int sb_size;
} ts_int_program_t;

struct ts_writer_t
{
    struct
    {
        int         i_bitstream;
        uint8_t     *p_bitstream;
        bs_t        bs;
    } out;

    uint64_t bytes_written;
    uint64_t packets_written;
    uint64_t pcr_start;

    int ts_type;
    int ts_id;

    int cbr;
    int ts_muxrate;

    int pat_cc;

    int num_programs;
    ts_int_program_t *programs[MAX_PROGRAMS];

    int pat_period;
    int pcr_period;
    int sdt_period;
    int first_input;

    int pat_version;

    int network_pid;
    int network_id;

    int num_buffered_frames;
    ts_int_pes_t **buffered_frames;

    int num_pcrs;
    int pcr_list_alloced;
    int64_t *pcr_list;

    /* system control */
    buffer_t tb;     /* transport buffer */
    buffer_t main_b; /* main buffer */

    int rx_sys;      /* flow from transport to main buffer */
    int r_sys;       /* flow from main buffer to system decoder */

    /* CableLabs */
    int legacy_constraints;

    /* DVB-specific */
    ts_int_stream_t *nit;
    ts_int_stream_t *sdt;
    ts_int_stream_t *eit;
    ts_int_stream_t *tdt;
    ts_int_stream_t *sit;

    uint64_t last_pat;
    uint64_t last_pmt;
    uint64_t last_nit;
    uint64_t last_sdt;
    uint64_t last_eit;
    uint64_t last_tdt;
    uint64_t last_sit;

    ts_dtcp_t *dtcp_ctx;
};

enum adaptation_field_control_e
{
    PAYLOAD_ONLY = 1,
    ADAPT_FIELD_ONLY = 2,
    ADAPT_FIELD_AND_PAYLOAD = 3,
};

void write_bytes( bs_t *s, uint8_t *bytes, int length );
void write_packet_header( ts_writer_t *w, bs_t *s, int start, int pid, int adapt_field, int *cc );
void write_registration_descriptor( bs_t *s, int descriptor_tag, int descriptor_length, char *format_id );
void write_crc( bs_t *s, int start );
int write_padding( bs_t *s, int start );
int increase_pcr( ts_writer_t *w, int num_packets, int imaginary );
ts_int_stream_t *find_stream( ts_writer_t *w, int pid );

#endif
