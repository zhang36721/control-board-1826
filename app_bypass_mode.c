#ifndef _APP_BYPASS_MODE_C_
#define _APP_BYPASS_MODE_C_

#include "videoprocess.h"

VOID video_bypass_tx_cfg(UINT8 u8_index)
{
	UINT8 u8_tx_id = 0;
	for (u8_tx_id = 0; u8_tx_id < 4; u8_tx_id ++)
	{
		if(Bypass_Tx_Status[u8_tx_id] == 1 && Matrix_Tx_Mapping[u8_tx_id] == u8_index)
		{
			g_st_hd_tx[u8_tx_id].u8_txIndex = g_u8_HD_TX_PROT[u8_tx_id];
			g_st_hd_tx[u8_tx_id].u8_color_depth = g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].u8_color_depth;//HD_COLOR_DEPTH_24;
			g_st_hd_tx[u8_tx_id].u8_audio_src = FROM_HD_RX_AUDIO;
			g_st_hd_tx[u8_tx_id].u8_video_clk_src = FROM_RX_TMDS_CLK;
			g_st_hd_tx[u8_tx_id].u32_tmds_clk = g_stVideo_HDRx_Timing[Matrix_Tx_Mapping[u8_tx_id]].u16_pixclk;
			memcpy((&g_st_hd_tx[u8_tx_id].st_infoframes_packets), &g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes, sizeof(HD_INFOFRAMES_PACKETS_T));
																		
			g_st_txcsc_dev[u8_tx_id].st_csc_config_param.u8_pixel_repeat = HD_PIXEL_REPEAT_X1;
			g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_colorspace = g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].u8_color_space;//st_packets_infoframes.st_avi_infoframe.e_colorspace;
						
			if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes.st_avi_infoframe.e_colorspace == HD_COLORSPACE_RGB)
			{
				if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes.st_avi_infoframe.e_rgb_quantization_range < HD_QUANTIZATION_RANGE_FULL)
				{
					g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range = CSC_RGB_16_235;
				} 
				else
				{
					g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range = CSC_RGB_0_255;
				}
				
			}
			else if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes.st_avi_infoframe.e_colorspace > HD_COLORSPACE_RGB &&
					g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes.st_avi_infoframe.e_colorspace < HD_COLORSPACE_RESERVED4)
			{
				if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_id]].st_packets_infoframes.st_avi_infoframe.e_colorimetry == HD_COLORIMETRY_ITU_709)
				{
					g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT709;
				} 
				else
				{
					g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT601;
				}
				
			}
			api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id], MS_FALSE);
			if(g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_colorspace == HD_COLORSPACE_YCBCR420)
			{
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
			}
			else
			{
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_out_colorspace = g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_colorspace;
				g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_out_color_range = g_st_txcsc_dev[u8_tx_id].st_csc_config_param.csc_in_color_range;
			}
			api_csc_tx_set(&g_st_txcsc_dev[u8_tx_id]); 
			g_st_tx_mux[u8_tx_id].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_tx_id]];
			g_st_tx_mux[u8_tx_id].u8_video_mux = g_u8_misc_module_rx[g_u8_HD_RX_PROT[Matrix_Tx_Mapping[u8_tx_id]]];
			api_misc_tx_mux_cfg(&g_st_tx_mux[u8_tx_id]);
			api_hd_tx_config_output(&g_st_hd_tx[u8_tx_id]); 

			api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id], ENABLE);
			//audio
			app_tx_audio_cfg(Matrix_Tx_Mapping[u8_tx_id], u8_tx_id);
		}
	}
}

VOID switch_bypass_channel(UINT8 u8_rx_sel, UINT8 u8_tx_sel)
{
	api_hd_tx_output_en(&g_st_hd_tx[u8_tx_sel], MS_FALSE);
	mculib_delay_ms(200);
	g_st_hd_tx[u8_tx_sel].u8_txIndex = g_u8_HD_TX_PROT[u8_tx_sel];
	g_st_hd_tx[u8_tx_sel].u8_color_depth = g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].u8_color_depth;
	g_st_hd_tx[u8_tx_sel].u8_audio_src = FROM_HD_RX_AUDIO;
	g_st_hd_tx[u8_tx_sel].u8_video_clk_src = FROM_RX_TMDS_CLK;
	g_st_hd_tx[u8_tx_sel].u32_tmds_clk = g_stVideo_HDRx_Timing[Matrix_Tx_Mapping[u8_tx_sel]].u16_pixclk;
	memcpy((&g_st_hd_tx[u8_tx_sel].st_infoframes_packets), &g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes, sizeof(HD_INFOFRAMES_PACKETS_T));
	api_hd_tx_output_en(&g_st_hd_tx[u8_tx_sel], MS_FALSE);
	api_hd_tx_config_output(&g_st_hd_tx[u8_tx_sel]);
	
	g_st_tx_mux[u8_tx_sel].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_tx_sel]];
	g_st_tx_mux[u8_tx_sel].u8_video_mux = g_u8_misc_module_rx[g_u8_HD_RX_PROT[Matrix_Tx_Mapping[u8_tx_sel]]];
	api_misc_tx_mux_cfg(&g_st_tx_mux[u8_tx_sel]);
	
	g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.u8_pixel_repeat = HD_PIXEL_REPEAT_X1;
	g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_colorspace = g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].u8_color_space;//st_packets_infoframes.st_avi_infoframe.e_colorspace;
	
	if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes.st_avi_infoframe.e_colorspace == HD_COLORSPACE_RGB)
	{
		if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes.st_avi_infoframe.e_rgb_quantization_range < HD_QUANTIZATION_RANGE_FULL)
		{
			g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range = CSC_RGB_16_235;
		} 
		else
		{
			g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range = CSC_RGB_0_255;
		}
		
	}
	else if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes.st_avi_infoframe.e_colorspace > HD_COLORSPACE_RGB &&
			g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes.st_avi_infoframe.e_colorspace < HD_COLORSPACE_RESERVED4)
	{
		if(g_st_hd_rx[Matrix_Tx_Mapping[u8_tx_sel]].st_packets_infoframes.st_avi_infoframe.e_colorimetry == HD_COLORIMETRY_ITU_709)
		{
			g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT709;
		} 
		else
		{
			g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range = CSC_YUV444_BT601;
		}
		
	}
	if(g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_colorspace == HD_COLORSPACE_YCBCR420)
	{
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
	}
	else
	{
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_out_colorspace = g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_colorspace;
		g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_out_color_range = g_st_txcsc_dev[u8_tx_sel].st_csc_config_param.csc_in_color_range;
	}
	api_csc_tx_set(&g_st_txcsc_dev[u8_tx_sel]);

	api_hd_tx_output_en(&g_st_hd_tx[u8_tx_sel], MS_ENABLE);
	//audio
	app_tx_audio_cfg(Matrix_Tx_Mapping[u8_tx_sel], u8_tx_sel);
	LOG("[INFO] BYPASS_MATRIX_SWITCHER switch.");
	LOG2("[INFO] HD TX: ", u8_tx_sel + 1);
	LOG2("[INFO] HD RX: ", Matrix_Tx_Mapping[u8_tx_sel] + 1);
}

#endif
