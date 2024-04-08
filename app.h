#ifndef _APP_H
#define _APP_H

#include "mculib_common.h"
#include "ms1826_api.h"
#include "ms1826_timing_table.h"
#include "font.h"


#define MS1826_4X4DEMO_SW_VERSION "MS1826_HD4X4_Demo_V1.2"

extern UINT8 const g_u8_rx_idx[4];
extern UINT8 const g_u8_csc_idx[4];   
extern UINT8 const g_u8_misc_module_rx[4];
extern UINT8 const g_u8_4ksdn_idx[4];
extern UINT8 const g_u8_sdn_idx[4];
extern UINT8 const g_u8_dsc_idx[4];
extern UINT8 const g_u8_dma_idx[4];
extern UINT8 const g_u8_dma_dsc_idx[4];
extern UINT8 const g_u8_misc_module_vipss[4];
extern UINT8 const g_u8_misc_module_vopss[4];
extern UINT8 const g_u8_su_idx[4];
extern UINT8 const g_u8_su_dsc_idx[4];
extern UINT8 const g_u8_mixer_idx[4];
extern UINT8 const g_u8_mixer_layer_src[4];
extern UINT8 const g_u8_mixer_layer_dsc_src[4];
extern UINT8 const g_u8_misc_mixer[4];
extern UINT8 const g_u8_misc_timgen[4];
extern UINT8 const g_u8_misc_su[4];
extern UINT8 const g_u8_misc_dsc_su[4];
extern UINT8 const g_u8_misc_module_tx[4];
extern UINT8 const g_u8_split_idx[4];
extern UINT8 const g_u8_ss_ch_idx[4];
extern UINT8 const g_u8_ss_ch_dsc_idx[4]; 
extern UINT8 const g_u8_vb_path[4];
extern UINT8 const g_u8_vb_dsc_path[4];
extern UINT8 const g_u8_vb_rx[4];
extern UINT8 const g_u8_vb_vdstmg[4];
extern UINT8 const g_u8_intsrc_rx[4];
extern UINT8 const g_u8_intsrc_rxvb[4]; 
extern UINT8 const g_u8_misc_clksrc[4];
extern UINT8 const g_u8_osd_idx[4];
extern UINT8 const g_u8_misc_osd_idx[4];
extern UINT8 const g_u8_i2s_idx[4];
extern UINT8 const g_u8_spdif_idx[4] ;
extern UINT8 const g_u8_i2s_audio_idx[4];
extern UINT8 const g_u8_spdif_audio_idx[4] ;


extern UINT8 const g_arrOutputTable[3];
extern UINT8 const g_u8_HD_RX_PROT[4];
extern UINT8 const g_u8_HD_TX_PROT[4];

extern UINT16 const str_osd_input_channel[4][8];
extern UINT16 const str_osd_input_channel_audio[4][8];
extern UINT16 const str_osd_output_channel[4][5];
extern char const str_osd_res[3][15];

extern HD_RX_DEV_T g_st_hd_rx[4];
extern HD_VIDEO_TIMING_T	g_stVideo_HDRx_Timing[4];
extern MISC_POLARITY_T	g_st_rx_polarity[4];
extern CSC_DEV_T g_st_vipss_csc[4];
extern BOOL b_RxVideoChg[4];
extern UINT8 u8_wdma_buf_id;
extern UINT8 u8_rdma_buf_id;

extern HD_TX_DEV_T g_st_hd_tx[4];
extern HD_VIDEO_TIMING_T	g_stVideo_HDTx_Timing;
extern MISC_TX_MUX_T g_st_tx_mux[4];
extern CSC_DEV_T g_st_txcsc_dev[4];
extern VDS_TMG_DEV_T g_st_timgen[4];
extern UINT8 hd_tx_edid[256];
extern HD_EDID_FLAG_T g_hd_edid_flag[4];
extern BOOL b_hpd_bak[4];

extern UINT8 g_u8_display_mode;
extern UINT8 g_u8_display_mode_bak;
extern UINT8 g_u8_output_index;
extern UINT8 Matrix_Tx_Mapping[4];
extern UINT8 Vipss_Mapping_Rx[4];
extern UINT8 Bypass_Tx_Status[4];
extern UINT8 u8_Tx_audio_sel[4];

extern DEINT_DEV_T g_st_deint_dev;

extern OSD_DEV_T g_st_osd_dev[4];
extern MISC_OSD_MUX_T g_st_misc_osd_mux[4];
extern OSD_WINSIZE_T st_osd_win_size;
extern OSD_STRING_T st_str;
extern OSD_FONT_T st_font;
extern char p_str[15];
extern BOOL g_bOSDShown;
extern UINT8 g_u8OSDCount;

extern HD_INFOFRAMES_PACKETS_T gst_inf_pack;

extern VIDEO_MIXER_DEV_T g_st_video_mixer_dev[4];
extern VIDEO_MIXER_LAYER_SEL_T g_st_mixer_layer_sel;
extern VIDEO_MIXER_ALF_CONFIG_T g_st_mixer_alf_cfg;
extern VIDEO_MIXER_COLOR_T g_st_bg_color;

extern VEHC_DEV_T g_st_vivehc[4];
extern VEHC_DEV_T g_st_vovehc[4];

extern BOOL b_audio_mute;

UINT16 return_minof_three(UINT16 u16dat1, UINT16 u16dat2, UINT16 u16dat3);
UINT16 return_com_multiple(UINT16 u16dat, UINT8 u8_data);
VOID app_video_vipss_trig(UINT8 u8_vipss_sel);
VOID app_video_vopss_trig(UINT8 u8_vopss_sel);
VOID app_tx_audio_cfg(UINT8 u8_audio_src, UINT8 u8_tx);
VOID app_digital_audio_cfg(UINT8 u8_audio_src, UINT8 u8_digital_audio_id, BOOL b_status);
VOID app_bcsh_adjust(UINT8 u8_vehc_idx);
BOOL video_ms1826_reset(UINT8 u8_dis_cur, UINT8 u8_dis_bak);
VOID app_digital_input_hd_output_cfg(UINT8 g_u8_digital_idx,UINT8 u8_tx);
VOID app_hd_input_hd_output_cfg(UINT8 u8_audio_src, UINT8 u8_tx);

typedef struct _T_MS_VIDEO_SIZE_
{
	UINT16 u16_h;
	UINT16 u16_v;
} VIDEOSIZE_T;

typedef struct _T_SCALE_SIZE_
{
	VIDEOSIZE_T tInSize;
	VIDEOSIZE_T tOutSize;
	VIDEOSIZE_T tMemSize;
} SCALE_SIZE_T;

typedef struct _T_WIN_BORDER_
{
    INT16 top;
    INT16 bottom;
    INT16 left;
    INT16 right;
} WINBORDER_T;

/* MS1826 设备的工作模式 _by_zzw*/
typedef enum E_MS1826_WORKING_MODE_
{
	MS1826_VIDEO_SPLITTER               = 0, /* 视频分割模式 _by_zzw*/
	MS1826_VIDEO_SPLICER                = 1, /* 视频拼接模式 _by_zzw*/
	MS1826_SEAMLESS_MATRIX_SWITCHER     = 2, /* 无缝切换矩阵模式 _by_zzw*/
	MS1826_BYPASS_MATRIX_SWITCHER       = 3  /* 矩阵模式 _by_zzw*/
}MS1826_WORKING_MODE;

#endif

