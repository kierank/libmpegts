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
#define VIDEO_H264        0x1b

#define AUDIO_MPEG1       0x03
#define AUDIO_MPEG2       0x04
#define AUDIO_ADTS        0x0f
#define AUDIO_LATM        0x11

#define PRIVATE_SECTION   0x05
#define PRIVATE_DATA      0x06

#define TS_HEADER_SIZE 4
#define TS_PACKET_SIZE 188
#define TS_CLOCK       27000000LL
#define TS_START       0

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
#define SVC_EXTENSION_DESCRIPTOR_TAG         0x30
#define MVC_EXTENSION_DESCRIPTOR_TAG         0x31

#define TB_SIZE       4096
#define RX_SYS        1000000
#define R_SYS_DEFAULT 80000

/* Macros */
#define BOOLIFY(x) x = !!x
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )


#endif
