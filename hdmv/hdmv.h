/*****************************************************************************
 * hdmv.h : HDMV specific headers
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

#ifndef LIBMPEGTS_HDMV_H
#define LIBMPEGTS_HDMV_H

/* Blu-Ray specific stream types */
#define AUDIO_LPCM                     0x80
#define AUDIO_DTS                      0x82
#define AUDIO_DOLBY_LOSSLESS           0x83
#define AUDIO_DTS_HD                   0x85
#define AUDIO_DTS_HD_XLL               0x86
#define AUDIO_EAC3_SECONDARY           0xa1
#define AUDIO_DTS_HD_SECONDARY         0xa2
#define SUB_PRESENTATION_GRAPHICS      0x90
#define SUB_INTERACTIVE_GRAPHICS       0x91
#define SUB_TEXT                       0x92

/* Descriptor Tags */
#define HDMV_PARTIAL_TS_DESCRIPTOR_TAG 0x63
#define HDMV_AC3_DESCRIPTOR_TAG        0x81
#define HDMV_CAPTION_DESCRIPTOR_TAG    0x86
#define HDMV_COPY_CTRL_DESCRIPTOR_TAG  0x88

void write_hdmv_copy_control_descriptor( ts_writer_t *w, bs_t *s );
void write_hdmv_video_registration_descriptor( bs_t *s, ts_int_stream_t *stream );
void write_hdmv_lpcm_descriptor( bs_t *s, ts_int_stream_t *stream );
void write_partial_ts_descriptor( ts_writer_t *w, bs_t *s );

#endif
