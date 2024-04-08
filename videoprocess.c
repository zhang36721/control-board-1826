#include "mculib_common.h"
#include "videoprocess.h"

extern BOOL b_osd_menu;
extern UINT8 g_u8_time;
extern MISCTIMING_T __CODE g_arrTimingTable[];
extern BOOL g_dsc_enable;
extern VOID ms1826_HAL_SetBits_Ex(UINT32 u32_index, UINT32 u32_mask);

static UINT8 const g_u8c_sys_edid_default_buf[256]	=
{

	// Explore Semiconductor,	Inc. EDID	Editor V2
	0x00, 0xFF,	0xFF, 0xFF,	0xFF, 0xFF,	0xFF, 0x00,		// address 0x00
	0x21, 0x57,	0x36, 0x18,	0xBD, 0xE9,	0x02, 0x00,		//
	0x25, 0x1D,	0x01, 0x03,	0x80, 0x35,	0x1D, 0x78,		// address 0x10
	0x62, 0xEE,	0x91, 0xA3,	0x54, 0x4C,	0x99, 0x26,		//
	0x0F, 0x50,	0x54, 0x21,	0x0F, 0x00,	0x81, 0x00,		// address 0x20
	0x81, 0x40,	0x81, 0x80,	0x90, 0x40,	0x95, 0x00,		//
	0x01, 0x01,	0xA9, 0x40,	0xB3, 0x00,	0x02, 0x3A,		// address 0x30
	0x80, 0x18,	0x71, 0x38,	0x2D, 0x40,	0x58, 0x2C,		//
	0x55, 0x00,	0xE0, 0x0E,	0x11, 0x00,	0x00, 0x5F,		// address 0x40
	0x0E, 0x1F,	0x00, 0x80,	0x51, 0x00,	0x1E, 0x30,		//
	0x40, 0x80,	0x37, 0x00,	0x0F, 0x48,	0x42, 0x00,		// address 0x50
	0x00, 0x1C,	0x00, 0x00,	0x00, 0xFD,	0x00, 0x17,		//
	0x55, 0x1E,	0x64, 0x1E,	0x00, 0x0A,	0x20, 0x20,		// address 0x60
	0x20, 0x20,	0x20, 0x20,	0x00, 0x00,	0x00, 0xFC,		//
	0x00, 0x4D,	0x41, 0x43,	0x52, 0x4F,	0x53, 0x49,		// address 0x70
	0x4C, 0x49,	0x43, 0x4F,	0x4E, 0x0A,	0x01, 0x86,		//
	0x02, 0x03,	0x2E, 0xF1,	0x52, 0x02,	0x11, 0x13,		// address 0x80
	0x84, 0x1F,	0x10, 0x03,	0x12, 0x06,	0x15, 0x07,		//
	0x16, 0x05,	0x14, 0x5E,	0x5F, 0x63,	0x64, 0x23,		// address 0x90
	0x09, 0x7F,	0x07, 0x83,	0x7F, 0x00,	0x00, 0x6E,		//
	0x03, 0x0C,	0x00, 0x10,	0x00, 0x78,	0x3C, 0x20,		// address 0xA0
	0x00, 0x80,	0x01, 0x02,	0x03, 0x04,	0x66, 0x21,		//
	0x50, 0xB0,	0x51, 0x00,	0x1B, 0x30,	0x40, 0x70,		// address 0xB0
	0x36, 0x00,	0x0F, 0x48,	0x42, 0x00,	0x00, 0x1E,		//
	0x66, 0x21,	0x56, 0xAA,	0x51, 0x00,	0x1E, 0x30,		// address 0xC0
	0x46, 0x8F,	0x33, 0x00,	0x0F, 0x48,	0x42, 0x00,		//
	0x00, 0x1E,	0x8C, 0x0A,	0xD0, 0x8A,	0x20, 0xE0,		// address 0xD0
	0x2D, 0x10,	0x10, 0x3E,	0x96, 0x00,	0x10, 0x09,		//
	0x00, 0x00,	0x00, 0x18,	0x00, 0x00,	0x00, 0x00,		// address 0xE0
	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,		//
	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,		// address 0xF0
	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,	0x00, 0x41,		//

};

static VOID	_ms1826_init(VOID)
{
	UINT8 u8_ddr_status;
	//DDR INIT.
	UINT8 u8_chip_addr = 0;
	u8_chip_addr = api_misc_get_i2c_addr();
	LOG1("[DEBUG] MS1826 Chip Address: ", u8_chip_addr);
	api_misc_set_i2c_addr(u8_chip_addr);
	DMC_T st_dmc_cfg;
	PLL_DEV_T st_pll_dev;	  
	if(g_u8_display_mode == MS1826_SEAMLESS_MATRIX_SWITCHER)
        g_dsc_enable = MS_TRUE;
    else
	    g_dsc_enable = MS_FALSE;
	if(g_dsc_enable)
	{
		st_pll_dev.u32_clk_out = 26650 * 10000;	
		st_pll_dev.u8_pll_idx =	PLLM;
		st_pll_dev.b_osc_input = MS_TRUE;	
		api_pll_config(&st_pll_dev);
			
		st_dmc_cfg.u32_sdram_spdbin	= SDRAM_SPDBIN_1066M;
		st_dmc_cfg.u32_apb_freq	= 24;
		u8_ddr_status = api_dmc_init(&st_dmc_cfg);
	}
	else
	{
		st_pll_dev.u32_clk_out = 40000 * 10000;
		st_pll_dev.u8_pll_idx =	PLLM;
		st_pll_dev.b_osc_input = MS_TRUE;	
		api_pll_config(&st_pll_dev);
			
		st_dmc_cfg.u32_sdram_spdbin	= 1600;
		st_dmc_cfg.u32_apb_freq	= 24;
		u8_ddr_status = api_dmc_init(&st_dmc_cfg);
	}
	switch(u8_ddr_status)
	{
		case ERR_OK:
			LOG("[INFO] DDR init Ok.");
			break;
		case ERR_FAIL:
			LOG("[INFO] DDR init Fail.");
			break;
		case ERR_INV_PARM:
			LOG("[INFO] DDR init parameter invalid.");
			break;
	}
	api_misc_timing_table_init((MISC_TIMING_T *)g_arrTimingTable, get_timing_table_size());
}


static VOID	_video_port_hd_rx_init(VOID)
{
	UINT8 u8_rx_id;
	for(u8_rx_id = 0; u8_rx_id < 4;	u8_rx_id ++)
	{
		memset(&g_stVideo_HDRx_Timing[u8_rx_id],NULL,sizeof(HD_VIDEO_TIMING_T));
		api_hd_rx_instance_init(&g_st_hd_rx[u8_rx_id], g_u8_HD_RX_PROT[u8_rx_id]);
		g_st_hd_rx[u8_rx_id].u8_rx_idx = g_u8_HD_RX_PROT[u8_rx_id];
		g_st_hd_rx[u8_rx_id].b_hdkey_engine_enable	= MS_FALSE;
		g_st_hd_rx[u8_rx_id].pu8_edid_buf	= (VOID	*)g_u8c_sys_edid_default_buf;
		g_st_hd_rx[u8_rx_id].u16_edid_size = sizeof(g_u8c_sys_edid_default_buf);
		g_st_hd_rx[u8_rx_id].pu8_bksv_buf	= (UINT8 *)NULL;
		g_st_hd_rx[u8_rx_id].pu8_key_buf = (UINT8 *)NULL;
		g_st_hd_rx[u8_rx_id].b_hdp_override_en = MS_FALSE;
		api_hd_rx_init(&g_st_hd_rx[u8_rx_id]);
			
	}
}

static VOID	_tx_csc_cfg(UINT8 u8_txcsc_id)
{
	g_st_txcsc_dev[u8_txcsc_id].u8_sel = g_u8_csc_idx[g_u8_HD_TX_PROT[u8_txcsc_id]];
	api_csc_tx_instance_init(&g_st_txcsc_dev[u8_txcsc_id], g_u8_csc_idx[g_u8_HD_TX_PROT[u8_txcsc_id]]);
	g_st_txcsc_dev[u8_txcsc_id].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
	g_st_txcsc_dev[u8_txcsc_id].st_csc_config_param.csc_out_color_range	= CHIP_TX_CSC_OUT_COLORRANGE;
	g_st_txcsc_dev[u8_txcsc_id].st_csc_config_param.u8_pixel_repeat	= HD_PIXEL_REPEAT_X1;
	g_st_txcsc_dev[u8_txcsc_id].st_csc_config_param.csc_in_colorspace =	CHIP_INTERNAL_PROCESS_COLORSPACE;
	g_st_txcsc_dev[u8_txcsc_id].st_csc_config_param.csc_out_colorspace = CHIP_TX_CSC_OUT_COLORSPACE;
	api_csc_tx_set(&g_st_txcsc_dev[u8_txcsc_id]);
}


static VOID	_video_timgen_cfg(UINT8	u8_timgen_id)
{
	MISC_MODULE_CLK_T st_tmg_src;
	PLL_DEV_T st_pll_dev;
	MISC_MODULE_CLK_T st_module_clk;
	
	api_misc_timing_get(g_arrOutputTable[g_u8_output_index],&g_stVideo_HDTx_Timing);
	
	st_module_clk.u8_clkdiv	= MISC_CLKDIV_1;
	st_module_clk.u8_dst_module	= MISC_VIDEO_MODULE_PLLV1;
	st_module_clk.u8_src_clk = MISC_CLKSRC_XTAL;	
	api_misc_pllv_clkin_sel(&st_module_clk);
	
	st_pll_dev.u8_pll_idx =	PLLV1;
	st_pll_dev.b_osc_input = MS_TRUE;
	st_pll_dev.u32_clk_out = g_stVideo_HDTx_Timing.u16_pixclk * 10000;
	api_pll_config(&st_pll_dev); 
	
	st_tmg_src.u8_src_clk =	MISC_CLKSRC_PLLV1;
	st_tmg_src.u8_dst_module = g_u8_misc_timgen[u8_timgen_id];
	api_misc_timgen_clkin_sel(&st_tmg_src);
	
	api_timgen_instance_init(&g_st_timgen[u8_timgen_id], u8_timgen_id);
	g_st_timgen[u8_timgen_id].u8_workmode =	FreeRun_Mode;
	g_st_timgen[u8_timgen_id].u8_timgen_id = u8_timgen_id;
	g_st_timgen[u8_timgen_id].b_timgen_en =	ENABLE;
	g_st_timgen[u8_timgen_id].pst_timing = &g_stVideo_HDTx_Timing;
		
	api_vds_tmg_init(&g_st_timgen[u8_timgen_id]);
	api_vds_tmg_config(&g_st_timgen[u8_timgen_id]);	
}

VOID _video_port_hd_tx_init(UINT8	u8_index)
{
	UINT8 u8_tx_id = 0;
	gst_inf_pack.st_avi_infoframe.u8_pixel_repeat =	HD_PIXEL_REPEAT_X1;
	gst_inf_pack.st_avi_infoframe.e_scan_mode =	HD_SCAN_MODE_OVERSCAN;
	gst_inf_pack.st_avi_infoframe.e_picture_aspect = HD_PICTURE_ASPECT_16_9;
	gst_inf_pack.st_avi_infoframe.e_colorspace = CHIP_TX_CSC_OUT_COLORSPACE;
	if(gst_inf_pack.st_avi_infoframe.e_colorspace == HD_COLORSPACE_RGB)
		gst_inf_pack.st_avi_infoframe.e_colorimetry	= HD_COLORIMETRY_NONE;
	if(gst_inf_pack.st_avi_infoframe.e_colorspace == HD_COLORSPACE_YCBCR444)
		gst_inf_pack.st_avi_infoframe.e_colorimetry	= HD_COLORIMETRY_ITU_601;
	gst_inf_pack.st_avi_infoframe.e_extended_colorimetry = HD_EXTENDED_COLORIMETRY_OPRGB;
	for(u8_tx_id = 0; u8_tx_id < 4;	u8_tx_id ++)
	{
		api_hd_tx_instance_init(&g_st_hd_tx[u8_tx_id], g_u8_HD_TX_PROT[u8_tx_id]);
		api_misc_timing_get(g_arrOutputTable[u8_index],&g_stVideo_HDTx_Timing);
		g_st_hd_tx[u8_tx_id].u8_txIndex =	g_u8_HD_TX_PROT[u8_tx_id];
		g_st_hd_tx[u8_tx_id].b_is_hd = TRUE;
		g_st_hd_tx[u8_tx_id].b_full_edid = 0;
		g_st_hd_tx[u8_tx_id].u8_color_depth =	HD_COLOR_DEPTH_24;
		g_st_hd_tx[u8_tx_id].u8_audio_mode = HD_AUDIO_STREAM_TYPE_MAS;
		g_st_hd_tx[u8_tx_id].u8_audio_src	= FROM_HD_RX_AUDIO;
		g_st_hd_tx[u8_tx_id].u8_video_clk_src	= FROM_RX_OR_PLL_PIXEL_CLK;
		g_st_hd_tx[u8_tx_id].u32_tmds_clk	= g_stVideo_HDTx_Timing.u16_pixclk;
		g_st_hd_tx[u8_tx_id].pu8_edid_buf	= (VOID	*)hd_tx_edid;
		g_st_hd_tx[u8_tx_id].pu8_bksv_buf	= (UINT8 *)NULL;
		g_st_hd_tx[u8_tx_id].pu8_key_buf = (UINT8 *)NULL;
		g_st_hd_tx[u8_tx_id].pst_edid_info = (VOID *)&g_hd_edid_flag[u8_tx_id];
		api_hd_tx_init(&g_st_hd_tx[u8_tx_id]);
		memcpy((&g_st_hd_tx[u8_tx_id].st_infoframes_packets), &gst_inf_pack, sizeof(HD_INFOFRAMES_PACKETS_T));
		api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id],MS_FALSE);
		api_hd_tx_config_output(&g_st_hd_tx[u8_tx_id]);
		_tx_csc_cfg(u8_tx_id);
		
		g_st_tx_mux[u8_tx_id].u8_tx_module = MISC_VIDEO_MODULE_TX1 + u8_tx_id;
		g_st_tx_mux[u8_tx_id].u8_video_mux = MISC_VIDEO_MODULE_MIX1	+ u8_tx_id;
		api_misc_tx_mux_cfg(&g_st_tx_mux[u8_tx_id]);

		api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id], MS_TRUE);
	}
    
}

VOID _video_mute(UINT8 u8_path, BOOL b_mute)
{
	switch(g_u8_display_mode)
	{
		case MS1826_SEAMLESS_MATRIX_SWITCHER:
			single_video_mute(u8_path, b_mute);
			break;
		case MS1826_VIDEO_SPLITTER:
			splitter_video_mute(u8_path, b_mute);
			break;
		case MS1826_VIDEO_SPLICER:
			splicer_video_mute(u8_path, b_mute);
			break;
	}
}

VOID video_process_init(VOID)
{
	b_hpd_bak[0] = 0;
	b_hpd_bak[1] = 0;
	b_hpd_bak[2] = 0;
	b_hpd_bak[3] = 0;
	_ms1826_init();	
	_video_port_hd_rx_init();	
	_video_timgen_cfg(TIMGEN_1);
	_video_port_hd_tx_init(g_u8_output_index);
	g_st_app_splitter_dev.u8_dst_sel = 0;
	g_st_app_splitter_dev.b_splitter_sw	= MS_FALSE;
	gst_splicer_app_dev.b_splicer_sw = MS_FALSE;
	gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
	switch(g_u8_display_mode)
	{
		case MS1826_SEAMLESS_MATRIX_SWITCHER:
			app_single_mode_config();
			LOG("[INFO] MS1826 function is SEAMLESS_MATRIX_SWITCHER.");
			break;
		case MS1826_VIDEO_SPLITTER:
			app_splitter_mode_config(&g_st_app_splitter_dev);
			LOG("[INFO] MS1826 function is VIDEO_SPLITTER.");
			break;
		case MS1826_VIDEO_SPLICER:
			app_splicer_mode_config(&gst_splicer_app_dev);
			LOG("[INFO] MS1826 function is VIDEO_SPLICER.");
			break;
		case MS1826_BYPASS_MATRIX_SWITCHER:
			Bypass_Tx_Status[0]	= 1;
			Bypass_Tx_Status[1]	= 1;
			Bypass_Tx_Status[2]	= 1;
			Bypass_Tx_Status[3]	= 1;
			LOG("[INFO] MS1826 function is BYPASS_MATRIX_SWITCHER.");
			break;
	}
	
	_video_mute(HD_RX_A, MS_TRUE);
	_video_mute(HD_RX_B, MS_TRUE);
	_video_mute(HD_RX_C, MS_TRUE);
	_video_mute(HD_RX_D, MS_TRUE);
	app_osd_init();
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}

VOID video_process_cfg(VOID)
{
	_ms1826_init();	
	_video_port_hd_rx_init();	
	_video_timgen_cfg(TIMGEN_1);
	_video_port_hd_tx_init(g_u8_output_index);	
	app_osd_init();
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}

VOID _video_misc_process(UINT8 u8_rx_id)
{
	UINT8 u8_vipss_id = 0;
	//require positive polarity input
	g_st_rx_polarity[u8_rx_id].u8_chan = g_u8_misc_module_rx[g_u8_HD_RX_PROT[u8_rx_id]];
	switch(g_stVideo_HDRx_Timing[u8_rx_id].u8_polarity)
	{
		case ProgrVNegHNeg:
		case InterVNegHNeg:
			g_st_rx_polarity[u8_rx_id].u8_hsync_flip_en	= MS_TRUE;
			g_st_rx_polarity[u8_rx_id].u8_vsync_flip_en	= MS_TRUE;
			break;
		case ProgrVNegHPos:
		case InterVNegHPos:
			g_st_rx_polarity[u8_rx_id].u8_hsync_flip_en	= MS_FALSE;
			g_st_rx_polarity[u8_rx_id].u8_vsync_flip_en	= MS_TRUE;
			break;
		case ProgrVPosHNeg:
		case InterVPosHNeg:
			g_st_rx_polarity[u8_rx_id].u8_hsync_flip_en	= MS_TRUE;
			g_st_rx_polarity[u8_rx_id].u8_vsync_flip_en	= MS_FALSE;
			break;
		case ProgrVPosHPos:
		case InterVPosHPos:
			g_st_rx_polarity[u8_rx_id].u8_hsync_flip_en	= MS_FALSE;
			g_st_rx_polarity[u8_rx_id].u8_vsync_flip_en	= MS_FALSE;
			break;
	}
	api_misc_rx_polarity_cfg(&g_st_rx_polarity[u8_rx_id]);
	
	api_hd_rx_get_avi_infoframe(&g_st_hd_rx[u8_rx_id]);
	
	for(u8_vipss_id = 0; u8_vipss_id < 4; u8_vipss_id ++)
	{
		if(Vipss_Mapping_Rx[u8_vipss_id] == g_u8_HD_RX_PROT[u8_rx_id])
		{
			g_st_vipss_csc[u8_vipss_id].u8_sel = g_u8_csc_idx[u8_vipss_id];
			g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_colorspace	= 
				g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_colorspace;
			if(g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_colorspace == HD_COLORSPACE_RGB)
			{
				if(g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_rgb_quantization_range <	HD_QUANTIZATION_RANGE_FULL)
				{
					g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_color_range = CSC_RGB_16_235;
				}	
				else
				{
					g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_color_range = CSC_RGB_0_255;
				}
			}
			else if(g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_colorspace > HD_COLORSPACE_RGB &&
					g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_colorspace < HD_COLORSPACE_RESERVED4)
			{
				if(g_st_hd_rx[u8_rx_id].st_packets_infoframes.st_avi_infoframe.e_colorimetry == HD_COLORIMETRY_ITU_709)
				{
					g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT709;
				}	
				else
				{
					g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT601;
				}
			}
			if(g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_colorspace == HD_COLORSPACE_YCBCR420)
			{
				g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
				g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
			}
			g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
			g_st_vipss_csc[u8_vipss_id].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
			g_st_vipss_csc[u8_vipss_id].st_csc_config_param.u8_pixel_repeat = HD_PIXEL_REPEAT_X1;
			api_csc_rx_set(&g_st_vipss_csc[u8_vipss_id]);	
		}
	}
}

static BOOL	_diff_compare(UINT16 a,	UINT16 b)
{
	UINT16 u16diff = (a	> b) ? a - b : b - a;
	return u16diff < 10;
}

static BOOL	_timing_compare(HD_VIDEO_TIMING_T	*pstTiming1, HD_VIDEO_TIMING_T *pstTiming2)
{
	if (pstTiming1->u8_polarity	== pstTiming2->u8_polarity &&
		_diff_compare(pstTiming1->u16_htotal,	pstTiming2->u16_htotal)	&&
		_diff_compare(pstTiming1->u16_vtotal,	pstTiming2->u16_vtotal)	&&
		_diff_compare(pstTiming1->u16_hactive, pstTiming2->u16_hactive)	&&
		_diff_compare(pstTiming1->u16_vactive, pstTiming2->u16_vactive)	&&
		_diff_compare(pstTiming1->u16_pixclk,	pstTiming2->u16_pixclk)	&&
		_diff_compare(pstTiming1->u16_vfreq, pstTiming2->u16_vfreq)	&&
		_diff_compare(pstTiming1->u16_hoffset, pstTiming2->u16_hoffset)	&&
		_diff_compare(pstTiming1->u16_voffset, pstTiming2->u16_voffset)	&&
		_diff_compare(pstTiming1->u16_hsyncwidth,	pstTiming2->u16_hsyncwidth)	&&
		_diff_compare(pstTiming1->u16_vsyncwidth,	pstTiming2->u16_vsyncwidth))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static VOID _video_tx_process()
{
	BOOL b_hpd = 0;
	UINT8 u8_tx_id = 0;
	
	for(u8_tx_id = 0; u8_tx_id < 4; u8_tx_id ++)
	{
		b_hpd = api_hd_tx_get_hpd_status(&g_st_hd_tx[u8_tx_id]);
		if(b_hpd !=	b_hpd_bak[u8_tx_id])
		{
			b_hpd_bak[u8_tx_id]	= b_hpd;
			if(b_hpd)
			{
				switch(u8_tx_id)
				{
					case 0:
						HD_TX_LED1(TRUE);
						LOG("[INFO] HD Tx1 connected..");
						break;
					case 1:
						HD_TX_LED2(TRUE);
						LOG("[INFO] HD Tx2 connected..");
						break;
					case 2:
						HD_TX_LED3(TRUE);
						LOG("[INFO] HD Tx3 connected..");
						break;
					case 3:
						HD_TX_LED4(TRUE);
						LOG("[INFO] HD Tx4 connected..");
						break;
				}
			}
			else
			{
				switch(u8_tx_id)
				{
					case 0:
						HD_TX_LED1(FALSE);
						LOG("[INFO] HD Tx1 disconnected..");
						break;
					case 1:
						HD_TX_LED2(FALSE);
						LOG("[INFO] HD Tx2 disconnected..");
						break;
					case 2:
						HD_TX_LED3(FALSE);
						LOG("[INFO] HD Tx3 disconnected..");
						break;
					case 3:
						HD_TX_LED4(FALSE);
						LOG("[INFO] HD Tx4 disconnected..");
						break;
				}
			}
		}
	}
}

static VOID _video_rx_process(UINT8 u8_rx_idx)
{
	UINT8 u8_tx_id = 0;
	api_hd_rx_task(&g_st_hd_rx[u8_rx_idx]);	
	if(api_hd_rx_is_audio_changed(&g_st_hd_rx[u8_rx_idx]))
	{
		for(u8_tx_id = 0; u8_tx_id < 4; u8_tx_id ++)
		{
			if(u8_Tx_audio_sel[u8_tx_id] == u8_rx_idx)
			{
				api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx_id], MS_TRUE);
				memcpy((&g_st_hd_tx[u8_tx_id].st_infoframes_packets.st_audio_infoframe), &g_st_hd_rx[u8_rx_idx].st_packets_infoframes.st_audio_infoframe, sizeof(HD_AUDIO_INFOFRAME_T));
				api_hd_tx_audio_change(&g_st_hd_tx[u8_tx_id]);
				api_hd_tx_audio_mute(&g_st_hd_tx[u8_tx_id], b_audio_mute);
			}
		}
	}
	if(api_hd_rx_is_video_changed(&g_st_hd_rx[u8_rx_idx]))
	{	
		_video_mute(u8_rx_idx, TRUE);
		switch (u8_rx_idx)
		{
			case 0:
				HD_RX_LED1(FALSE);
				break;
			case 1:
				HD_RX_LED2(FALSE);
				break;
			case 2:
				HD_RX_LED3(FALSE);
				break;
			case 3:
				HD_RX_LED4(FALSE);
				break;
			default:
				break;
		}		
		if(api_hd_rx_get_video_timing(&g_st_hd_rx[u8_rx_idx]) == ERR_OK)
		{
			if(_timing_compare(&g_stVideo_HDRx_Timing[u8_rx_idx],	&(g_st_hd_rx[u8_rx_idx].st_rx_video_timing)) == FALSE)
			{
				memcpy(&g_stVideo_HDRx_Timing[u8_rx_idx],&(g_st_hd_rx[u8_rx_idx].st_rx_video_timing),sizeof(HD_VIDEO_TIMING_T));
				if(g_stVideo_HDRx_Timing[u8_rx_idx].u8_polarity % 2 == 0)
				{
					g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive /= 2;
				}
				b_RxVideoChg[u8_rx_idx] = TRUE;
			}
			else
			{
				if(b_RxVideoChg[u8_rx_idx] == MS_FALSE)
				{
					_video_mute(u8_rx_idx, FALSE);
				}
			}
		}
		else
		{
			memset(&g_stVideo_HDRx_Timing[u8_rx_idx],NULL,sizeof(HD_VIDEO_TIMING_T));
		}
		g_bOSDShown = MS_TRUE;
		sys_show_osd(g_bOSDShown, 0x0F);
		_video_misc_process(u8_rx_idx);	
	}
	else
	{
		if(g_st_hd_rx[u8_rx_idx].u8_rx_state == RX_STATE_RUNNING && b_RxVideoChg[u8_rx_idx])
		{	
			b_RxVideoChg[u8_rx_idx] = FALSE;
			switch(g_u8_display_mode)
			{
				case MS1826_SEAMLESS_MATRIX_SWITCHER:	 
					app_single_process(u8_rx_idx); 
					break;
				case MS1826_VIDEO_SPLITTER:
					app_splitter_process(u8_rx_idx);									 
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_src_sel == u8_rx_idx)
					{			 
						app_splicer_process(&gst_splicer_app_dev);
					}	
					break;
			}		
			mculib_delay_ms(200);
			video_bypass_tx_cfg(u8_rx_idx);
			switch (u8_rx_idx)
			{
				case 0:
					HD_RX_LED1(TRUE);
					LOG1("[INFO] hd1 cur status: ", g_st_hd_rx[u8_rx_idx].u8_rx_state);
					break;
				case 1:
					HD_RX_LED2(TRUE);
					LOG1("[INFO] hd2 cur status: ", g_st_hd_rx[u8_rx_idx].u8_rx_state);
					break;
				case 2:
					HD_RX_LED3(TRUE);
					LOG1("[INFO] hd3 cur status: ", g_st_hd_rx[u8_rx_idx].u8_rx_state);
					break;
				case 3:
					HD_RX_LED4(TRUE);
					LOG1("[INFO] hd4 cur status: ", g_st_hd_rx[u8_rx_idx].u8_rx_state);
					break;
				default:
					break;
			}
			_video_mute(u8_rx_idx, FALSE);
		}
	}
	if(g_st_hd_rx[u8_rx_idx].u8_rx_state != RX_STATE_RUNNING)
		_video_mute(u8_rx_idx, TRUE);
}

VOID video_process(VOID)
{
	static UINT8 u8_rx_idx = 0;

	_video_tx_process();
	_video_rx_process(u8_rx_idx);

	u8_rx_idx ++;
	if(u8_rx_idx >=	4)
		u8_rx_idx =	0;
}

VOID osd_service(VOID)
{
	UINT8 u8_osd_time;
	
	if(g_u8_time == 0)
		u8_osd_time =10;
	else if(g_u8_time == 1)
		u8_osd_time =30;
	else if(g_u8_time == 2)
		u8_osd_time =60;
	
    if (!g_bOSDShown && !b_osd_menu)
    {
        g_u8OSDCount = 0;
        return;
    }

    g_u8OSDCount ++;

    if (g_u8OSDCount > u8_osd_time) // count every 500ms 
    {
        g_u8OSDCount = 0;
        sys_show_osd(FALSE, 0x0F);
        g_bOSDShown = FALSE;
    }
}

