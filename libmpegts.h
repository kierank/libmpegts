/*****************************************************************************
 * libmpegts.h : libmpegts public API
 *****************************************************************************
 * Copyright (C) 2010-11 Kieran Kunhya
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
#define LIBMPEGTS_API_VERSION_MINOR 3

/**** Stream Formats ****/
/* Generic */
#define LIBMPEGTS_VIDEO_MPEG2 1
#define LIBMPEGTS_VIDEO_AVC   2
#define LIBMPEGTS_VIDEO_DIRAC 3

#define LIBMPEGTS_AUDIO_MPEG1 32
#define LIBMPEGTS_AUDIO_MPEG2 33
#define LIBMPEGTS_AUDIO_ADTS  34 /* Usually MPEG-2 AAC */
#define LIBMPEGTS_AUDIO_LATM  35 /* Usually MPEG-4 AAC */
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
#define LIBMPEGTS_ANCILLARY_RDD11 97
#define LIBMPEGTS_ANCILLARY_2038  98

/* DVB Stream Formats */
#define LIBMPEGTS_DVB_SUB      128
#define LIBMPEGTS_DVB_TELETEXT 129
#define LIBMPEGTS_DVB_VBI      130

/* Misc */
#define LIBMPEGTS_AUDIO_OPUS   160
#define LIBMPEGTS_DATA_SCTE35  161

/**** Stream IDs ****/
/* SMPTE 302M, AC3, DVB subtitles and Teletext use Private Stream 1 */
#define LIBMPEGTS_STREAM_ID_PRIVATE_1  0xbd
#define LIBMPEGTS_STREAM_ID_PRIVATE_2  0xbf

/* MPEG Audio (all types): 0xc0-0xdf */
#define LIBMPEGTS_STREAM_ID_MPEGAUDIO  0xc0

/* MPEG Video (all types): 0xe0-0xef */
#define LIBMPEGTS_STREAM_ID_MPEGVIDEO  0xe0

/* Extended Stream-id */
#define LIBMPEGTS_STREAM_ID_EXTENDED   0xfd

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
/* DVB-AU Pic-Struct MPEG-2 */
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_NONE      1
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_TOP_FIELD 2
#define LIBMPEGTS_DVB_AU_PIC_STRUCT_BOT_FIELD 3

/* DVB-AU Coding type MPEG-2 */
#define LIBMPEGTS_CODING_TYPE_I        1
#define LIBMPEGTS_CODING_TYPE_P        2
#define LIBMPEGTS_CODING_TYPE_B        3

/* DVB-AU Coding type H.264 */
#define LIBMPEGTS_CODING_TYPE_SLICE_IDR (1<<3)
#define LIBMPEGTS_CODING_TYPE_SLICE_I   (1<<2)
#define LIBMPEGTS_CODING_TYPE_SLICE_P   (1<<1)
#define LIBMPEGTS_CODING_TYPE_SLICE_B   (1<<0)

/**** Audio Service Type ****/
#define LIBMPEGTS_AUDIO_SERVICE_UNDEFINED        0
#define LIBMPEGTS_AUDIO_SERVICE_CLEAN_EFFECTS    1
#define LIBMPEGTS_AUDIO_SERVICE_HEARING_IMPAIRED 2
#define LIBMPEGTS_AUDIO_SERVICE_VISUAL_IMPAIRED  3

/* TS types: Packetised Elementary Stream,
 *           Transport Stream, DVB, ATSC, CableLabs, ISDB, Generic (188 bytes),
 *           Blu-Ray HDMV (192 bytes) */
enum ts_type_t
{
    TS_TYPE_GENERIC,
    TS_TYPE_DVB,
    TS_TYPE_CABLELABS,
    TS_TYPE_ATSC,
    TS_TYPE_ISDB,
    TS_TYPE_BLU_RAY,
};

/**** Levels and Profiles ****/
/* MPEG-2 Levels */
enum mpeg2_level_t
{
    LIBMPEGTS_MPEG2_LEVEL_LOW = 1,
    LIBMPEGTS_MPEG2_LEVEL_MAIN,
    LIBMPEGTS_MPEG2_LEVEL_HIGH_1440,
    LIBMPEGTS_MPEG2_LEVEL_HIGH,
    LIBMPEGTS_MPEG2_LEVEL_HIGHP,
};

/* MPEG-2 Profiles */
enum mpeg2_profile_t
{
    LIBMPEGTS_MPEG2_PROFILE_SIMPLE,
    LIBMPEGTS_MPEG2_PROFILE_MAIN,
    LIBMPEGTS_MPEG2_PROFILE_422,
};

/* AVC Profiles */
enum avc_profile_t
{
    AVC_BASELINE,
    AVC_MAIN,
    AVC_HIGH,
    AVC_HIGH_10,
    AVC_HIGH_422,
    AVC_HIGH_444_PRED,
    AVC_HIGH_10_INTRA,
    AVC_HIGH_422_INTRA,
    AVC_HIGH_444_INTRA,
    AVC_CAVLC_444_INTRA,
};

/* Opaque Structure */
typedef struct ts_writer_t ts_writer_t;

// TODO make certain syntax elements updatable
/* General Stream Information
 *
 * PID
 * stream_format - Use stream formats above
 * stream_id - See Table 2-22 in ISO 13818-1
 *
 * audio_frame_size - size of one audio frame in 90KHz ticks. (e.g. for ac3 1536 * 90000/samplerate )
 *
 * write_lang_code - Write ISO 639 descriptor for audio
 * lang_code - ISO 639 Part 2 Language code (or non-standard codes)
 * audio_type - Audio service type
 *
 * has_stream_identifier - Set to 1 if stream identifier is present
 * stream_identifier - Stream identifier
 *
 * dvb_au - write DVB AU_information elements (video streams only)
 * dvb_au_frame_rate - DVB AU_information frame rate code (see above #defines)
 *
 * hdmv_frame_rate - For H.264 "Frame-rate = time_scale/num_units_in_tick/2" TODO MPEG-2 (see above #defines)
 * hdmv_aspect_ratio - either LIBMPEGTS_HDMV_AR_4_3 or LIBMPEGTS_HDMV_AR_16_9
 * hdmv_video_format - Video format (see above #defines) */

typedef struct
{
    int pid;
    int stream_format;
    int stream_id;

    int audio_frame_size;

    int write_lang_code;
    char lang_code[4];
    int audio_type;

    int has_stream_identifier;
    int stream_identifier;

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

/* SDT Stream Information
 *
 * service_type - See above
 * service_name - Self Explanatory
 * provider_name - Self Explanatory */

typedef struct
{
    int service_type;
    char *service_name;
    char *provider_name;
} sdt_program_ctx_t;

/* Program attributes:
 *
 * PIDs must be between 33 and 8190 (DVB)
 * program_num must be between 1 and 8190
 * PCR PID can be the same as a stream in the program (video PID or separate PID recommended)
 *
 * is_3dtv -
 * Write 3d_MPEG2_descriptor in PMT (CableLabs OC-SP-CEP3.0-I01-100827).
 * Stream MUST have appropriate MPEG-2 user_data or AVC SEI with 3D information.
 *
 * Smoothing Buffer (Required for ATSC) -
 * sb_leak_rate - smoothing buffer leak rate (in units of 400 bits/s)
 * sb_size - in bytes
 */
typedef struct
{
    int pmt_pid;
    int program_num;
    int pcr_pid;

    int num_streams;
    ts_stream_t *streams;

    int is_3dtv;

    /* ATSC */
    int sb_leak_rate;
    int sb_size;

    /* DVB */
    sdt_program_ctx_t sdt;
} ts_program_t;

/**** Functions ****/

/* Create Writer */
ts_writer_t *ts_create_writer( void );

/*
 * ts_id - Transport Stream ID
 * muxrate - Transport stream muxing rate
 * cbr - Pad to constant bitrate with null packets
 * ts_type - Type of transport stream to write
 * network_pid - PID of the network table (0 otherwise)
 * legacy_constraints - Comply with CableLabs legacy contraints in Section 7.3 of Content Encoding Profiles 3.0 Specification
 *
 * retransmit periods in milliseconds
 *
 * CURRENT LIMITATIONS
 *
 * Single Program Transport Streams only supported currently.
 * Only one video stream allowed.
 *
 *
 * */

typedef struct ts_main_t
{
    int num_programs;
    ts_program_t *programs;

    int ts_id;
    int muxrate;
    int cbr;
    int ts_type;
    int lowlatency;

    int network_pid;

    int legacy_constraints;

    int pcr_period;
    int pat_period;

    // FIXME dvb land
    int network_id;
    int sdt_period;
    int nit_period;
    int tdt_period;
    int tot_period;
} ts_main_t;

int ts_setup_transport_stream( ts_writer_t *w, ts_main_t *params );

/* update transport stream
 *
 * muxrate is the only tested parameter
 *
 * TODO: implement versioning so other parameters can be updated
 *
 */
void ts_update_transport_stream( ts_writer_t *w, ts_main_t *params );

/**** Additional Codec-Specific functions ****/
/* Many formats require extra information. Setup the relevant information using the following functions */

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
 * level_idc / profile_idc are as defined in the AVC Specification.
 * vbv_maxrate - maximum bitrate into the vbv in bits/s
 * vbv_bufsize - vbv buffer size
 * frame_rate - not used in AVC
 *
 * The only supported AVC files are those with a Buffering Period SEI at each keyframe but nal_hrd_parameters_present set to 0.
 * This is owing to bugs/inconsistencies in the TS specification. This does not apply to Blu-Ray. */

int ts_setup_mpegvideo_stream( ts_writer_t *w, int pid, int level, int profile, int vbv_maxrate, int vbv_bufsize, int frame_rate );

/* Audio */
#define LIBMPEGTS_MPEG2_AAC_MAIN_PROFILE 0
#define LIBMPEGTS_MPEG2_AAC_LC_PROFILE   1

#define LIBMPEGTS_MPEG2_AAC_1_CHANNEL    1
#define LIBMPEGTS_MPEG2_AAC_2_CHANNEL    2
#define LIBMPEGTS_MPEG2_AAC_3_CHANNEL    3
#define LIBMPEGTS_MPEG2_AAC_4_CHANNEL    4
#define LIBMPEGTS_MPEG2_AAC_5_CHANNEL    5
#define LIBMPEGTS_MPEG2_AAC_5_POINT_1_CHANNEL 6
#define LIBMPEGTS_MPEG2_AAC_7_POINT_1_CHANNEL 7

/* Setup / Update MPEG-2 AAC Stream
 *
 */

int ts_setup_mpeg2_aac_stream( ts_writer_t *w, int pid, int profile, int channel_map );

#define LIBMPEGTS_MPEG4_AAC_MAIN_PROFILE_LEVEL_1 0x10
#define LIBMPEGTS_MPEG4_AAC_MAIN_PROFILE_LEVEL_2 0x11
#define LIBMPEGTS_MPEG4_AAC_MAIN_PROFILE_LEVEL_3 0x12
#define LIBMPEGTS_MPEG4_AAC_MAIN_PROFILE_LEVEL_4 0x13
#define LIBMPEGTS_MPEG4_AAC_PROFILE_LEVEL_1      0x50
#define LIBMPEGTS_MPEG4_AAC_PROFILE_LEVEL_2      0x51
#define LIBMPEGTS_MPEG4_AAC_PROFILE_LEVEL_4      0x52
#define LIBMPEGTS_MPEG4_AAC_PROFILE_LEVEL_5      0x53
#define LIBMPEGTS_MPEG4_HE_AAC_PROFILE_LEVEL_2   0x58
#define LIBMPEGTS_MPEG4_HE_AAC_PROFILE_LEVEL_3   0x59
#define LIBMPEGTS_MPEG4_HE_AAC_PROFILE_LEVEL_4   0x5a
#define LIBMPEGTS_MPEG4_HE_AAC_PROFILE_LEVEL_5   0x5b
#define LIBMPEGTS_MPEG4_HE_AAC_V2_PROFILE_LEVEL_2 0x60
#define LIBMPEGTS_MPEG4_HE_AAC_V2_PROFILE_LEVEL_3 0x61
#define LIBMPEGTS_MPEG4_HE_AAC_V2_PROFILE_LEVEL_4 0x62
#define LIBMPEGTS_MPEG4_HE_AAC_V2_PROFILE_LEVEL_5 0x63

/* Setup / Update MPEG-4 AAC Stream
 * profile_and_level - self explanatory
 * num_channels - number of channels (excluding LFE channel)
 *
 * It is the responsibility of the calling application to encapsulate using ADTS or LATM
 */

int ts_setup_mpeg4_aac_stream( ts_writer_t *w, int pid, int profile_and_level, int num_channels );

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

/* Opus */

#define LIBMPEGTS_CHANNEL_CONFIG_DUAL_MONO 0x00
#define LIBMPEGTS_CHANNEL_CONFIG_MONO      0x01
#define LIBMPEGTS_CHANNEL_CONFIG_STEREO    0x02

int ts_setup_opus_stream( ts_writer_t *w, int pid, int channel_map );


/**** DVB Specific Information ****/

/* DVB Subtitles */
#define LIBMPEGTS_DVB_SUB_TYPE_EBU_TELETEXT            0x01
#define LIBMPEGTS_DVB_SUB_TYPE_ASSOCIATED_EBU_TELETEXT 0x02
#define LIBMPEGTS_DVB_SUB_TYPE_VBI_DATA                0x03
#define LIBMPEGTS_DVB_SUB_TYPE_NORMAL_NO_AR            0x10
#define LIBMPEGTS_DVB_SUB_TYPE_NORMAL_4_3_AR           0x11
#define LIBMPEGTS_DVB_SUB_TYPE_NORMAL_2_21_1_AR        0x13
#define LIBMPEGTS_DVB_SUB_TYPE_NORMAL_HD               0x14
#define LIBMPEGTS_DVB_SUB_TYPE_HEARING_NO_AR           0x20
#define LIBMPEGTS_DVB_SUB_TYPE_HEARING_4_3_AR          0x21
#define LIBMPEGTS_DVB_SUB_TYPE_HEARING_16_9_AR         0x22
#define LIBMPEGTS_DVB_SUB_TYPE_HEARING_HD              0x24

/* ts_dvb_sub_t
 *
 * lang_code - ISO 639 Part 2 Language code
 * subtitling_type - see above #defines
 * composition_page_id - composition page
 * ancillary_page_id - optional ancillary page (should be set to composition page otherwise)
 */

typedef struct
{
    char lang_code[4];
    int  subtitling_type;
    int  composition_page_id;
    int  ancillary_page_id;
} ts_dvb_sub_t;

/* ts_setup_dvb_subtitles
 *
 * has_dds - has display definition segment
 */

int ts_setup_dvb_subtitles( ts_writer_t *w, int pid, int has_dds, int num_subtitles, ts_dvb_sub_t *subtitles );

/* DVB Teletext */
#define LIBMPEGTS_DVB_TTX_TYPE_INITIAL 0x01
#define LIBMPEGTS_DVB_TTX_TYPE_SUB     0x02
#define LIBMPEGTS_DVB_TTX_TYPE_ADDITIONAL_INFO 0x03
#define LIBMPEGTS_DVB_TTX_TYPE_SCHEDULE        0x04
#define LIBMPEGTS_DVB_TTX_TYPE_SUB_HEARING_IMP 0x05

/* ts_dvb_ttx_t
 * lang_code - ISO 639 Part 2 Language code
 * teletext_type - see above #defines
 * teletext_magazine_number - self explanatory
 * teletext_page_number - self explanatory
 */

typedef struct
{
    char lang_code[4];
    int  teletext_type;
    int  teletext_magazine_number;
    int  teletext_page_number;
} ts_dvb_ttx_t;

int ts_setup_dvb_teletext( ts_writer_t *w, int pid, int num_teletexts, ts_dvb_ttx_t *teletexts );

/* ts_dvb_vbi_t
 * field_parity - 1 for first field (odd), 0 for second field (even)
 * line_offset - line number on which data is presented if it is transcoded into the VBI
 */
typedef struct
{
    int field_parity;
    int line_offset;
} ts_dvb_vbi_line_t;

/* DVB service-ids */
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_TTX 0x01 /* Requires call to ts_setup_dvb_teletext */
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_INVERTED_TTX 0x02
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_VPS 0x04
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_WSS 0x05
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_CC  0x06
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_MONO_SAMPLES 0x07

/* SCTE-127 service-ids */
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_VITC 0xf7
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_COPY_PROTECTION 0xf9
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_TVG2X 0xfb
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_NABTS 0xfc
#define LIBMPEGTS_DVB_VBI_DATA_SERVICE_ID_AMOL_48_96 0xfe

/* ts_dvb_vbi_t
 * data_service_id - see above #defines
 * lines - one ts_dvb_vbi_line_t per line
 */

typedef struct
{
    int data_service_id;
    int num_lines;
    ts_dvb_vbi_line_t *lines;
} ts_dvb_vbi_t;

int ts_setup_dvb_vbi( ts_writer_t *w, int pid, int num_vbis, ts_dvb_vbi_t *vbis );

/* Service Description Table
 *
 * ts_setup_sdt enables SDT
 * ts_remove_sdt disables SDT
 *
 */

int ts_setup_sdt( ts_writer_t *w );
void ts_remove_sdt( ts_writer_t *w );

/* Network Information Table */

typedef struct
{

} ts_dvb_nit_t;

int ts_setup_nit( ts_writer_t *w );
void ts_update_nit( ts_writer_t *w );
void ts_remove_nit( ts_writer_t *w );

/* EIT
 * FIXME only now and next
 *  */

int ts_setup_eit( ts_writer_t *w );
void ts_update_eit( ts_writer_t *w );
void ts_remove_eit( ts_writer_t *w );

/* TDT
 *
 *  */

int ts_setup_tdt( ts_writer_t *w );
void ts_remove_tdt( ts_writer_t *w );

/* Time Offset Table
 *
 *  */

int ts_setup_tot( ts_writer_t *w );
void ts_remove_tot( ts_writer_t *w );

/**** DVB / Blu-Ray Tables ****/

/* SIT
 *
 *  */

int ts_setup_sit( ts_writer_t *w );
void ts_update_sit( ts_writer_t *w );
int ts_remove_sit( ts_writer_t *w );

/**** ATSC/CableLabs specific information ****/

/* ATSC Setup/Update AC3 stream
 *
 * Note: This channel status refers to channel map the current program being broadcast.
 *       If the channel changes from 5.1 to 2.0 for an advertising break, this
 *       need not be updated. On changing to a program with a different channel map,
 *       this descriptor should be updated.
 */

//int ts_setup_atsc_ac3_stream( ts_writer_t *w, int pid, ts_atsc_ac3_info *ac3_info );

/**** Cablelabs specific information ****/


/**** Blu-Ray specific information ****/

/* Setup HDMV LPCM Stream
 *
 * num_channels - number of audio channels
 * sample_rate  - sample rate in KHz
 * bits_per_sample - number of bits per sample
 *
 * NOTE: It is the responsibility of the calling application to write the appropriate LPCM headers
 */
int ts_setup_hdmv_lpcm_stream( ts_writer_t *w, int pid, int num_channels, int sample_rate, int bits_per_sample );

/* Digital Transmission Content Protection
 *
 * See Appendix B of DTCP Specification for information about DTCP_descriptor
 * byte_1 and byte_2 correspond to the two private_data_bytes in the DTCP_descriptor
 */
int ts_setup_dtcp( ts_writer_t *w, uint8_t byte_1, uint8_t byte_2 );

/* TODO: other relevant tables */

/* Writing frames to libmpegts
 *
 * The duration of a video frame and associated audio frames must be as close as possible.
 * The duration of audio frames can either be slighly less, equal to (rare), or slightly greater than
 * the video frame duration - libmpegts can handle all three conditions.
 * There should be no more than one frame with a DTS larger than that of the associated video stream.
 *
 * The DTS of a given PID must be monotonically increasing. Interleaving of frames from different streams is allowed.
 * Only a single video frame at a time must be written. */

/* ts_frame_t
 *
 * PID - Packet Identifier (i.e. which stream the payload is associated with)
 *
 * ** Video Only **
 *
 * CPB Initial Arrival Time - initial arrival time of picture in CPB (in 27MHz clock ticks)
 * CPB Final Arrival Time - final arrival time of picture in CPB (in 27MHz clock ticks)
 * DTS - Decode Time Stamp (in 90kHz clock ticks - maximum 30 bits)
 * PTS - Presentation Time Stamp (in 90kHz clock ticks - maximum 30 bits)
 * (PTS and DTS may have codec-specific meanings. See ISO 13818-1 for more information)
 * Generally, non-video formats have PTS equal to DTS.
 *
 * This data does not need to be wrapped around )
 * random_access - Data contains an "elementary stream access point"
 * priority - Indicate payload has priority
 * (random_access and priority can be codec specific. See ISO 13818-1 for more information.)
 *
 * DVB AU_information fields
 * frame_type - Single Value for MPEG-2, Bitfield for AVC (see above #defines)
 * ref_pic_idc - Set if frame is needed for reconstruction of other frames (MPEG-2), nal_ref_idc in AVC
 * write_pulldown_info - Write pulldown info in AU_Information
 * pic_struct - AVC pic_struct element - only used if write_pulldown_info set
 *
 * opaque - opaque pointer that libmpegts does nothing with
 */

typedef struct
{
    uint8_t *data;
    int size;
    int pid;
    int64_t cpb_initial_arrival_time;
    int64_t cpb_final_arrival_time;
    int64_t dts;
    int64_t pts;
    int64_t duration; /* SMPTE 302M only */
    int random_access;
    int priority;

    /* DVB AU_Information specific fields */
    uint8_t frame_type;
    int ref_pic_idc;
    int write_pulldown_info;
    int pic_struct;

    void *opaque;
} ts_frame_t;

/* ts_write_frames
 *
 * libmpegts buffers one frame so the last set of packets can be output by setting num_frames = 0.
 *
 * pcr_list contains an array of pcr values, one for each output packet. The array length is len/188.
 * NOTE: This PCR list does not wrap around
 *
 */

int ts_write_frames( ts_writer_t *w, ts_frame_t *frames, int num_frames, uint8_t **out, int *len, int64_t **pcr_list );

/* INACTIVE
 *
 * */
int ts_delete_stream( ts_writer_t *w, int pid );



int ts_close_writer( ts_writer_t *w );

/* Examples TODO */



#endif
