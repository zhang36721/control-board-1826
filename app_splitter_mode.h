#ifndef _APP_SPLITTER_MODE_H_
#define _APP_SPLITTER_MODE_H_

#include "app.h"

#define  SPLITTER_BUF_NUM                     	TRI_BUF

typedef enum _SPLITTER_MODE_E
{
	MODE_SIGNLE_1,
	MODE_SIGNLE_2,
	MODE_SIGNLE_3,
	MODE_SIGNLE_4,
	MODE_DUAL_UP1_DOWN2,
	MODE_DUAL_UP1_DOWN2_4_3,
	MODE_DUAL_UP3_DOWN4,
	MODE_DUAL_UP3_DOWN4_4_3,
	MODE_DUAL_LEFT1_RIGHT2,
	MODE_DUAL_LEFT1_RIGHT2_4_3,
	MODE_DUAL_LEFT3_RIGHT4,
	MODE_DUAL_LEFT3_RIGHT4_4_3,
	MODE_PIP_12_1_3,
	MODE_PIP_21_1_3,
	MODE_PIP_34_1_3,
	MODE_PIP_43_1_3,
	MODE_THREE_EQUALIZE,
	MODE_FOUR_MAIN1_SUB234,
	MODE_FOUR_MAIN2_SUB134,
	MODE_FOUR_MAIN3_SUB124,
	MODE_FOUR_MAIN4_SUB123,
	MODE_FOUR_H_SUB1234,
	MODE_FOUR_EQUALIZE
}SPLITTER_MODE_E;

typedef enum _SPLITTER_AUDIO_SRC_E
{
	AUDIO_HD_RX1,
	AUDIO_HD_RX2,
	AUDIO_HD_RX3,
	AUDIO_HD_RX4,
}SPLITTER_AUDIO_SRC_E;

typedef struct _SPLITTER_VIPSS_T
{ 
	UINT8               u8_vipss_sel;
	MISC_VIDEO_MUX_T    st_vipss_mux_dev;
	SDN4K_DEV_T 		st_sdn4k_dev;
	SDN_DEV_T           st_splitter_sdn_dev;
	VWDMA_DEV_T         st_splitter_vwdma_dev;
	DMA_COMMON_DEV_T    st_splitter_com_wdma_dev;
	SS_DEV_T            st_splitter_ss_vipss_dev;
}SPLITTER_VIPSS_T;

typedef struct _SPLITTER_VOPSS_T
{
	UINT8               u8_vopss_sel;
	DMA_COMMON_DEV_T    st_splitter_com_rdma_dev;
	VRDMA_DEV_T         st_splitter_vrdma_dev;
	SU_DEV_T            st_splitter_su_dev;
	SS_DEV_T            st_splitter_ss_vopss_dev;
}SPLITTER_VOPSS_T;

typedef struct _SPLITTER_APP_T
{
	UINT8               u8_splitter_mode;
	UINT8               u8_dst_sel;
	BOOL                b_splitter_sw;
	VIDEO_WIN_T         st_video_win[4];
	SCALE_SIZE_T        st_scale_size[4];
	DMA_START_ADDR_T    st_dma_addr[4];

}SPLITTER_APP_T;

extern SPLITTER_APP_T g_st_app_splitter_dev;
extern SPLITTER_VIPSS_T gst_splitter_vipss_dev[4];
extern SPLITTER_VOPSS_T gst_splitter_vopss_dev[4];

VOID app_splitter_output_resolution(UINT8 u8_vic, SPLITTER_APP_T *pst_app_splitter_dev);
VOID app_splitter_mode_config(SPLITTER_APP_T *pst_app_splitter_dev);
VOID app_splitter_process(UINT8 u8_rx_idx);
VOID app_splitter_mode_video_freeze(SPLITTER_APP_T *pst_app_splitter_dev);
VOID app_splitter_mode_audio_src(VOID);
VOID splitter_video_mute(UINT8 u8_path, BOOL b_mute);
VOID app_splitter_rx_rotate(UINT8 u8_rx_idx, UINT8 u8_rotate);
VOID app_splitter_osd_init(VOID);
VOID app_splitter_osd_config(VOID);
#endif
