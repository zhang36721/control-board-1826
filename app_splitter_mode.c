#ifndef _APP_SPLITTER_MODE_C_
#define _APP_SPLITTER_MODE_C_

#include "videoprocess.h"

DMA_COMMON_DEV_T st_test_dma_com_dev;
	
SPLITTER_VIPSS_T gst_splitter_vipss_dev[4];
SPLITTER_VOPSS_T gst_splitter_vopss_dev[4];
SPLITTER_APP_T g_st_app_splitter_dev;
UINT8 g_u8_splitter_audio_src = AUDIO_HD_RX1;/* ÒôÆµ·Ö¸îÆ÷µÄÒôÆµÔ´ _by_zzw*/

static VOID app_splitter_vipss_instance_init(SPLITTER_VIPSS_T *pst_splitter_vipss_dev)
{
	api_4ksdn_instance_init(&pst_splitter_vipss_dev->st_sdn4k_dev, g_u8_4ksdn_idx[g_u8_rx_idx[pst_splitter_vipss_dev->u8_vipss_sel]]);
	api_sdn_instance_init(&pst_splitter_vipss_dev->st_splitter_sdn_dev, g_u8_sdn_idx[g_u8_rx_idx[pst_splitter_vipss_dev->u8_vipss_sel]]);
	api_vwdma_instance_init(&pst_splitter_vipss_dev->st_splitter_vwdma_dev, g_u8_dma_idx[g_u8_rx_idx[pst_splitter_vipss_dev->u8_vipss_sel]]);
	api_dma_common_instance_init(&pst_splitter_vipss_dev->st_splitter_com_wdma_dev, g_u8_dma_idx[g_u8_rx_idx[pst_splitter_vipss_dev->u8_vipss_sel]]);  
}

static VOID app_splitter_vipss_rx_mux_cfg(UINT8 u8_vipss_sel, SPLITTER_APP_T *pst_app_splitter_dev)
{
	if(pst_app_splitter_dev->u8_splitter_mode < MODE_DUAL_UP1_DOWN2)
	{
		gst_splitter_vipss_dev[u8_vipss_sel].st_vipss_mux_dev.u8_src_module = g_u8_misc_module_rx[g_u8_HD_RX_PROT[pst_app_splitter_dev->u8_splitter_mode]];
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace = g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].u8_color_space;
	}
	else
	{
		gst_splitter_vipss_dev[u8_vipss_sel].st_vipss_mux_dev.u8_src_module = g_u8_misc_module_rx[g_u8_HD_RX_PROT[u8_vipss_sel]];
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace = g_st_hd_rx[u8_vipss_sel].u8_color_space ;
	}

	//vipss rx mux config
	gst_splitter_vipss_dev[u8_vipss_sel].st_vipss_mux_dev.u8_dst_module = g_u8_misc_module_vipss[u8_vipss_sel];
	api_misc_rx_mux_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_vipss_mux_dev);
			 
	if(g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace == HD_COLORSPACE_YCBCR420)
	{
		//vipss rx csc config
		g_st_vipss_csc[u8_vipss_sel].u8_sel = g_u8_csc_idx[u8_vipss_sel];
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		api_csc_rx_set(&g_st_vipss_csc[u8_vipss_sel]);
		//vipss mux config
		api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_5);       //< CSC -> 4KSDN -> SDN -> VWDMA1 */
		
		gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_idx = g_u8_4ksdn_idx[u8_vipss_sel];
		api_sdn4k_init(&gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
		gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_input_colorSpace = HD_COLORSPACE_YCBCR420;
		gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_output_colorSpace = HD_COLORSPACE_YCBCR444;
		gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_mode = SDN4K_MODE_AUTO;
		api_sdn4k_config(&gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
		api_sdn4k_enable(&gst_splitter_vipss_dev[u8_vipss_sel].st_sdn4k_dev, MS_ENABLE);
		if(pst_app_splitter_dev->u8_splitter_mode < MODE_DUAL_UP1_DOWN2)
			g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_vactive = g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].st_rx_video_timing.u16_vactive / 2;
		else
			g_stVideo_HDRx_Timing[u8_vipss_sel].u16_vactive = g_st_hd_rx[u8_vipss_sel].st_rx_video_timing.u16_vactive / 2;
	}
	else
	{
		//vipss rx csc config
		g_st_vipss_csc[u8_vipss_sel].u8_sel = g_u8_csc_idx[u8_vipss_sel];
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		api_csc_rx_set(&g_st_vipss_csc[u8_vipss_sel]);
		
		if(pst_app_splitter_dev->u8_splitter_mode < MODE_DUAL_UP1_DOWN2 &&
			g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].st_rx_video_timing.u8_polarity % 2 == 0)
		{
			switch(g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].st_rx_video_timing.u16_vactive)
			{
				case 1080:
					//vipss mux config
					api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_8);       //< CSC ->DEINT-> SDN -> VWDMA1
					//vipss rx deint config
					g_st_deint_dev.b_yuv444 = MS_TRUE;
					g_st_deint_dev.b_deint_en = MS_TRUE;
					g_st_deint_dev.u16_vactive = 1080;
					g_st_deint_dev.b_hd_full_mode = MS_TRUE;
					g_st_deint_dev.u32_buff0_start_addr = 0;
					g_st_deint_dev.u32_buff1_start_addr = 4000000;
					api_deint_init(&g_st_deint_dev);
					api_deint_config(&g_st_deint_dev);
					g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_vactive = 1080;
					break;
				case 480:
					//vipss mux config
					api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_8);       //< CSC ->DEINT-> SDN -> VWDMA1
					//vipss rx deint config
					g_st_deint_dev.b_yuv444 = MS_TRUE;
					g_st_deint_dev.b_deint_en = MS_TRUE;
					g_st_deint_dev.u16_vactive = 480;
					g_st_deint_dev.b_hd_full_mode = MS_FALSE;
					g_st_deint_dev.u32_buff0_start_addr = 0;
					g_st_deint_dev.u32_buff1_start_addr = 4000000;
					api_deint_init(&g_st_deint_dev);
					api_deint_config(&g_st_deint_dev);
					g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_hactive = g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].st_rx_video_timing.u16_hactive/2;
					g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_vactive = 480;
					break;
				case 576:
					//vipss mux config
					api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_8);       //< CSC ->DEINT-> SDN -> VWDMA1
					//vipss rx deint config
					g_st_deint_dev.b_yuv444 = MS_TRUE;
					g_st_deint_dev.b_deint_en = MS_TRUE;
					g_st_deint_dev.u16_vactive = 576;
					g_st_deint_dev.b_hd_full_mode = MS_FALSE;
					g_st_deint_dev.u32_buff0_start_addr = 0;
					g_st_deint_dev.u32_buff1_start_addr = 4000000;
					api_deint_init(&g_st_deint_dev);
					api_deint_config(&g_st_deint_dev);
					g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_hactive = g_st_hd_rx[pst_app_splitter_dev->u8_splitter_mode].st_rx_video_timing.u16_hactive/2;
					g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_vactive = 576;
					break;
				default:
					g_st_deint_dev.b_deint_en = MS_FALSE;
					api_deint_config(&g_st_deint_dev);
					//vipss mux config
					api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_4);       //< CSC -> SDN -> VWDMA1
					break;
			}
		}
		else
		{
			if(g_st_hd_rx[u8_vipss_sel].st_rx_video_timing.u8_polarity % 2 == 0)
			{
				g_st_deint_dev.b_deint_en = MS_FALSE;
				api_deint_config(&g_st_deint_dev);
				g_stVideo_HDRx_Timing[u8_vipss_sel].u16_hactive = g_st_hd_rx[u8_vipss_sel].st_rx_video_timing.u16_hactive;
				g_stVideo_HDRx_Timing[u8_vipss_sel].u16_vactive = g_st_hd_rx[u8_vipss_sel].st_rx_video_timing.u16_vactive / 2;
			
			}
			//vipss mux config
			api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_4);       //< CSC -> SDN -> VWDMA1
		}
	}
}

static VOID app_splitter_vipss_cfg(UINT8 u8_vipss_sel, SPLITTER_APP_T *pst_app_splitter_dev)
{
	//vipss sdn config
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.u8_sel = g_u8_sdn_idx[u8_vipss_sel];
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.b_enable = MS_TRUE;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.u8_sdn_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.st_video_size.st_InRect.u16_h = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tInSize.u16_h;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.st_video_size.st_InRect.u16_v = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tInSize.u16_v;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tMemSize.u16_h;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tMemSize.u16_v;
	api_sdn_enable_set(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev);
	
	api_sdn_scaler_size_set(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev);
	api_sdn_csc_set(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_sdn_dev);

	//vipss vwdma config
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u8_sel = g_u8_dma_idx[u8_vipss_sel];
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.b_enable = MS_FALSE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u16_request_len = 128;
	api_vwdma_request_len_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
		
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u8_sel = g_u8_dma_idx[u8_vipss_sel];
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.b_enable = MS_TRUE;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.st_vwdma_in_size.u16_h_size = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tMemSize.u16_h;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.st_vwdma_in_size.u16_v_size = pst_app_splitter_dev->st_scale_size[u8_vipss_sel].tMemSize.u16_v;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u8_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
			
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[u8_vipss_sel].u32_dma_start_addr_0;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[u8_vipss_sel].u32_dma_start_addr_1;
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[u8_vipss_sel].u32_dma_start_addr_2;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
	
	api_vwdma_start_addr_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u16_line_offset = g_st_app_splitter_dev.st_scale_size[u8_vipss_sel].tMemSize.u16_h;
	api_vwdma_line_offset_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
	api_vwdma_burst_num_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);
	
	gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev.u8_vwdma_rotatation_deg = ROTATION_0_DEG;
	api_vwdma_rotation_cfg(&gst_splitter_vipss_dev[u8_vipss_sel].st_splitter_vwdma_dev);				
}

static VOID app_splitter_vopss_instance_init(SPLITTER_VOPSS_T *pst_splitter_vopss_dev)
{
	api_vrdma_instance_init(&pst_splitter_vopss_dev->st_splitter_vrdma_dev, g_u8_dma_idx[g_u8_rx_idx[pst_splitter_vopss_dev->u8_vopss_sel]]);
	
	api_su_instance_init(&pst_splitter_vopss_dev->st_splitter_su_dev, g_u8_su_idx[g_u8_rx_idx[pst_splitter_vopss_dev->u8_vopss_sel]]);
}

static VOID app_splitter_vopss_cfg(UINT8 u8_vopss_sel, SPLITTER_APP_T *pst_app_splitter_dev)
{
	MISC_MODULE_CLK_T st_module_clk;
	
	//vopss VRDMA config
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u8_sel = g_u8_dma_idx[u8_vopss_sel];
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.b_enable = MS_FALSE;
	api_vrdma_enable_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u16_request_len = 128;
	api_vrdma_request_len_cfg(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u8_sel = g_u8_dma_idx[u8_vopss_sel];
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.b_enable = MS_TRUE;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u8_mem_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.st_vrdma_out_size.u16_h_size = pst_app_splitter_dev->st_scale_size[u8_vopss_sel].tMemSize.u16_h;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.st_vrdma_out_size.u16_v_size = pst_app_splitter_dev->st_scale_size[u8_vopss_sel].tMemSize.u16_v;

	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u16_line_offset = gst_splitter_vipss_dev[u8_vopss_sel].st_splitter_vwdma_dev.u16_line_offset;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[u8_vopss_sel].u32_dma_start_addr_0;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[u8_vopss_sel].u32_dma_start_addr_1;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[u8_vopss_sel].u32_dma_start_addr_2;   
		
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev.u8_mirror_cfg = DMA_H_V_MIRROR_ALL_DISABLE;           
	api_vrdma_mirror_cfg(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	api_vrdma_enable_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	api_vrdma_start_addr_cfg(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	api_vrdma_burst_num_cfg(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
	api_vrdma_line_offset_cfg(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_vrdma_dev);
			
	// su clk src
	st_module_clk.u8_dst_module = g_u8_misc_su[u8_vopss_sel];
	st_module_clk.u8_src_clk = MISC_CLKSRC_VDSTMG1;
	st_module_clk.u8_clkdiv = MISC_CLKDIV_1;
	api_misc_su_clkin_sel(&st_module_clk);
	
	//vopss su config
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.u8_sel = g_u8_su_idx[u8_vopss_sel];
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.b_enable = MS_TRUE;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.u8_su_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;

	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_size.st_MemRect.u16_h = pst_app_splitter_dev->st_scale_size[u8_vopss_sel].tMemSize.u16_h;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_size.st_MemRect.u16_v = pst_app_splitter_dev->st_scale_size[u8_vopss_sel].tMemSize.u16_v;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_size.st_OutRect.u16_h = 
	pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_sp - pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_size.st_OutRect.u16_v = 
	pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_sp - pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_win.u16_hde_st = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_win.u16_hde_sp = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_sp;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_win.u16_vde_st = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_video_win.u16_vde_sp = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_sp;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_dis_win.u8_dis_win_en = MS_TRUE;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_dis_win.u16_hde_st = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_dis_win.u16_hde_sp = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_hde_sp;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_dis_win.u16_vde_st = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_st;
	gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev.st_dis_win.u16_vde_sp = pst_app_splitter_dev->st_video_win[u8_vopss_sel].u16_vde_sp;
	api_su_enable_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev);
	api_su_scaler_size_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev);
	api_su_csc_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev);
	api_su_video_win_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev);
	api_su_display_win_set(&gst_splitter_vopss_dev[u8_vopss_sel].st_splitter_su_dev);        
}
	
static VOID _splitter_mode_frc_cfg(void)
{
	 //DMA config
	gst_splitter_vipss_dev[VIPSS1].st_splitter_ss_vipss_dev.u8_sel = g_u8_ss_ch_idx[VIPSS1];
	gst_splitter_vipss_dev[VIPSS1].st_splitter_ss_vipss_dev.u8_vipss_wdma_id_sel = VOPSS1;
	api_ss_vipss_wdma_buf_sel(&gst_splitter_vipss_dev[VIPSS1].st_splitter_ss_vipss_dev);   

	gst_splitter_vipss_dev[VIPSS2].st_splitter_ss_vipss_dev.u8_sel = g_u8_ss_ch_idx[VIPSS2];
	gst_splitter_vipss_dev[VIPSS2].st_splitter_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS2;
	api_ss_vipss_wdma_buf_sel(&gst_splitter_vipss_dev[VIPSS2].st_splitter_ss_vipss_dev); 

	gst_splitter_vipss_dev[VIPSS3].st_splitter_ss_vipss_dev.u8_sel = g_u8_ss_ch_idx[VIPSS3];
	gst_splitter_vipss_dev[VIPSS3].st_splitter_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS3;
	api_ss_vipss_wdma_buf_sel(&gst_splitter_vipss_dev[VIPSS3].st_splitter_ss_vipss_dev); 

	gst_splitter_vipss_dev[VIPSS4].st_splitter_ss_vipss_dev.u8_sel = g_u8_ss_ch_idx[VIPSS4];
	gst_splitter_vipss_dev[VIPSS4].st_splitter_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS4;
	api_ss_vipss_wdma_buf_sel(&gst_splitter_vipss_dev[VIPSS4].st_splitter_ss_vipss_dev);     
		
	gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev.u8_sel = g_u8_dma_idx[VOPSS1];
	gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev.u8_dma_buf_num_sel = SPLITTER_BUF_NUM;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO;  
	gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_ss_vopss_dev.u8_sel = g_u8_ss_ch_idx[VOPSS1];
	gst_splitter_vopss_dev[VOPSS1].st_splitter_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS1;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS1;
	api_dma_common_buf_num_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_splitter_vopss_dev[VOPSS1].st_splitter_com_rdma_dev);   
	api_ss_vopss_src_vb_sel(&gst_splitter_vopss_dev[VOPSS1].st_splitter_ss_vopss_dev); 
	api_ss_vopss_dst_buf_id_sel(&gst_splitter_vopss_dev[VOPSS1].st_splitter_ss_vopss_dev);
	
	gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev.u8_sel = g_u8_dma_idx[VOPSS2];
	gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev.u8_dma_buf_num_sel = SPLITTER_BUF_NUM;
	gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE; 
	gst_splitter_vopss_dev[VOPSS2].st_splitter_ss_vopss_dev.u8_sel = g_u8_ss_ch_idx[VOPSS2];
	gst_splitter_vopss_dev[VOPSS2].st_splitter_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS2;
	gst_splitter_vopss_dev[VOPSS2].st_splitter_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS2;
	api_dma_common_buf_num_set(&gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_splitter_vopss_dev[VOPSS2].st_splitter_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_splitter_vopss_dev[VOPSS2].st_splitter_ss_vopss_dev); 
	api_ss_vopss_dst_buf_id_sel(&gst_splitter_vopss_dev[VOPSS2].st_splitter_ss_vopss_dev);

	gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev.u8_sel = g_u8_dma_idx[VOPSS3];
	gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev.u8_dma_buf_num_sel = SPLITTER_BUF_NUM;
	gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE;
	gst_splitter_vopss_dev[VOPSS3].st_splitter_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS3;
	gst_splitter_vopss_dev[VOPSS3].st_splitter_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS3;
	api_dma_common_buf_num_set(&gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_splitter_vopss_dev[VOPSS3].st_splitter_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_splitter_vopss_dev[VOPSS3].st_splitter_ss_vopss_dev); 
	api_ss_vopss_dst_buf_id_sel(&gst_splitter_vopss_dev[VOPSS3].st_splitter_ss_vopss_dev);
		
	gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev.u8_sel = g_u8_dma_idx[VOPSS4];
	gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev.u8_dma_buf_num_sel = SPLITTER_BUF_NUM;
	gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE;
	gst_splitter_vopss_dev[VOPSS4].st_splitter_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS4;
	gst_splitter_vopss_dev[VOPSS4].st_splitter_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS4;
	api_dma_common_buf_num_set(&gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_splitter_vopss_dev[VOPSS4].st_splitter_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_splitter_vopss_dev[VOPSS4].st_splitter_ss_vopss_dev); 
	api_ss_vopss_dst_buf_id_sel(&gst_splitter_vopss_dev[VOPSS4].st_splitter_ss_vopss_dev);
											
}
VOID _splitter_video_freeze(UINT8 u8_path_sel)
{
	BOOL b_vb_h;
	DMA_COMMON_DEV_T st_dma_dev;
	//video freeze
	__disable_irq();   
	gst_splitter_vipss_dev[u8_path_sel].st_splitter_com_wdma_dev.u8_sel = g_u8_dma_idx[u8_path_sel];
	gst_splitter_vipss_dev[u8_path_sel].st_splitter_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_MANU;
	gst_splitter_vipss_dev[u8_path_sel].st_splitter_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_MANU;
		 
	st_dma_dev.u8_sel = g_u8_dma_idx[u8_path_sel];
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vipss_dev[u8_path_sel].st_splitter_com_wdma_dev);
	g_u16_timer_out = 0;
	do
	{
		g_u16_timer_out ++;
		b_vb_h = api_misc_vb_status_get(g_u8_vb_path[u8_path_sel]);  
	}   
	while(b_vb_h == MS_FALSE && (g_u16_timer_out < 500));   
	api_dma_common_trigger(&st_dma_dev);  
	api_dma_common_buf_id_get(&st_dma_dev); 
	u8_rdma_buf_id = st_dma_dev.st_dma_status.u8_rdma_buf_id;
	u8_wdma_buf_id = (u8_rdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = u8_wdma_buf_id;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_rdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);
	api_dma_common_trigger(&st_dma_dev);
	__enable_irq(); 
}

VOID _splitter_video_unfreeze(void)
{
	BOOL b_int;
	DMA_COMMON_DEV_T st_dma_dev;
	SS_DEV_T st_vopss_ss_dev;
	
	st_vopss_ss_dev.u8_sel = g_u8_ss_ch_idx[VOPSS1] | g_u8_ss_ch_idx[VOPSS2] | g_u8_ss_ch_idx[VOPSS3] | g_u8_ss_ch_idx[VOPSS4] ;
	st_vopss_ss_dev.st_vopss_trigger_mode.u8_trigger_sel = TRIGGER_BY_VOPSS_TRIGGER;
	st_vopss_ss_dev.st_vopss_trigger_mode.u8_trigger_tmg = VDS_TMG1;
	st_vopss_ss_dev.st_vopss_trigger_mode.u8_trigger_mode = FORCE_TRIGGER;
	api_ss_vopss_trig_mode_cfg(&st_vopss_ss_dev);

	__disable_irq();   
	st_dma_dev.u8_sel = g_u8_dma_idx[0];
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);

	st_dma_dev.u8_sel = g_u8_dma_idx[1];
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);
	
	st_dma_dev.u8_sel = g_u8_dma_idx[2];
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);
	
	st_dma_dev.u8_sel = g_u8_dma_idx[3];
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);
	
	g_u16_timer_out = 0;
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	do
	{
		g_u16_timer_out ++;        
	}
	while(!api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB) && (g_u16_timer_out < 500));     
	api_ss_vopss_simultaneous_trig(&st_vopss_ss_dev);
	api_video_mixer_layer_only_config(&g_st_video_mixer_dev[VIDEO_MIXER_1]);
	
	st_dma_dev.u8_sel = g_u8_dma_idx[0];
	api_dma_common_fifo_status_get(&st_dma_dev);
	if(st_dma_dev.st_dma_status.b_h2l_status == MS_TRUE)
	{
		st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 2) % 3;
		st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
		api_dma_comom_buf_id_set(&st_dma_dev);
	}

	st_dma_dev.u8_sel = g_u8_dma_idx[1];
	api_dma_common_fifo_status_get(&st_dma_dev);
	if(st_dma_dev.st_dma_status.b_h2l_status == MS_TRUE)
	{
		st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 2) % 3;
		st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
		api_dma_comom_buf_id_set(&st_dma_dev);
	}
	
	st_dma_dev.u8_sel = g_u8_dma_idx[2];
	api_dma_common_fifo_status_get(&st_dma_dev);
	if(st_dma_dev.st_dma_status.b_h2l_status == MS_TRUE)
	{
		st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 2) % 3;
		st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
		api_dma_comom_buf_id_set(&st_dma_dev);
	}
	
	st_dma_dev.u8_sel = g_u8_dma_idx[3];
	api_dma_common_fifo_status_get(&st_dma_dev);
	if(st_dma_dev.st_dma_status.b_h2l_status == MS_TRUE)
	{
		st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = (u8_wdma_buf_id + 2) % 3;
		st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_wdma_buf_id;   
		api_dma_comom_buf_id_set(&st_dma_dev);
	}
	
	g_u16_timer_out = 0;
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	do
	{
		g_u16_timer_out ++;        
	}
	while(!api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB) && (g_u16_timer_out < 500));  
	api_dma_common_simultaneous_trigger();
	
	gst_splitter_vipss_dev[VIPSS1].st_splitter_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vipss_dev[VIPSS1].st_splitter_com_wdma_dev); 
	gst_splitter_vipss_dev[VIPSS2].st_splitter_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vipss_dev[VIPSS2].st_splitter_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vipss_dev[VIPSS2].st_splitter_com_wdma_dev);   
	gst_splitter_vipss_dev[VIPSS3].st_splitter_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vipss_dev[VIPSS3].st_splitter_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vipss_dev[VIPSS3].st_splitter_com_wdma_dev);   
	gst_splitter_vipss_dev[VIPSS4].st_splitter_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_splitter_vipss_dev[VIPSS4].st_splitter_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	api_dma_comom_buf_id_mode_cfg(&gst_splitter_vipss_dev[VIPSS4].st_splitter_com_wdma_dev);
	
	g_u16_timer_out = 0;
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	do
	{
		g_u16_timer_out ++;
		b_int = api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB);           
	}
	while((b_int != MS_TRUE) && (g_u16_timer_out < 500)); 
	
	api_dma_common_simultaneous_trigger();
	__enable_irq();   
}

VOID splitter_video_mute(UINT8 u8_path, BOOL b_mute)
{
	BOOL b_int;
	if(g_st_app_splitter_dev.u8_splitter_mode < MODE_DUAL_UP1_DOWN2)
	{
		if(u8_path == g_st_app_splitter_dev.u8_splitter_mode)
		{
			u8_path = 0;
		}
		else
		{
			return;
		}
	}
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.u8_sel = g_u8_su_idx[u8_path];
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.bd_enable = b_mute;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.border_color.u8VR = 0x80;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.border_color.u8YG = 0x10;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.border_color.u8UB = 0x80;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.bd_cover = MS_TRUE;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.border_size.u16_border_h = gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_size.st_OutRect.u16_h;
	gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_border.border_size.u16_border_v = gst_splitter_vopss_dev[u8_path].st_splitter_su_dev.st_video_size.st_OutRect.u16_v;
	api_su_video_border_set(&gst_splitter_vopss_dev[u8_path].st_splitter_su_dev);    
	g_u16_timer_out = 0;
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	do
	{
		g_u16_timer_out ++;
		b_int = api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB);           
	}
	while((b_int != MS_TRUE) && (g_u16_timer_out < 500)); 
	api_su_trigger(&gst_splitter_vopss_dev[u8_path].st_splitter_su_dev); 
}

//video mixer setup
static VOID _app_video_splitter_mixer_cfg(UINT8 u8_vmixer_idx)
{
	MISC_MODULE_CLK_T g_st_module_clk;
	
	g_st_module_clk.u8_dst_module = MISC_VIDEO_MODULE_MIX1;
	g_st_module_clk.u8_clkdiv = MISC_CLKDIV_1;
	g_st_module_clk.u8_src_clk = MISC_CLKSRC_VDSTMG1;
	api_misc_mixer_clkin_sel(&g_st_module_clk);
	
	g_st_video_mixer_dev[u8_vmixer_idx].u8_video_mixer_idx = u8_vmixer_idx;
	api_video_mixer_instance_init(&g_st_video_mixer_dev[u8_vmixer_idx], u8_vmixer_idx);
	
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel = &g_st_mixer_layer_sel;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf     = &g_st_mixer_alf_cfg;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bd_attr   = NULL;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bd_color  = NULL;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bg_color  = &g_st_bg_color;
	
	g_st_video_mixer_dev[u8_vmixer_idx].u8_video_mixer_idx = u8_vmixer_idx;
	g_st_video_mixer_dev[u8_vmixer_idx].b_videomixer_en = MS_TRUE;
	api_video_mixer_bg_color_config(&g_st_video_mixer_dev[u8_vmixer_idx]);
	api_video_mixer_enable(&g_st_video_mixer_dev[u8_vmixer_idx]);
	
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer0_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer1_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer2_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer3_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
	
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS1_DSC_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS1_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS2_DSC_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS2_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS3_DSC_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS3_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS4_DSC_SU_alf_ratio = 0xFF;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf->u8_VOPSS4_SU_alf_ratio = 0xFF;
	api_video_mixer_layer_config(&g_st_video_mixer_dev[u8_vmixer_idx]);
}

VOID app_splitter_mode_config(SPLITTER_APP_T *pst_app_splitter_dev)
{
	UINT8 u8_i = 0;
	for(u8_i = 0; u8_i < 4; u8_i ++)
	{
		if(g_u8_HD_TX_PROT[pst_app_splitter_dev->u8_dst_sel] == g_u8_HD_TX_PROT[u8_i])
		{
			Bypass_Tx_Status[u8_i] = MS_FALSE;
		}
		else
			Bypass_Tx_Status[u8_i] = MS_TRUE;
	}
	Vipss_Mapping_Rx[0] = g_u8_HD_RX_PROT[0];
	Vipss_Mapping_Rx[1] = g_u8_HD_RX_PROT[1];
	Vipss_Mapping_Rx[2] = g_u8_HD_RX_PROT[2];
	Vipss_Mapping_Rx[3] = g_u8_HD_RX_PROT[3];
	gst_splitter_vipss_dev[0].st_splitter_vwdma_dev.b_enable = FALSE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[0].st_splitter_vwdma_dev);
	gst_splitter_vipss_dev[1].st_splitter_vwdma_dev.b_enable = FALSE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[1].st_splitter_vwdma_dev);
	gst_splitter_vipss_dev[2].st_splitter_vwdma_dev.b_enable = FALSE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[2].st_splitter_vwdma_dev);
	gst_splitter_vipss_dev[3].st_splitter_vwdma_dev.b_enable = FALSE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[3].st_splitter_vwdma_dev);
	
	gst_splitter_vopss_dev[0].st_splitter_vrdma_dev.b_enable = FALSE;
	api_vrdma_enable_set(&gst_splitter_vopss_dev[0].st_splitter_vrdma_dev);
	gst_splitter_vopss_dev[1].st_splitter_vrdma_dev.b_enable = FALSE;
	api_vrdma_enable_set(&gst_splitter_vopss_dev[1].st_splitter_vrdma_dev);
	gst_splitter_vopss_dev[2].st_splitter_vrdma_dev.b_enable = FALSE;
	api_vrdma_enable_set(&gst_splitter_vopss_dev[2].st_splitter_vrdma_dev);
	gst_splitter_vopss_dev[3].st_splitter_vrdma_dev.b_enable = FALSE;
	api_vrdma_enable_set(&gst_splitter_vopss_dev[3].st_splitter_vrdma_dev);
	app_video_vipss_trig(CHANNEL1_without_dsc);
	app_video_vipss_trig(CHANNEL2_without_dsc);
	app_video_vipss_trig(CHANNEL3_without_dsc);
	app_video_vipss_trig(CHANNEL4_without_dsc);
	if(pst_app_splitter_dev->b_splitter_sw == MS_TRUE)
	{
		_splitter_video_freeze(VIPSS1);
		_splitter_video_freeze(VIPSS2);
		_splitter_video_freeze(VIPSS3);
		_splitter_video_freeze(VIPSS4);
		mculib_delay_ms(50); 
	}
	else
	{
		//vipss1 instance init
		gst_splitter_vipss_dev[VIPSS1].u8_vipss_sel = VIPSS1;
		app_splitter_vipss_instance_init(&gst_splitter_vipss_dev[VIPSS1]);  

		//vopss1 instance init
		gst_splitter_vopss_dev[VOPSS1].u8_vopss_sel = VOPSS1;
		app_splitter_vopss_instance_init(&gst_splitter_vopss_dev[VOPSS1]);  
		
		//vipss2 instance init
		gst_splitter_vipss_dev[VIPSS2].u8_vipss_sel = VIPSS2;
		app_splitter_vipss_instance_init(&gst_splitter_vipss_dev[VIPSS2]);  
		
		//vopss2 instance init
		gst_splitter_vopss_dev[VOPSS2].u8_vopss_sel = VOPSS2;
		app_splitter_vopss_instance_init(&gst_splitter_vopss_dev[VOPSS2]);
		
		//vipss3 instance init
		gst_splitter_vipss_dev[VIPSS3].u8_vipss_sel = VIPSS3;
		app_splitter_vipss_instance_init(&gst_splitter_vipss_dev[VIPSS3]);  

		//vopss3 instance init
		gst_splitter_vopss_dev[VOPSS3].u8_vopss_sel = VOPSS3;
		app_splitter_vopss_instance_init(&gst_splitter_vopss_dev[VOPSS3]);  
		
		//vipss4 instance init
		gst_splitter_vipss_dev[VIPSS4].u8_vipss_sel = VIPSS4;
		app_splitter_vipss_instance_init(&gst_splitter_vipss_dev[VIPSS4]);  
		
		//vopss4 instance init
		gst_splitter_vopss_dev[VOPSS4].u8_vopss_sel = VOPSS4;
		app_splitter_vopss_instance_init(&gst_splitter_vopss_dev[VOPSS4]);
		
		//FRC
		_splitter_mode_frc_cfg();
	
		g_st_tx_mux[pst_app_splitter_dev->u8_dst_sel].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[pst_app_splitter_dev->u8_dst_sel]];
		g_st_tx_mux[pst_app_splitter_dev->u8_dst_sel].u8_video_mux = MISC_VIDEO_MODULE_MIX1;
		api_misc_tx_mux_cfg(&g_st_tx_mux[pst_app_splitter_dev->u8_dst_sel]);
		
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].b_is_hd = TRUE;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].b_full_edid = 0;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_color_depth = HD_COLOR_DEPTH_24;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_audio_mode = HD_AUDIO_STREAM_TYPE_MAS;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_audio_src = FROM_HD_RX_AUDIO;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_video_clk_src = FROM_RX_OR_PLL_PIXEL_CLK;
		g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u32_tmds_clk = g_stVideo_HDTx_Timing.u16_pixclk; 
		memcpy((&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].st_infoframes_packets), &gst_inf_pack, sizeof(HD_INFOFRAMES_PACKETS_T));		
		api_hd_tx_output_en(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel],MS_FALSE);
		api_hd_tx_config_output(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel]); 

		api_hd_tx_output_en(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel],MS_TRUE);
		_app_video_splitter_mixer_cfg(VIDEO_MIXER_1);
		
		//vopss tx csc para
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].u8_sel = g_u8_csc_idx[g_u8_HD_TX_PROT[pst_app_splitter_dev->u8_dst_sel]];
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].st_csc_config_param.csc_out_colorspace = CHIP_TX_CSC_OUT_COLORSPACE;
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].st_csc_config_param.csc_out_color_range = CHIP_TX_CSC_OUT_COLORRANGE;
		g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel].st_csc_config_param.u8_pixel_repeat = HD_PIXEL_REPEAT_X1;
		api_csc_tx_set(&g_st_txcsc_dev[pst_app_splitter_dev->u8_dst_sel]);
	}
	switch(pst_app_splitter_dev->u8_splitter_mode)
	{
		case MODE_SIGNLE_1:  
		case MODE_SIGNLE_2:  
		case MODE_SIGNLE_3:  
		case MODE_SIGNLE_4:  
			Vipss_Mapping_Rx[0] = g_u8_HD_RX_PROT[pst_app_splitter_dev->u8_splitter_mode];
			app_splitter_vipss_rx_mux_cfg(0, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[pst_app_splitter_dev->u8_splitter_mode].u16_vactive;   
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = 
				return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_4K_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v =
				return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_4K_V_SIZE_MAX);
		
			 
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_4K_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_4K_SIZE;
			app_splitter_vipss_cfg(0, pst_app_splitter_dev);             
			app_video_vipss_trig(g_u8_ss_ch_idx[0]);
			mculib_delay_ms(50); 
			app_splitter_vopss_cfg(0, pst_app_splitter_dev);
			   
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = pst_app_splitter_dev->u8_splitter_mode;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel);           
			LOG2("[INFO] SPLITTER Single Mode",pst_app_splitter_dev->u8_splitter_mode+1);
			break;
		case MODE_DUAL_UP1_DOWN2:  
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;    
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			 
			
			//window size           
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
					
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
						
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);
		
			app_video_vipss_trig(g_u8_ss_ch_idx[g_u8_rx_idx[0]]);
			app_video_vipss_trig(g_u8_ss_ch_idx[g_u8_rx_idx[1]]);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_UP1_DOWN2");
			break;
		case MODE_DUAL_UP1_DOWN2_4_3:    
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 			
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive + g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;       
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);

			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive + g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st =  g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
						
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);     
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_UP1_DOWN2_4_3");
			break;
		case MODE_DUAL_UP3_DOWN4:          
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);                 
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 

			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_UP3_DOWN4");
			break;
		case MODE_DUAL_UP3_DOWN4_4_3:  
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive + g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st =  g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + (g_stVideo_HDTx_Timing.u16_hactive + g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);             
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_UP3_DOWN4_4_3");
			break;
		case MODE_DUAL_LEFT1_RIGHT2:        
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);
						
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_UP3_DOWN4_4_3");
			break;
		case MODE_DUAL_LEFT1_RIGHT2_4_3:          
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive + g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3;
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive + g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev); 
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_LEFT1_RIGHT2_4_3");
			break;
		case MODE_DUAL_LEFT3_RIGHT4:            
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
										
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_LEFT1_RIGHT2_4_3");
			break;
		case MODE_DUAL_LEFT3_RIGHT4_4_3:           
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st =  g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive + g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + (g_stVideo_HDTx_Timing.u16_vactive + g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);          
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);  
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_DUAL_LEFT3_RIGHT4_4_3");
			break;
		case MODE_PIP_12_1_3:   
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);
					
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			mculib_delay_ms(50);  
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_PIP_12_1_3");
			break;
		case MODE_PIP_21_1_3:   
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);
					
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			mculib_delay_ms(50);  
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX2;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_PIP_21_1_3");
			break;
		case MODE_PIP_34_1_3:     
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
										
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_PIP_34_1_3");
			break;
		case MODE_PIP_43_1_3:     
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
										
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[3];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX4;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_PIP_43_1_3");
			break;
		case MODE_THREE_EQUALIZE:
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
				
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_THREE_EQUALIZE");
			break;
		case MODE_FOUR_MAIN1_SUB234:          
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
		
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);   
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);     
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_MAIN1_SUB234");
			break;
		case MODE_FOUR_MAIN2_SUB134:          
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
					
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
						
			
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);  
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX2;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_MAIN2_SUB134");
			break;
		case MODE_FOUR_MAIN3_SUB124:         
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
		
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
						
			
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);  
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX3;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_MAIN3_SUB124");
			break;
		case MODE_FOUR_MAIN4_SUB123:         
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
		
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

		
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
		
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
						
				
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX4;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_MAIN4_SUB123");
			break;
		case MODE_FOUR_H_SUB1234:            
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
				
			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
						
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
						
				
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_H_SUB1234");
			break;
		case MODE_FOUR_EQUALIZE:              
			app_splitter_vipss_rx_mux_cfg(VIPSS1, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS2, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS3, pst_app_splitter_dev); 
			app_splitter_vipss_rx_mux_cfg(VIPSS4, pst_app_splitter_dev); 
			//scale size
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[0].u16_hactive;
			pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[0].u16_vactive;  
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;     
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[0].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[0].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[0].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			 
			
			//window size
			pst_app_splitter_dev->st_video_win[0].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[0].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[0].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[0].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
		
			
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h = g_stVideo_HDRx_Timing[1].u16_hactive;
			pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v = g_stVideo_HDRx_Timing[1].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[1].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[1].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[1].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[1].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[1].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[1].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
			pst_app_splitter_dev->st_video_win[1].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
		
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[0].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

		
			//scale size
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h = g_stVideo_HDRx_Timing[2].u16_hactive;
			pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v = g_stVideo_HDRx_Timing[2].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[2].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[2].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[2].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			//window size
			pst_app_splitter_dev->st_video_win[2].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
			pst_app_splitter_dev->st_video_win[2].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[2].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			//addr config
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[1].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;

			
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h = g_stVideo_HDRx_Timing[3].u16_hactive;
			pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v = g_stVideo_HDRx_Timing[3].u16_vactive;      
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_h = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_h, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			pst_app_splitter_dev->st_scale_size[3].tMemSize.u16_v = return_minof_three(pst_app_splitter_dev->st_scale_size[3].tInSize.u16_v, pst_app_splitter_dev->st_scale_size[3].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
		
			
			pst_app_splitter_dev->st_video_win[3].u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
			pst_app_splitter_dev->st_video_win[3].u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive / 2;
			pst_app_splitter_dev->st_video_win[3].u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 = pst_app_splitter_dev->st_dma_addr[2].u32_dma_start_addr_2 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_1080P_SIZE;
			pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_2 = pst_app_splitter_dev->st_dma_addr[3].u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_1080P_SIZE;
										 
			app_splitter_vipss_cfg(VIPSS1, pst_app_splitter_dev);            
			app_splitter_vipss_cfg(VIPSS2, pst_app_splitter_dev);       
			app_splitter_vipss_cfg(VIPSS3, pst_app_splitter_dev);     
			app_splitter_vipss_cfg(VIPSS4, pst_app_splitter_dev);
				
			app_video_vipss_trig(CHANNEL1_without_dsc);
			app_video_vipss_trig(CHANNEL2_without_dsc);
			app_video_vipss_trig(CHANNEL3_without_dsc);
			app_video_vipss_trig(CHANNEL4_without_dsc);
			mculib_delay_ms(50);
			app_splitter_vopss_cfg(VOPSS1, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS2, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS3, pst_app_splitter_dev);
			app_splitter_vopss_cfg(VOPSS4, pst_app_splitter_dev);
			 
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer7_src_sel = g_u8_mixer_layer_src[0];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer6_src_sel = g_u8_mixer_layer_src[1];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer5_src_sel = g_u8_mixer_layer_src[2];
			g_st_video_mixer_dev[VIDEO_MIXER_1].pst_layer_sel->u8_layer4_src_sel = g_u8_mixer_layer_src[3];
			_splitter_video_unfreeze(); 
			//audio cfg
			g_u8_splitter_audio_src = AUDIO_HD_RX1;
			app_tx_audio_cfg(g_u8_splitter_audio_src, pst_app_splitter_dev->u8_dst_sel); 
			LOG("[INFO] SPLITTER MODE_FOUR_EQUALIZE");
			break;
	}
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x01);
}


VOID app_splitter_process(UINT8 u8_rx_idx)
{
	switch(g_st_app_splitter_dev.u8_splitter_mode)
	{
		case MODE_SIGNLE_1:
		case MODE_SIGNLE_2:
		case MODE_SIGNLE_3:
		case MODE_SIGNLE_4:
			//scale size
			if(u8_rx_idx == g_st_app_splitter_dev.u8_splitter_mode)               
			{
				app_splitter_vipss_rx_mux_cfg(VIPSS1, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[0].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[0].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[0].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[0].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[0].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[0].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[0].tOutSize.u16_h, MEMORY_4K_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[0].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[0].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[0].tOutSize.u16_v, MEMORY_4K_V_SIZE_MAX);
				u8_rx_idx = 0;
			}
			else
			{
								
				return;
			}
			break;
		case MODE_DUAL_UP1_DOWN2:  
			if(u8_rx_idx == 0 || u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_UP3_DOWN4:
			if(u8_rx_idx == 2 || u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_UP1_DOWN2_4_3:
			if(u8_rx_idx == 0 || u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_UP3_DOWN4_4_3: 
			if(u8_rx_idx == 2 || u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_vactive / 2 * 4 / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_LEFT1_RIGHT2:
			if(u8_rx_idx == 0 || u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_LEFT3_RIGHT4: 
			if(u8_rx_idx == 2 || u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_LEFT1_RIGHT2_4_3:    
			if(u8_rx_idx == 0 || u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_DUAL_LEFT3_RIGHT4_4_3:  
			if(u8_rx_idx == 2 || u8_rx_idx == 3)
			{     
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 				
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;    
		case MODE_PIP_12_1_3:  
			if(u8_rx_idx == 0)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else if(u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_PIP_34_1_3:
			if(u8_rx_idx == 2)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else if(u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_PIP_21_1_3:  
			if(u8_rx_idx == 1)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else if(u8_rx_idx == 0)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_PIP_43_1_3:
			if(u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else if(u8_rx_idx == 2)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				//scale size
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3; 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				return;
			}
			break;
		case MODE_THREE_EQUALIZE:
			if(u8_rx_idx == 0 || u8_rx_idx == 1 || u8_rx_idx == 2)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			break;
		case MODE_FOUR_MAIN1_SUB234: 
		case MODE_FOUR_MAIN2_SUB134:       
		case MODE_FOUR_MAIN3_SUB124:       
		case MODE_FOUR_MAIN4_SUB123:        
			if(u8_rx_idx == g_st_app_splitter_dev.u8_splitter_mode - MODE_FOUR_MAIN1_SUB234)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}     
			break;
		case MODE_FOUR_H_SUB1234:       
			if(u8_rx_idx == 0 || u8_rx_idx == 3)
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			else
			{
				app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 3;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
				g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			}
			break;
		case MODE_FOUR_EQUALIZE: 
			app_splitter_vipss_rx_mux_cfg(u8_rx_idx, &g_st_app_splitter_dev); 
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h = g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive;
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v = g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive;      
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h = g_stVideo_HDTx_Timing.u16_hactive / 2;
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v = g_stVideo_HDTx_Timing.u16_vactive / 2;
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_h, MEMORY_1080P_H_SIZE_MAX);
			g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v = return_minof_three(g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive, g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tOutSize.u16_v, MEMORY_1080P_V_SIZE_MAX);
			break;
	}

	//vipss sdn config     
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_sdn_dev.st_video_size.st_InRect.u16_h = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_h;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_sdn_dev.st_video_size.st_InRect.u16_v = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tInSize.u16_v;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v;

	api_sdn_scaler_size_set(&gst_splitter_vipss_dev[u8_rx_idx].st_splitter_sdn_dev);
	//vipss vwdma config
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.st_vwdma_in_size.u16_h_size = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.st_vwdma_in_size.u16_v_size = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.u16_line_offset = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_0;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_1;
	gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_2;
	api_vwdma_start_addr_cfg(&gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev);
	api_vwdma_line_offset_cfg(&gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev);
	api_vwdma_burst_num_cfg(&gst_splitter_vipss_dev[u8_rx_idx].st_splitter_vwdma_dev);
 

	 //vopss VRDMA config
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.b_enable = MS_TRUE; 
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.u16_line_offset = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.st_vrdma_out_size.u16_h_size = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.st_vrdma_out_size.u16_v_size = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_0 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_0;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_1 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_1;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_2 = g_st_app_splitter_dev.st_dma_addr[u8_rx_idx].u32_dma_start_addr_2;
	
	api_vrdma_enable_set(&gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev);
	api_vrdma_start_addr_cfg(&gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev);
	api_vrdma_burst_num_cfg(&gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev);
	api_vrdma_line_offset_cfg(&gst_splitter_vopss_dev[u8_rx_idx].st_splitter_vrdma_dev);

	
	//vopss su config
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_su_dev.st_video_size.st_MemRect.u16_h = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_h;
	gst_splitter_vopss_dev[u8_rx_idx].st_splitter_su_dev.st_video_size.st_MemRect.u16_v = g_st_app_splitter_dev.st_scale_size[u8_rx_idx].tMemSize.u16_v;
	api_su_scaler_size_set(&gst_splitter_vopss_dev[u8_rx_idx].st_splitter_su_dev);  
	
	app_video_vipss_trig(g_u8_ss_ch_idx[u8_rx_idx]);
	app_video_vopss_trig(g_u8_ss_ch_idx[u8_rx_idx]);
	mculib_delay_ms(200);
}

VOID app_splitter_rx_rotate(UINT8 u8_rx_idx, UINT8 u8_rotate)
{
	UINT16 u16_h_size;
	UINT16 u16_v_size;
	//rotate input clock must be lower than 148.5M
	if(g_stVideo_HDRx_Timing[u8_rx_idx].u16_pixclk > 14900)
			return;
	
	_splitter_video_freeze(VIPSS1);
	//vipss sdn config     
	gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.u8_sel = g_u8_sdn_idx[VIPSS1];
	gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.b_enable = MS_TRUE;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.u8_sdn_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_h = 
	g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive  > 0 ? g_stVideo_HDRx_Timing[u8_rx_idx].u16_hactive : 1920;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_v = 
	g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive  > 0 ? g_stVideo_HDRx_Timing[u8_rx_idx].u16_vactive : 1080;
	if(u8_rotate % 2 > 0)
	{
		gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h = 
				return_minof_three(gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_h, g_stVideo_HDTx_Timing.u16_vactive, 1080);
		gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v = 
				return_minof_three(gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_v, g_stVideo_HDTx_Timing.u16_hactive, 1080);
	}
	else
	{
		gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h = 
				return_minof_three(gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_h, g_stVideo_HDTx_Timing.u16_hactive, 1920);
		gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v = 
				return_minof_three(gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_InRect.u16_v, g_stVideo_HDTx_Timing.u16_vactive, 1080);  
	}   

	api_sdn_scaler_size_set(&gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev);

	//vipss vwdma config
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.u8_sel = g_u8_dma_idx[VIPSS1];
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.b_enable = MS_TRUE;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_in_size.u16_h_size = 
			gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_in_size.u16_v_size = 
			gst_splitter_vipss_dev[VIPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.u8_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.u16_line_offset = 1920;
	
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 = 0;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 = gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 + SINGLE_BUF_MEMORY_4K_SIZE;
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2 = gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 + SINGLE_BUF_MEMORY_4K_SIZE;
	api_vwdma_enable_set(&gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev);
	
	gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev.u8_vwdma_rotatation_deg = (u8_rotate == 2) ? 0: u8_rotate;
	api_vwdma_rotation_cfg(&gst_splitter_vipss_dev[VIPSS1].st_splitter_vwdma_dev);
	
	app_video_vipss_trig(g_u8_ss_ch_idx[VIPSS1]);
	
	 //vopss VRDMA config
	u16_h_size = gst_splitter_vipss_dev[VOPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_h;
	u16_v_size = gst_splitter_vipss_dev[VOPSS1].st_splitter_sdn_dev.st_video_size.st_MemRect.u16_v;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.u8_sel = g_u8_dma_idx[VOPSS1];
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.b_enable = MS_TRUE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.u8_mem_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_out_size.u16_h_size = (u8_rotate % 2 > 0) ? u16_v_size : u16_h_size;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_out_size.u16_v_size = (u8_rotate % 2 > 0) ? u16_h_size : u16_v_size;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.u16_line_offset = 1920;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_0 =
		gst_splitter_vipss_dev[VOPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_1 = 
		gst_splitter_vipss_dev[VOPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_2 = 
		gst_splitter_vipss_dev[VOPSS1].st_splitter_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2;
			
	gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.u8_mirror_cfg = u8_rotate == 2 ? DMA_H_V_MIRROR_ALL_ENALBE : DMA_H_V_MIRROR_ALL_DISABLE;           
	api_vrdma_mirror_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev);
	api_vrdma_start_addr_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev);
	api_vrdma_burst_num_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev);
	api_vrdma_line_offset_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev);
	api_vrdma_mirror_cfg(&gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev);
			
	//vopss su config
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.u8_sel = g_u8_su_idx[VOPSS1];
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.b_enable = MS_TRUE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.u8_su_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_size.st_MemRect.u16_h = 
		gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_out_size.u16_h_size;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_size.st_MemRect.u16_v = 
			 gst_splitter_vopss_dev[VOPSS1].st_splitter_vrdma_dev.st_vrdma_out_size.u16_v_size;
	
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_win.u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_win.u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_win.u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset ;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_video_win.u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_dis_win.u8_dis_win_en = MS_TRUE;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_dis_win.u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_dis_win.u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_dis_win.u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
	gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev.st_dis_win.u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;

	api_su_enable_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev);
	api_su_scaler_size_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev);
	api_su_csc_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev);
	api_su_video_win_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev);
	api_su_display_win_set(&gst_splitter_vopss_dev[VOPSS1].st_splitter_su_dev);
	
	_splitter_video_unfreeze();
		
}

VOID app_splitter_mode_audio_src(VOID)
{
    switch (g_st_app_splitter_dev.u8_splitter_mode)
    {
    case MODE_SIGNLE_1:
        if(g_u8_splitter_audio_src == 0)
			g_u8_splitter_audio_src = 4;
		else
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_SIGNLE_2:
        if(g_u8_splitter_audio_src == 1)
			g_u8_splitter_audio_src = 4;
		else
			g_u8_splitter_audio_src = 1;
        break;
    case MODE_SIGNLE_3:
        if(g_u8_splitter_audio_src == 2)
			g_u8_splitter_audio_src = 4;
		else
			g_u8_splitter_audio_src = 2;
        break;
    case MODE_SIGNLE_4:
        if(g_u8_splitter_audio_src == 3)
			g_u8_splitter_audio_src = 4;
		else
			g_u8_splitter_audio_src = 3;
        break;
	case MODE_THREE_EQUALIZE:
		g_u8_splitter_audio_src ++;
		if(g_u8_splitter_audio_src == 3)
			g_u8_splitter_audio_src = 4;
		if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_FOUR_MAIN1_SUB234:
		g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_FOUR_MAIN2_SUB134:
        g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_FOUR_MAIN3_SUB124:
        g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_FOUR_MAIN4_SUB123:
        g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_FOUR_EQUALIZE:
	case MODE_FOUR_H_SUB1234:
        g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_DUAL_LEFT1_RIGHT2:
    case MODE_DUAL_LEFT1_RIGHT2_4_3:
	case MODE_DUAL_UP1_DOWN2:
	case MODE_DUAL_UP1_DOWN2_4_3:
	case MODE_PIP_12_1_3:
	case MODE_PIP_21_1_3:
        g_u8_splitter_audio_src ++;
		if(g_u8_splitter_audio_src == 2)
			g_u8_splitter_audio_src = 4;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 0;
        break;
    case MODE_DUAL_LEFT3_RIGHT4:
    case MODE_DUAL_LEFT3_RIGHT4_4_3:
	case MODE_DUAL_UP3_DOWN4:
	case MODE_DUAL_UP3_DOWN4_4_3:
	case MODE_PIP_34_1_3:
	case MODE_PIP_43_1_3:
        g_u8_splitter_audio_src ++;
        if(g_u8_splitter_audio_src == 5)
			g_u8_splitter_audio_src = 2;
        break;
    }
	if(g_u8_splitter_audio_src < 4)
	{
		b_audio_mute = MS_FALSE;
	}
	else
	{
		b_audio_mute = MS_TRUE;
	}
	app_tx_audio_cfg(g_u8_splitter_audio_src, g_st_app_splitter_dev.u8_dst_sel);
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}

VOID app_splitter_output_resolution(UINT8 u8_vic, SPLITTER_APP_T *pst_app_splitter_dev)
{
	PLL_DEV_T         st_pll_dev;
	MISC_MODULE_CLK_T st_module_clk;
	VDS_TMG_DEV_T     st_timgen_dev;  
	api_hd_tx_output_en(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel], MS_FALSE);
	api_misc_timing_get(u8_vic, &g_stVideo_HDTx_Timing);
	st_timgen_dev.pst_timing = &g_stVideo_HDTx_Timing;
	st_timgen_dev.u8_timgen_id = TIMGEN_1;
	st_timgen_dev.u8_workmode  = FreeRun_Mode;            

	st_pll_dev.b_osc_input = MS_TRUE;
	st_pll_dev.u8_pll_idx = PLLV1;
	st_pll_dev.u32_clk_out = st_timgen_dev.pst_timing->u16_pixclk * 10000;

	st_module_clk.u8_dst_module = MISC_VIDEO_MODULE_TIMGEN1;
	st_module_clk.u8_src_clk = MISC_CLKSRC_PLLV1;
	st_module_clk.u8_clkdiv = MISC_CLKDIV_1;
		 
	api_pll_config(&st_pll_dev);
	api_misc_timgen_clkin_sel(&st_module_clk);
	api_vds_tmg_config(&st_timgen_dev);
	app_splitter_mode_config(pst_app_splitter_dev);
	
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_txIndex = g_u8_HD_TX_PROT[pst_app_splitter_dev->u8_dst_sel];
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].b_is_hd = TRUE;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].b_full_edid = 0;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_color_depth = HD_COLOR_DEPTH_24;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_audio_mode = HD_AUDIO_STREAM_TYPE_MAS;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_audio_src = FROM_HD_RX_AUDIO;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u8_video_clk_src = FROM_RX_OR_PLL_PIXEL_CLK;
	g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].u32_tmds_clk = g_stVideo_HDTx_Timing.u16_pixclk;
	memcpy((&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel].st_infoframes_packets), &gst_inf_pack, sizeof(HD_INFOFRAMES_PACKETS_T));  
	api_hd_tx_output_en(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel],MS_FALSE);
	api_hd_tx_config_output(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel]);

	api_hd_tx_output_en(&g_st_hd_tx[pst_app_splitter_dev->u8_dst_sel], MS_TRUE);
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}
VOID app_splitter_osd_init(VOID)
{
	UINT8 u8_idx = 0;
	for(u8_idx = 0; u8_idx < 4; u8_idx ++)
	{
		g_st_misc_osd_mux[u8_idx].b_enable = MS_TRUE;
		g_st_misc_osd_mux[u8_idx].u8_osd_module = g_u8_misc_osd_idx[u8_idx];
		g_st_misc_osd_mux[u8_idx].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[g_st_app_splitter_dev.u8_dst_sel]];
		api_misc_osd_timing_mux_cfg(&g_st_misc_osd_mux[u8_idx]);
	}
}

VOID app_splitter_osd_config(VOID)
{
	st_str.u8_x = 0;
	st_str.u8_y = 0;
	st_font.u8_x = 0;
	st_font.u8_y = 0;
	st_osd_win_size.u8_osd_win = OSD_WIN_DISPLAY;
	st_osd_win_size.u16_w = 15;
	st_osd_win_size.u16_h = 4;
	app_splitter_osd_init();
	switch(g_st_app_splitter_dev.u8_splitter_mode)
	{
		case MODE_SIGNLE_1:
		case MODE_SIGNLE_2:
		case MODE_SIGNLE_3:
		case MODE_SIGNLE_4:
			st_osd_win_size.u16_x = 100;
			st_osd_win_size.u16_y = 100;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != g_st_app_splitter_dev.u8_splitter_mode)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[g_st_app_splitter_dev.u8_splitter_mode];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[g_st_app_splitter_dev.u8_splitter_mode];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);	
			
			st_str.u8_y = 1;
			set_osd_res(g_st_app_splitter_dev.u8_splitter_mode);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
			break;
		case MODE_DUAL_UP1_DOWN2:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);	
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_UP1_DOWN2_4_3:
			st_osd_win_size.u16_x = (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_UP3_DOWN4:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_UP3_DOWN4_4_3:
			st_osd_win_size.u16_x = (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);	
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = (g_stVideo_HDTx_Timing.u16_hactive - g_stVideo_HDTx_Timing.u16_vactive / 2 / 3 * 4) / 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_LEFT1_RIGHT2:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_LEFT1_RIGHT2_4_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_LEFT3_RIGHT4:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_DUAL_LEFT3_RIGHT4_4_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = (g_stVideo_HDTx_Timing.u16_vactive - g_stVideo_HDTx_Timing.u16_hactive / 2 / 4 * 3) / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_PIP_12_1_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_PIP_21_1_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_PIP_34_1_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_PIP_43_1_3:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			break;
		case MODE_THREE_EQUALIZE:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			break;
		case MODE_FOUR_MAIN1_SUB234:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
		case MODE_FOUR_MAIN2_SUB134:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
		case MODE_FOUR_MAIN3_SUB124:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
		
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
		case MODE_FOUR_MAIN4_SUB123:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 3 * 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
		case MODE_FOUR_H_SUB1234:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 3 * 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
		case MODE_FOUR_EQUALIZE:
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 0)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[0];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[0];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[0], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(0);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[0], &st_str);	
			app_osd_trigger(0);
		
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = 50;
			api_osd_window_position_set(&g_st_osd_dev[1], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 1)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[1];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[1];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[1], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(1);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[1], &st_str);	
			app_osd_trigger(1);
			
			st_osd_win_size.u16_x = 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[2], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 2)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[2];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[2];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[2], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(2);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[2], &st_str);	
			app_osd_trigger(2);
			
			st_osd_win_size.u16_x = g_stVideo_HDTx_Timing.u16_hactive / 2 + 50;
			st_osd_win_size.u16_y = g_stVideo_HDTx_Timing.u16_vactive / 2 + 50;
			api_osd_window_position_set(&g_st_osd_dev[3], &st_osd_win_size);
			
			if(g_u8_splitter_audio_src != 3)
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel[3];
			}
			else
			{
				st_font.pu16_idx = (UINT16 *)str_osd_input_channel_audio[3];
			}
			st_font.u16_length = 8;
			api_osd_show_index(&g_st_osd_dev[3], &st_font);
			
			st_str.u8_y = 1;
			set_osd_res(3);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[3], &st_str);	
			app_osd_trigger(3);
			break;
	}
}
#endif
