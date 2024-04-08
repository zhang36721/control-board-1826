#ifndef _APP_SINGLE_MODE_H_
#define _APP_SINGLE_MODE_H_

#include "app.h"

#define  SINGLE_BUF_NUM                TRI_BUF

typedef struct _SINGLE_VIPSS_T
{ 
	BOOL 	            b_is_dsc;
    UINT8               u8_vipss_sel;
    MISC_VIDEO_MUX_T    st_vipss_mux_dev;
	SDN4K_DEV_T 		st_sdn4k_dev;
    SDN_DEV_T           st_single_sdn_dev;
    VWDMA_DEV_T         st_single_vwdma_dev;
	DSC_DEV_T           st_encoder_dev;
    DMA_COMMON_DEV_T    st_single_com_wdma_dev;
    SS_DEV_T            st_single_ss_vipss_dev;
}SINGLE_VIPSS_T;

typedef struct _SINGLE_VOPSS_T
{
	BOOL 				b_is_dsc;
    UINT8               u8_vopss_sel;
    DMA_COMMON_DEV_T    st_single_com_rdma_dev;
    VRDMA_DEV_T         st_single_vrdma_dev;
	DSC_DEV_T           st_decoder_dev;
    SU_DEV_T            st_single_su_dev;
    SS_DEV_T            st_single_ss_vopss_dev;
}SINGLE_VOPSS_T;

extern SINGLE_VIPSS_T gst_single_vipss_dev[4];
extern SINGLE_VOPSS_T gst_single_vopss_dev[4];

VOID app_single_output_resolution(UINT8 u8_vic);
VOID app_single_mode_config(VOID);
VOID app_single_process(UINT8 u8_rx_idx);
VOID single_video_mute(UINT8 u8_path, BOOL b_mute);
VOID app_single_mode_rx_switch(UINT8 u8_rx, UINT8 u8_tx);
VOID app_single_osd_init(VOID);
VOID app_single_osd_config(UINT8 u8_index);
#endif
