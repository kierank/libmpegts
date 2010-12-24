/*****************************************************************************
 * libmpegts.h : libmpegts public API
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

#ifndef LIBMPEGTS_H
#define LIBMPEGTS_H

#if !defined(_STDINT_H) && !defined(_STDINT_H_) && \
    !defined(_INTTYPES_H) && !defined(_INTTYPES_H_)
# ifdef _MSC_VER
#  pragma message("You must include stdint.h or inttypes.h before libmpegts.h")
# else
#  warning You must include stdint.h or inttypes.h before libmpegts.h
# endif
#endif

/**** Version ****/
#define LIBMPEGTS_API_VERSION_MAJOR 0
#define LIBMPEGTS_API_VERSION_MINOR 1

/**** Stream Formats ****/
/* Generic */
#define LIBMPEGTS_VIDEO_MPEG2 1
#define LIBMPEGTS_VIDEO_H264  2

#define LIBMPEGTS_AUDIO_MPEG1 32
#define LIBMPEGTS_AUDIO_MPEG2 33
#define LIBMPEGTS_AUDIO_ADTS  34
#define LIBMPEGTS_AUDIO_LATM  35
#define LIBMPEGTS_AUDIO_AC3   36
#define LIBMPEGTS_AUDIO_EAC3  37
#define LIBMPEGTS_AUDIO_LPCM  38 /* Blu-Ray only */
#define LIBMPEGTS_AUDIO_DTS   39

/* Blu-Ray Stream Formats */
#define LIBMPEGTS_AUDIO_DOLBY_LOSSLESS      40 /* "Dolby Lossless audio stream for Primary audio" */
#define LIBMPEGTS_AUDIO_DTS_HD              41 /* "DTS-HD audio stream except XLL for Primary audio" */
#define LIBMPEGTS_AUDIO_DTS_HD_XLL          42 /* "DTS-HD audio stream XLL for Primary audio" */
#define LIBMPEGTS_AUDIO_EAC3_SECONDARY      43 /* "Dolby Digital Plus audio stream for Secondary audio" */
#define LIBMPEGTS_AUDIO_DTS_HD_SECONDARY    44 /* "DTS-HD LBR audio stream for Secondary audio" */
#define LIBMPEGTS_SUB_PRESENTATION_GRAPHICS 45
#define LIBMPEGTS_SUB_INTERACTIVE_GRAPHICS  46
#define LIBMPEGTS_SUB_TEXT                  47

/* SMPTE Stream Formats */
#define LIBMPEGTS_AUDIO_302M  96

/* DVB Stream Formats */
#define LIBMPEGTS_SUB_DVB     128

/* Misc */
#define LIBMPEGTS_TELETEXT    160

/* Stream IDs */
/* SMPTE 302M, AC3 and DVB subtitles use Private Stream 1 */
#define LIBMPEGTS_STREAM_ID_PRIVATE_1  0xbd 
#define LIBMPEGTS_STREAM_ID_PRIVATE_2  0xbf

/* MPEG Audio (all types): 0xc0-0xdf */
#define LIBMPEGTS_STREAM_ID_MPEGAUDIO  0xc0

/* MPEG Video (all types): 0xe0-0xef */
#define LIBMPEGTS_STREAM_ID_MPEGVIDEO  0xe0

/**** Blu-Ray Information ****/
/* Blu-Ray Aspect Ratios */
#define LIBMPEGTS_HDMV_AR_4_3          2
#define LIBMPEGTS_HDMV_AR_16_9         3

/* Blu-Ray Video Formats */
#define LIBMPEGTS_HDMV_480I            1
#define LIBMPEGTS_HDMV_576I            2
#define LIBMPEGTS_HDMV_480P            3
#define LIBMPEGTS_HDMV_1080I           4
#define LIBMPEGTS_HDMV_720P            5
#define LIBMPEGTS_HDMV_1080P           6
#define LIBMPEGTS_HDMV_576P            7

/* DVB-AU AND Blu-Ray Frame Rate */
#define LIBMPEGTS_DVB_AU_23_976_FPS    1
#define LIBMPEGTS_DVB_AU_24_FPS        2
#define LIBMPEGTS_DVB_AU_25_FPS        3
#define LIBMPEGTS_DVB_AU_29_97_FPS     4
#define LIBMPEGTS_DVB_AU_30_FPS        5 /* Not allowed in Blu-Ray */
#define LIBMPEGTS_DVB_AU_50_FPS        6
#define LIBMPEGTS_DVB_AU_59_94_FPS     7
#define LIBMPEGTS_DVB_AU_60_FPS        8 /* Not allowed in Blu-Ray */

/**** DVB AU_information ****/
/* DVB-AU Pic-Struct */
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_NONE      1
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_TOP_FIELD 2
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_BOT_FIELD 3

/* DVB-AU Coding type MPEG-2 */
#define LIBMPEGTS_CODING_TYPE_I 1
#define LIBMPEGTS_CODING_TYPE_P 2
#define LIBMPEGTS_CODING_TYPE_B 3

/* DVB-AU Coding type H.264 */
#define LIBMPEGTS_CODING_TYPE_SLICE_IDR (1<<3)
#define LIBMPEGTS_CODING_TYPE_SLICE_I   (1<<2)
#define LIBMPEGTS_CODING_TYPE_SLICE_P   (1<<1)
#define LIBMPEGTS_CODING_TYPE_SLICE_B   (1<<0)

/* TS types: Packetised Elementary Stream,
 *           Transport Stream, DVB, ATSC, CableLabs, ISDB, Generic (188 bytes),
 *           Blu-Ray HDMV (192 bytes) */
enum ts_type_t
{
    TS_TYPE_DVB,
    TS_TYPE_ATSC,
    TS_TYPE_CABLELABS,
    TS_TYPE_ISDB,
    TS_TYPE_GENERIC,
    TS_TYPE_BLU_RAY,
};

/**** Levels and Profiles ****/
/* MPEG-2 Levels */
enum mpeg2_level_t
{
    MPEG2_LEVEL_LOW,
    MPEG2_LEVEL_MAIN,
    MPEG2_LEVEL_HIGH_1440,
    MPEG2_LEVEL_HIGH,
    MPEG2_LEVEL_HIGHP,
};

/* MPEG-2 Profiles */
enum mpeg2_profile_t
{
    MPEG2_PROFILE_SIMPLE,
    MPEG2_PROFILE_MAIN,
    MPEG2_PROFILE_422,
};

/* H264 Profiles */
enum h264_profile_t
{
    H264_BASELINE,
    H264_MAIN,
    H264_HIGH,
    H264_HIGH_10,
    H264_HIGH_422,
    H264_HIGH_444_PRED,
    H264_HIGH_10_INTRA,
    H264_HIGH_422_INTRA,
    H264_HIGH_444_INTRA,
    H264_CAVLC_444_INTRA,
};

/* Opaque Structure */
typedef struct ts_writer_t ts_writer_t;

/* General Stream Information
 *
 * PID
 * stream_format - Use stream formats above
 * stream_id - See Table 2-22 in ISO 13818-1
 * lang_code - ISO 639 Part 2 Language code (some non-standard codes exist)
 * * has_stream_identifier - Set to 1 if stream identifier is present
 * * stream_identifier - Stream identifier value FIXME all
 *
 * dvb_au - write DVB AU_information elements (video streams only)
 * dvb_au_frame_rate - DVB AU_information frame rate code (see above #defines)
 *
 * hdmv_frame_rate - For H.264 "Frame-rate = time_scale/num_units_in_tick/2" TODO MPEG-2 (see above #defines)
 * hdmv_aspect_ratio - either LIBMPEGTS_HDMV_AR_4_3 or LIBMPEGTS_HDMV_AR_16_9
 * hdmv_video_format - Video format (see above #defines)
 * */

typedef struct
{
    int pid;
    int stream_format;
    int stream_id;

    int write_lang_code;
    char lang_code[4];

    int dvb_au;
    int dvb_au_frame_rate;

    int hdmv_frame_rate;
    int hdmv_aspect_ratio;
    int hdmv_video_format;
} ts_stream_t;

/**** Stream attributes (DVB) ****/

#define DVB_SERVICE_TYPE_DIGITAL_TELEVISION  0x01
#define DVB_SERVICE_TYPE_DIGITAL_RADIO_SOUND 0x02
#define DVB_SERVICE_TYPE_TELETEXT            0x03
#define DVB_SERVICE_TYPE_ADVANCED_CODEC_SD   0x16
#define DVB_SERVICE_TYPE_ADVANCED_CODEC_HD   0x19
/***** Additional Codec-Specific functions *****/
/* Some formats may require extra information. Setup the relevant information using the following functions */

/* Video */
/* Setup / Update MPEG Video
 * Mandatory before writing any MPEG Video Stream.
 *
 * MPEG-2 Video Stream
 *
 * level - MPEG-2 level
 * profile - MPEG-2 profile
 * vbv_maxrate - maximum bitrate into the vbv in bits/s
 * vbv_bufsize - vbv buffer size
 * frame_rate - MPEG-2 framerate code
 *
 * AVC Stream
 *
 * level_idc / profile_idc are as defined in the H.264 Specification.
 * vbv_maxrate - maximum bitrate into the vbv in bits/s
 * vbv_bufsize - vbv buffer size
 * frame_rate - not used in AVC
 *
 * The only supported H.264 files are those with a Buffering Period SEI at each keyframe but nal_hrd_parameters_present set to 0.
 * This is owing to bugs/inconsistencies in the TS specification. This does not apply to Blu-Ray.
 */
/* Setup / Update SMPTE 302M Stream
 * Mandatory before writing any 302M Stream.
 *
 * bit_depth - 16, 20 or 24 bits per sample.
 * num_channels - 2, 4, 6 or 8 channels.
 *
 * As per SMPTE 302M the sample rate can only be 48000Hz.
 * Notes: The PTS of a SMPTE 302M frame shall be within 2ms of the corresponding video frame
 *        It is the responsibility of the calling application to encapsulate the SMPTE 302M data. */

int ts_setup_302m_stream( ts_writer_t *w, int pid, int bit_depth, int num_channels );
#endif
