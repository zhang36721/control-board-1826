#ifndef _APP_C
#define _APP_C

#include "app.h"
#include "videoprocess.h"

UINT8 const g_u8_rx_idx[4]				= {HD_RX_A, HD_RX_B, HD_RX_C, HD_RX_D};
UINT8 const g_u8_csc_idx[4]				= {CSC_SEL_1, CSC_SEL_2, CSC_SEL_3, CSC_SEL_4};
UINT8 const g_u8_misc_module_rx[4]		= {MISC_VIDEO_MODULE_RX1, MISC_VIDEO_MODULE_RX2, MISC_VIDEO_MODULE_RX3, MISC_VIDEO_MODULE_RX4};
UINT8 const g_u8_4ksdn_idx[4]			= {SDN4K_1, SDN4K_2, SDN4K_3, SDN4K_4};
UINT8 const g_u8_sdn_idx[4]				= {SDN_SEL_SDN1, SDN_SEL_SDN2, SDN_SEL_SDN3, SDN_SEL_SDN4};
UINT8 const g_u8_dsc_idx[4]				= {DSC_SEL_1, DSC_SEL_2, DSC_SEL_3,	DSC_SEL_4};
UINT8 const g_u8_dma_idx[4]				= {DMA_SEL_PATH1, DMA_SEL_PATH2, DMA_SEL_PATH3,	DMA_SEL_PATH4};
UINT8 const g_u8_dma_dsc_idx[4]			= {DMA_SEL_PATH1_DSC, DMA_SEL_PATH2_DSC, DMA_SEL_PATH3_DSC,	DMA_SEL_PATH4_DSC};
UINT8 const g_u8_misc_module_vipss[4]	= {MISC_VIDEO_MODULE_VIPSS1, MISC_VIDEO_MODULE_VIPSS2, MISC_VIDEO_MODULE_VIPSS3, MISC_VIDEO_MODULE_VIPSS4};
UINT8 const g_u8_misc_module_vopss[4]	= {MISC_VIDEO_MODULE_VOPSS1, MISC_VIDEO_MODULE_VOPSS2, MISC_VIDEO_MODULE_VOPSS3, MISC_VIDEO_MODULE_VOPSS4};
UINT8 const g_u8_su_idx[4]				= {SU_SEL_SU2, SU_SEL_SU4, SU_SEL_SU6, SU_SEL_SU8};
UINT8 const g_u8_su_dsc_idx[4]			= {SU_SEL_SU1, SU_SEL_SU3, SU_SEL_SU5, SU_SEL_SU7};
UINT8 const g_u8_mixer_idx[4]			= {VIDEO_MIXER_1, VIDEO_MIXER_2, VIDEO_MIXER_3,VIDEO_MIXER_4};
UINT8 const g_u8_mixer_layer_src[4]		= {VOPSS1_SU, VOPSS2_SU, VOPSS3_SU,	VOPSS4_SU};
UINT8 const g_u8_mixer_layer_dsc_src[4]	= {VOPSS1_DSC_SU, VOPSS2_DSC_SU, VOPSS3_DSC_SU, VOPSS4_DSC_SU};
UINT8 const g_u8_misc_mixer[4]			= {MISC_VIDEO_MODULE_MIX1, MISC_VIDEO_MODULE_MIX2, MISC_VIDEO_MODULE_MIX3, MISC_VIDEO_MODULE_MIX4};
UINT8 const g_u8_misc_timgen[4]			= {MISC_VIDEO_MODULE_TIMGEN1, MISC_VIDEO_MODULE_TIMGEN2, MISC_VIDEO_MODULE_TIMGEN3, MISC_VIDEO_MODULE_TIMGEN4};
UINT8 const g_u8_misc_su[4]				= {MISC_VIDEO_MODULE_VOPASS1SU1, MISC_VIDEO_MODULE_VOPASS2SU1, MISC_VIDEO_MODULE_VOPASS3SU1, MISC_VIDEO_MODULE_VOPASS4SU1};
UINT8 const g_u8_misc_dsc_su[4]			= {MISC_VIDEO_MODULE_VOPASS1SU0, MISC_VIDEO_MODULE_VOPASS2SU0, MISC_VIDEO_MODULE_VOPASS3SU0, MISC_VIDEO_MODULE_VOPASS4SU0};
UINT8 const g_u8_misc_module_tx[4]		= {MISC_VIDEO_MODULE_TX1, MISC_VIDEO_MODULE_TX2, MISC_VIDEO_MODULE_TX3, MISC_VIDEO_MODULE_TX4};
UINT8 const g_u8_split_idx[4]			= {SPLIT_1,	SPLIT_2, SPLIT_3, SPLIT_4};
UINT8 const g_u8_ss_ch_idx[4]			= {CHANNEL1_without_dsc, CHANNEL2_without_dsc, CHANNEL3_without_dsc, CHANNEL4_without_dsc};
UINT8 const g_u8_ss_ch_dsc_idx[4]		= {CHANNEL1_with_dsc, CHANNEL2_with_dsc, CHANNEL3_with_dsc,	CHANNEL4_with_dsc};
UINT8 const g_u8_vb_path[4]				= {MISC_VBPATH_VOPASS1SU1, MISC_VBPATH_VOPASS2SU1, MISC_VBPATH_VOPASS3SU1, MISC_VBPATH_VOPASS4SU1};
UINT8 const g_u8_vb_dsc_path[4]			= {MISC_VBPATH_VOPASS1SU0, MISC_VBPATH_VOPASS2SU0, MISC_VBPATH_VOPASS3SU0, MISC_VBPATH_VOPASS4SU0};
UINT8 const g_u8_vb_rx[4]				= {MISC_VBPATH_RX1,	MISC_VBPATH_RX2, MISC_VBPATH_RX3, MISC_VBPATH_RX4};
UINT8 const g_u8_vb_vdstmg[4]			= {MISC_VBPATH_VDSTMG1,	MISC_VBPATH_VDSTMG2, MISC_VBPATH_VDSTMG3, MISC_VBPATH_VDSTMG4};
UINT8 const g_u8_intsrc_rx[4]			= {MISC_INTSRC_RX1VDE, MISC_INTSRC_RX2VDE, MISC_INTSRC_RX3VDE, MISC_INTSRC_RX4VDE};
UINT8 const g_u8_intsrc_rxvb[4]			= {MISC_INTSRC_RX1VB, MISC_INTSRC_RX2VB, MISC_INTSRC_RX3VB,	MISC_INTSRC_RX4VB};
UINT8 const g_u8_misc_clksrc[4]			= {MISC_CLKSRC_VDSTMG1,	MISC_CLKSRC_VDSTMG2, MISC_CLKSRC_VDSTMG3, MISC_CLKSRC_VDSTMG4};
UINT8 const g_u8_osd_idx[4]				= {OSD_1, OSD_2, OSD_3, OSD_4};
UINT8 const g_u8_misc_osd_idx[4]		= {MISC_VIDEO_MODULE_OSD1, MISC_VIDEO_MODULE_OSD2, MISC_VIDEO_MODULE_OSD3, MISC_VIDEO_MODULE_OSD4};
UINT8 const g_u8_i2s_idx[4] 			= {DIG_AUDIO_I2S1, DIG_AUDIO_I2S2, DIG_AUDIO_I2S3, DIG_AUDIO_I2S4};
UINT8 const g_u8_spdif_idx[4] 			= {DIG_AUDIO_SPDIF1, DIG_AUDIO_SPDIF2, DIG_AUDIO_SPDIF3, DIG_AUDIO_SPDIF4};
UINT8 const g_u8_i2s_audio_idx[4] 		= {I2S1_AUDIO, I2S2_AUDIO, I2S3_AUDIO, I2S4_AUDIO};
UINT8 const g_u8_spdif_audio_idx[4] 	= {SPIDF1_AUDIO, SPIDF2_AUDIO, SPIDF3_AUDIO, SPIDF4_AUDIO};



UINT8 const g_arrOutputTable[3]	=
{
	VFMT_CEA_04_1280x720P_60HZ,
	VFMT_CEA_16_1920x1080P_60HZ,
	VFMT_VESA_234_3840X2160_30
};

UINT8 const g_u8_HD_RX_PROT[4] =
{
	HD_RX_A,
	HD_RX_D,
	HD_RX_C,
	HD_RX_B
};

UINT8 const g_u8_HD_TX_PROT[4] =
{
	HD_TX_A,
	HD_TX_B,
	HD_TX_C,
	HD_TX_D
};

UINT16 const str_osd_input_channel[4][8] =
{
	96, 97, 99, 100, 17, 0, 0, 0,
	96, 97, 99, 100, 18, 0, 0, 0,
	96, 97, 99, 100, 19, 0, 0, 0,
	96, 97, 99, 100, 20, 0, 0, 0,
};

UINT16 const str_osd_input_channel_audio[4][8] =
{
	96, 97, 99, 100, 17, 0, 0, 95,
	96, 97, 99, 100, 18, 0, 0, 95,
	96, 97, 99, 100, 19, 0, 0, 95,
	96, 97, 99, 100, 20, 0, 0, 95,
};

UINT16 const str_osd_output_channel[4][5] =
{
	96, 98, 99, 100, 17,
	96, 98, 99, 100, 18,
	96, 98, 99, 100, 19,
	96, 98, 99, 100, 20,
};

char const str_osd_res[3][15] =
{
	"1280x720@60Hz ",
	"1920x1080P@60Hz",
	"3840x2160P@30Hz",
};

HD_RX_DEV_T g_st_hd_rx[4];
HD_VIDEO_TIMING_T	g_stVideo_HDRx_Timing[4];
MISC_POLARITY_T	g_st_rx_polarity[4];
CSC_DEV_T g_st_vipss_csc[4];
BOOL b_RxVideoChg[4] = {MS_FALSE};
UINT8 u8_wdma_buf_id = 0;
UINT8 u8_rdma_buf_id = 0;

HD_TX_DEV_T g_st_hd_tx[4];
HD_VIDEO_TIMING_T	g_stVideo_HDTx_Timing;
MISC_TX_MUX_T g_st_tx_mux[4];
CSC_DEV_T g_st_txcsc_dev[4];
VDS_TMG_DEV_T g_st_timgen[4];
UINT8 hd_tx_edid[256]	= {0};
HD_EDID_FLAG_T g_hd_edid_flag[4];
BOOL b_hpd_bak[4] =	{0};

DEINT_DEV_T g_st_deint_dev;

OSD_DEV_T g_st_osd_dev[4];
MISC_OSD_MUX_T g_st_misc_osd_mux[4];
OSD_WINSIZE_T st_osd_win_size;
OSD_STRING_T st_str;
OSD_FONT_T st_font;
char p_str[15] = {'0'};
BOOL g_bOSDShown = MS_FALSE;/* 是否显示 _by_zzw*/
UINT8 g_u8OSDCount;

UINT8 g_u8_display_mode	= MS1826_SEAMLESS_MATRIX_SWITCHER;
UINT8 g_u8_display_mode_bak = MS1826_SEAMLESS_MATRIX_SWITCHER;
UINT8 g_u8_output_index	= 1;/* 前输出的索引 _by_zzw*/
UINT8 Matrix_Tx_Mapping[4] = {0, 1, 2, 3};/* 将输入信号映射到矩阵切换器的输出端口 _by_zzw*/
UINT8 Vipss_Mapping_Rx[4] =	{0,	1, 2, 3};/* 视频信号处理器（VIPSS）的输出映射到输入端口 _by_zzw*/
UINT8 Bypass_Tx_Status[4] =	{0,	0, 0, 0};/* 可能是存储旁路传输器（Bypass Tx）的状态 _by_zzw*/
UINT8 u8_Tx_audio_sel[4] = {0, 0 , 0, 0};/* 选择每个传输器的音频输入源 _by_zzw*/

HD_INFOFRAMES_PACKETS_T	gst_inf_pack;

VIDEO_MIXER_DEV_T g_st_video_mixer_dev[4];
VIDEO_MIXER_LAYER_SEL_T	g_st_mixer_layer_sel;
VIDEO_MIXER_ALF_CONFIG_T g_st_mixer_alf_cfg;
VIDEO_MIXER_COLOR_T g_st_bg_color = {0x10, 0x80, 0x80};

VEHC_DEV_T g_st_vivehc[4];
VEHC_DEV_T g_st_vovehc[4];

BOOL b_audio_mute = FALSE;
extern UINT8 g_u8_bcsh_bri;	
extern UINT8 g_u8_bcsh_con;


UINT16 return_minof_three(UINT16 u16dat1, UINT16 u16dat2, UINT16 u16dat3)
{
	UINT16 u16min = (u16dat1 >= u16dat2) ? u16dat2 : u16dat1;

	u16min = (u16min >= u16dat3) ? u16dat3 : u16min;

	return u16min;
}

UINT16 return_com_multiple(UINT16 u16dat, UINT8 u8_data)
{
	UINT16 u16_con_mult;
	if(u16dat % (8 * u8_data) != 0)
	{
		u16_con_mult = u16dat / (8 * u8_data) * (8 * u8_data);
		return u16_con_mult;
	}
	else
		return u16dat;
}

VOID app_video_vipss_trig(UINT8	u8_vipss_sel)
{
	SS_DEV_T st_split_vipss_ss_dev;
	
	st_split_vipss_ss_dev.u8_sel = u8_vipss_sel;
	st_split_vipss_ss_dev.u8_vipss_trigger_mode	= FORCE_TRIGGER;
	api_ss_vipss_trig(&st_split_vipss_ss_dev);
}

VOID app_video_vopss_trig(UINT8	u8_vopss_sel)
{
	SS_DEV_T st_split_vopss_ss_dev;
	st_split_vopss_ss_dev.u8_sel = u8_vopss_sel;
	st_split_vopss_ss_dev.st_vopss_trigger_mode.u8_trigger_sel = TRIGGER_BY_VOPSS_TRIGGER;
	st_split_vopss_ss_dev.st_vopss_trigger_mode.u8_trigger_mode	= FORCE_TRIGGER;
	api_ss_vopss_trig_mode_cfg(&st_split_vopss_ss_dev);
	api_ss_vopss_trig(&st_split_vopss_ss_dev);
}

VOID app_digital_audio_cfg(UINT8 u8_audio_src, UINT8 u8_digital_audio_id, BOOL b_status)
{
	MISC_DIGITAL_AUDIO_CFG g_st_misc_digital_audio_cfg;
	MISC_MCLK_CFG_T g_st_misc_mclk_cfg;
	g_st_misc_digital_audio_cfg.u8_audio_src = g_u8_HD_RX_PROT[u8_audio_src];
	g_st_misc_digital_audio_cfg.u8_digital_audio_dst = u8_digital_audio_id;
	
	
	if(u8_digital_audio_id == g_u8_i2s_idx[0])
		I2S1_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_i2s_idx[1])
		I2S2_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_i2s_idx[2])
		I2S3_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_i2s_idx[3])
		I2S4_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_spdif_idx[0])
		SPDIF1_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_spdif_idx[1])
		SPDIF2_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_spdif_idx[2])
		SPDIF3_SW_OUTPUT;
	else if(u8_digital_audio_id == g_u8_spdif_idx[3])
		SPDIF4_SW_OUTPUT;
	
	g_st_misc_digital_audio_cfg.b_enable = MS_FALSE;
	api_misc_digital_audio_out_cfg(&g_st_misc_digital_audio_cfg);
	
	g_st_misc_mclk_cfg.b_mclk_from_crystal = MS_FALSE;
	g_st_misc_mclk_cfg.u8_mclk_div = MISC_CLKDIV_1;
	g_st_misc_mclk_cfg.u8_digital_audio_dst = u8_digital_audio_id;
	g_st_misc_mclk_cfg.u8_mclk_en = b_status;
	g_st_misc_mclk_cfg.u8_audio_src = g_u8_HD_RX_PROT[u8_audio_src];
	api_misc_digital_mclk_cfg(&g_st_misc_mclk_cfg);
	
	g_st_misc_digital_audio_cfg.b_enable = b_status;
	api_misc_digital_audio_out_cfg(&g_st_misc_digital_audio_cfg);
}

VOID app_tx_audio_cfg(UINT8	u8_audio_src, UINT8 u8_tx)
{
	MISC_TX_AUDIO_SEL st_misc_tx_audio_sel;
	u8_Tx_audio_sel[u8_tx] = u8_audio_src;
	st_misc_tx_audio_sel.U8_tx_dst = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_tx]];
	st_misc_tx_audio_sel.u8_audio_src = g_u8_HD_RX_PROT[u8_audio_src];
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], MS_TRUE);
	api_misc_tx_audio_in_cfg(&st_misc_tx_audio_sel); 
	memcpy((&g_st_hd_tx[u8_tx].st_infoframes_packets.st_audio_infoframe), &g_st_hd_rx[u8_audio_src].st_packets_infoframes.st_audio_infoframe, sizeof(HD_AUDIO_INFOFRAME_T));
	api_hd_tx_audio_change(&g_st_hd_tx[u8_tx]);
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], b_audio_mute);
	app_digital_audio_cfg(u8_audio_src, g_u8_i2s_idx[u8_tx], !b_audio_mute);
	app_digital_audio_cfg(u8_audio_src, g_u8_spdif_idx[u8_tx], !b_audio_mute);
}

VOID app_hd_input_hd_output_cfg(UINT8 u8_audio_src, UINT8 u8_tx)
{
	MISC_TX_AUDIO_SEL st_misc_tx_audio_sel;
	u8_Tx_audio_sel[u8_tx] = u8_audio_src;
	st_misc_tx_audio_sel.U8_tx_dst = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_tx]];
	st_misc_tx_audio_sel.u8_audio_src = g_u8_HD_RX_PROT[u8_audio_src];
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], MS_TRUE);
	api_misc_tx_audio_in_cfg(&st_misc_tx_audio_sel); 
	memcpy((&g_st_hd_tx[u8_tx].st_infoframes_packets.st_audio_infoframe), &g_st_hd_rx[u8_audio_src].st_packets_infoframes.st_audio_infoframe, sizeof(HD_AUDIO_INFOFRAME_T));
	api_hd_tx_audio_change(&g_st_hd_tx[u8_tx]);
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], b_audio_mute);

}

VOID app_digital_input_hd_output_cfg(UINT8 g_u8_digital_idx,UINT8 u8_tx)
{
	MISC_TX_AUDIO_SEL st_misc_tx_audio_sel;
	MISC_DIGITAL_AUDIO_CFG g_st_misc_digital_audio_cfg;
	MISC_MCLK_CFG_T g_st_misc_mclk_cfg;
	UINT8 u8_digital_audio_dst;
	
	if(g_u8_digital_idx == I2S1_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_I2S_PAD;
		I2S1_SW_INPUT;
	}
	else if(g_u8_digital_idx == SPIDF1_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_SPDIF_PAD;
		SPDIF1_SW_INPUT;
	}
	else if(g_u8_digital_idx == I2S2_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_I2S_PAD;
		I2S2_SW_INPUT;
	}
	else if(g_u8_digital_idx == SPIDF2_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_SPDIF_PAD;
		SPDIF2_SW_INPUT;
	}
	else if(g_u8_digital_idx == I2S3_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_I2S_PAD;
		I2S3_SW_INPUT;
	}
	else if(g_u8_digital_idx == SPIDF3_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_SPDIF_PAD;
		SPDIF3_SW_INPUT;
	}
	else if(g_u8_digital_idx == I2S4_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_I2S_PAD;
		I2S4_SW_INPUT;
	}
	else if(g_u8_digital_idx == SPIDF4_AUDIO)
	{
		g_st_hd_tx[u8_tx].u8_audio_src = FROM_SPDIF_PAD;
		SPDIF4_SW_INPUT;
	}

	if(g_u8_digital_idx == I2S1_AUDIO)		
		u8_digital_audio_dst = DIG_AUDIO_I2S1;
	else if(g_u8_digital_idx == I2S2_AUDIO)
		u8_digital_audio_dst = DIG_AUDIO_I2S2;
	else if(g_u8_digital_idx == I2S3_AUDIO)
		u8_digital_audio_dst = DIG_AUDIO_I2S3;
	else if(g_u8_digital_idx == I2S4_AUDIO)
		u8_digital_audio_dst = DIG_AUDIO_I2S4;
		
	g_st_misc_digital_audio_cfg.u8_audio_src = RX1_AUDIO;
	g_st_misc_digital_audio_cfg.u8_digital_audio_dst = u8_digital_audio_dst;
	g_st_misc_digital_audio_cfg.b_enable = MS_FALSE;
	api_misc_digital_audio_out_cfg(&g_st_misc_digital_audio_cfg);

	g_st_hd_tx[u8_tx].u8_txIndex = g_u8_HD_TX_PROT[u8_tx];
	
	
	api_hd_tx_audio_change(&g_st_hd_tx[u8_tx]);
	
	st_misc_tx_audio_sel.U8_tx_dst = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_tx]];
	st_misc_tx_audio_sel.u8_audio_src = g_u8_digital_idx;
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], MS_TRUE);
	api_misc_tx_audio_in_cfg(&st_misc_tx_audio_sel); 
	api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx], MS_FALSE);
	
	g_st_misc_mclk_cfg.u8_mclk_div = MISC_CLKDIV_1;
	g_st_misc_mclk_cfg.u8_digital_audio_dst = u8_digital_audio_dst;
	g_st_misc_mclk_cfg.u8_mclk_en = MS_TRUE;
	g_st_misc_mclk_cfg.u8_audio_src = g_u8_HD_RX_PROT[0];
	api_misc_digital_mclk_cfg(&g_st_misc_mclk_cfg);
}

VOID app_bcsh_adjust(UINT8 u8_vehc_idx)
{
	if(u8_vehc_idx < VEHC_VIPSS_1)
	{
		api_vehc_instance_init(&g_st_vovehc[0], u8_vehc_idx);
		if(CHIP_INTERNAL_PROCESS_COLORSPACE == HD_COLORSPACE_RGB)
		{
			g_st_vovehc[0].u8_vehc_idx = u8_vehc_idx;
			g_st_vovehc[0].b_yuv = MS_FALSE;
			g_st_vovehc[0].st_bcsh.u8_bcsh_en_flag = BRI_EN | CON_EN;
						
			g_st_vovehc[0].st_bcsh.s8_bcsh_bri = g_u8_bcsh_bri*2 - 100;
			g_st_vovehc[0].st_bcsh.u8_bcsh_con = g_u8_bcsh_con*2 + 28;
		}
		else
		{
			g_st_vovehc[0].b_yuv = MS_TRUE;
			g_st_vovehc[0].st_bcsh.u8_bcsh_en_flag = BRI_EN | CON_EN | SAT_EN | HUE_EN;
		}
		api_vehc_bcsh_config(&g_st_vovehc[0]);
		
	}

	else if(u8_vehc_idx < VEHC_NUM)
	{
		u8_vehc_idx = u8_vehc_idx % 4;
		api_vehc_instance_init(&g_st_vivehc[0], u8_vehc_idx);
		if(CHIP_INTERNAL_PROCESS_COLORSPACE == HD_COLORSPACE_RGB)
		{
			g_st_vivehc[0].b_yuv = MS_FALSE;
			g_st_vivehc[0].st_bcsh.u8_bcsh_en_flag = BRI_EN | CON_EN;
		}
		else
		{
			g_st_vivehc[0].b_yuv = MS_TRUE;
			g_st_vivehc[0].st_bcsh.u8_bcsh_en_flag = BRI_EN | CON_EN | SAT_EN | HUE_EN;
		}
		api_vehc_bcsh_config(&g_st_vivehc[0]);
	}
	else
		return;
}

BOOL video_ms1826_reset(UINT8 u8_dis_cur, UINT8 u8_dis_bak)
{		 
    switch(u8_dis_cur)
    {
        case MS1826_VIDEO_SPLITTER:
            if(u8_dis_bak == MS1826_SEAMLESS_MATRIX_SWITCHER)
            {
                mculib_chip_reset();
                video_process_cfg();
                return MS_TRUE;
            }       
			break;			
        case MS1826_VIDEO_SPLICER:
            if(u8_dis_bak == MS1826_SEAMLESS_MATRIX_SWITCHER)
            {
                mculib_chip_reset();
                video_process_cfg();
                return MS_TRUE;
            }
			break;
        case MS1826_SEAMLESS_MATRIX_SWITCHER:
            mculib_chip_reset();
            video_process_cfg();
            return MS_TRUE;
        case MS1826_BYPASS_MATRIX_SWITCHER:
            if(u8_dis_bak == MS1826_SEAMLESS_MATRIX_SWITCHER)
            {
                mculib_chip_reset();
                video_process_cfg();
                return MS_TRUE;
            }
			break;

    }
    return MS_FALSE;

}

#endif
