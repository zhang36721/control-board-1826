#ifndef _APP_SINGLE_MODE_C_
#define _APP_SINGLE_MODE_C_

#include "videoprocess.h"

SINGLE_VIPSS_T gst_single_vipss_dev[4];
SINGLE_VOPSS_T gst_single_vopss_dev[4];
BOOL b_freeze_status[4] = {0};
BOOL g_dsc_enable;

static VOID _single_vipss_instance_init(SINGLE_VIPSS_T *pst_single_vipss_dev)
{
	UINT8 u8_path_sel = pst_single_vipss_dev->u8_vipss_sel;
	
	api_sdn_instance_init(&pst_single_vipss_dev->st_single_sdn_dev, g_u8_sdn_idx[u8_path_sel]);
	
	if(pst_single_vipss_dev->b_is_dsc)
	{
		gst_single_vipss_dev[u8_path_sel].st_single_vwdma_dev.u8_sel = g_u8_dma_dsc_idx[u8_path_sel];	
		api_vwdma_instance_init(&pst_single_vipss_dev->st_single_vwdma_dev, g_u8_dma_dsc_idx[u8_path_sel]); 
		api_dsc_encoder_instance_init(&pst_single_vipss_dev->st_encoder_dev,g_u8_dsc_idx[u8_path_sel]);
	}
    else
	{
		gst_single_vipss_dev[u8_path_sel].st_single_vwdma_dev.u8_sel = g_u8_dma_idx[u8_path_sel];	
		api_vwdma_instance_init(&pst_single_vipss_dev->st_single_vwdma_dev, g_u8_dma_idx[u8_path_sel]); 		
	}
}
static VOID _single_vipss_cfg(BOOL b_is_dsc,UINT8 u8_vipss_sel)
{
	UINT16 u16_in_size_h, u16_in_size_v;
	u16_in_size_h = g_stVideo_HDRx_Timing[u8_vipss_sel].u16_hactive;
	u16_in_size_v = g_stVideo_HDRx_Timing[u8_vipss_sel].u16_vactive;
	//vipss rx sel  
	gst_single_vipss_dev[u8_vipss_sel].st_vipss_mux_dev.u8_src_module = g_u8_misc_module_rx[g_u8_HD_RX_PROT[u8_vipss_sel]];   
	gst_single_vipss_dev[u8_vipss_sel].st_vipss_mux_dev.u8_dst_module = g_u8_misc_module_vipss[u8_vipss_sel];
	api_misc_rx_mux_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_vipss_mux_dev);  

	//vipss rx csc config
	g_st_vipss_csc[u8_vipss_sel].u8_sel = g_u8_csc_idx[u8_vipss_sel];
	if(g_st_hd_rx[u8_vipss_sel].u8_color_space == HD_COLORSPACE_YCBCR420)
	{
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;//just for bypass
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.u8_pixel_repeat  = HD_PIXEL_REPEAT_X1;			
	}
	else
	{
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_in_colorspace = g_st_hd_rx[u8_vipss_sel].u8_color_space;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_colorspace = CHIP_INTERNAL_PROCESS_COLORSPACE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.csc_out_color_range = CHIP_INTERNAL_PROCESS_COLORRANGE;
		g_st_vipss_csc[u8_vipss_sel].st_csc_config_param.u8_pixel_repeat  = HD_PIXEL_REPEAT_X1;	
	}

	api_csc_rx_set(&g_st_vipss_csc[u8_vipss_sel]);
	
	if(b_is_dsc)
	{
		//vipss mux config
		if(g_st_hd_rx[u8_vipss_sel].u8_color_space == HD_COLORSPACE_YCBCR420)
		{
			api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_6);  /**< CSC -> 4KSDN -> SDN -> DSC_E -> VWDMA0 */
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_idx = g_u8_4ksdn_idx[u8_vipss_sel];
			api_sdn4k_init(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_input_colorSpace = HD_COLORSPACE_YCBCR420;
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_output_colorSpace = HD_COLORSPACE_YCBCR444;
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_mode = SDN4K_MODE_AUTO;
			api_sdn4k_config(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
			api_sdn4k_enable(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev, MS_ENABLE);
			u16_in_size_v = g_stVideo_HDRx_Timing[u8_vipss_sel].u16_vactive / 2;
		}
		else
		{
			api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_3);  //**< CSC -> SDN -> DSC_E -> VWDMA0 */
		}
		
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.u8_sel = g_u8_sdn_idx[u8_vipss_sel];
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.b_enable = MS_TRUE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.u8_sdn_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h = u16_in_size_h;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v = u16_in_size_v;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h = 
			return_minof_three(gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h, g_stVideo_HDTx_Timing.u16_hactive, MEMORY_1080P_H_SIZE_MAX);
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v = 
			return_minof_three(gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v, g_stVideo_HDTx_Timing.u16_vactive, MEMORY_1080P_V_SIZE_MAX);		
		api_sdn_enable_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		api_sdn_scaler_size_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		api_sdn_csc_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		
		//vipss dsc config
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.u8_sel = g_u8_dsc_idx[u8_vipss_sel];
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u16_hactive 			= gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u16_vactive 			= gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;        
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u8_color_space  		= CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u8_dsc_data_path_clk 	= PXL_CLK_DIV2;
		gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u8_dsc_ratio 			= DSC_RATIO_4X;       
		api_dsc_encoder_enable(&gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev, MS_TRUE);
		api_dsc_encoder_set(&gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev);
			
		//vipss vwdma config  
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_sel                                   = g_u8_dma_dsc_idx[u8_vipss_sel];
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.b_enable                                 = MS_TRUE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_h_size              = gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u16_hactive;      
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_v_size              = gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u16_vactive;        
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_color_space 							= CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u16_line_offset 							= gst_single_vipss_dev[u8_vipss_sel].st_encoder_dev.st_dsc_config_param.u16_hactive;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_dsc_ratio 							= DSC_RATIO_4X;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 = 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 = 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE + SINGLE_BUF_MEMORY_1080P_SIZE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2 = 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE + 2 * SINGLE_BUF_MEMORY_1080P_SIZE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u16_request_len 						    = 128;
		api_vwdma_start_addr_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		api_vwdma_line_offset_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		api_vwdma_burst_num_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev); 
		api_vwdma_request_len_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);		
		api_vwdma_enable_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
			
	}
	else
	{		
		//vipss mux config
		if(g_st_hd_rx[u8_vipss_sel].u8_color_space == HD_COLORSPACE_YCBCR420)
		{
			api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_5);  /**< CSC -> 4KSDN -> SDN -> VWDMA1 */
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_idx = g_u8_4ksdn_idx[u8_vipss_sel];
			api_sdn4k_init(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_input_colorSpace = HD_COLORSPACE_YCBCR420;
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_output_colorSpace = HD_COLORSPACE_YCBCR444;
			gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev.u8_sdn4k_mode = SDN4K_MODE_AUTO;
			api_sdn4k_config(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev);
			api_sdn4k_enable(&gst_single_vipss_dev[u8_vipss_sel].st_sdn4k_dev, MS_ENABLE);
			u16_in_size_v = g_stVideo_HDRx_Timing[u8_vipss_sel].u16_vactive / 2;
		}
		else
		{
			api_misc_vipss_mux_cfg(u8_vipss_sel, MISC_VIPSS_MUXTYPE_4);  //< CSC -> SDN -> VWDMA1
		}

		//vipss sdn config     
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.u8_sel = g_u8_sdn_idx[u8_vipss_sel];
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.b_enable = MS_TRUE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.u8_sdn_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h = u16_in_size_h ? u16_in_size_h : 1920;
		gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v = u16_in_size_v > 0 ? u16_in_size_v : 1080;
		// I2P 
		if(g_st_hd_rx[u8_vipss_sel].st_rx_video_timing.u8_polarity % 2 == 0)
		{
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h = u16_in_size_h ? u16_in_size_h : 1920;
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v = u16_in_size_v > 0 ? u16_in_size_v / 2 : 1080;
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h = 
				return_minof_three(u16_in_size_h, g_stVideo_HDTx_Timing.u16_hactive, MEMORY_1080P_H_SIZE_MAX);
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v = 
				return_minof_three(u16_in_size_v / 2, g_stVideo_HDTx_Timing.u16_vactive, MEMORY_1080P_V_SIZE_MAX);		
		}
		else
		{ 
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h = u16_in_size_h ? u16_in_size_h : 1920;
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v = u16_in_size_v > 0 ? u16_in_size_v : 1080;
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h = 
				return_minof_three(gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_h, g_stVideo_HDTx_Timing.u16_hactive, MEMORY_1080P_H_SIZE_MAX);
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v = 
				return_minof_three(gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_InRect.u16_v, g_stVideo_HDTx_Timing.u16_vactive, MEMORY_1080P_V_SIZE_MAX);		
		}
		api_sdn_enable_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		api_sdn_scaler_size_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		api_sdn_csc_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev);
		//vipss vwdma config
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_sel = g_u8_dma_idx[u8_vipss_sel];
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.b_enable = MS_TRUE;
		api_vwdma_enable_set(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_h_size = 
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_v_size = 
			gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u16_line_offset = gst_single_vipss_dev[u8_vipss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u16_request_len 						    = 128;
		
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0 = MEMORY_BUF_ADDR_START + 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1 = MEMORY_BUF_ADDR_START + 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE + SINGLE_BUF_MEMORY_1080P_SIZE;
		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2 = MEMORY_BUF_ADDR_START + 3 * u8_vipss_sel * SINGLE_BUF_MEMORY_1080P_SIZE + 2 * SINGLE_BUF_MEMORY_1080P_SIZE;
		

		api_vwdma_start_addr_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		api_vwdma_line_offset_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		api_vwdma_burst_num_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);
		api_vwdma_request_len_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev);	

		gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev.u8_vwdma_rotatation_deg = 0;
		api_vwdma_rotation_cfg(&gst_single_vipss_dev[u8_vipss_sel].st_single_vwdma_dev); 
   }	
}

static VOID _single_vopss_instance_init(SINGLE_VOPSS_T *pst_single_vopss_dev)
{
	UINT8 u8_path_sel = pst_single_vopss_dev->u8_vopss_sel;
	
	if(pst_single_vopss_dev->b_is_dsc)
	{
		api_vrdma_instance_init(&pst_single_vopss_dev->st_single_vrdma_dev, g_u8_dma_dsc_idx[u8_path_sel]);	
		api_su_instance_init(&pst_single_vopss_dev->st_single_su_dev, g_u8_su_dsc_idx[u8_path_sel]);
		api_dsc_decoder_instance_init(&pst_single_vopss_dev->st_decoder_dev,g_u8_dsc_idx[u8_path_sel]);
	}
	else
	{
		api_vrdma_instance_init(&pst_single_vopss_dev->st_single_vrdma_dev, g_u8_dma_idx[u8_path_sel]);	
		api_su_instance_init(&pst_single_vopss_dev->st_single_su_dev, g_u8_su_idx[u8_path_sel]);
	}

	
}

static VOID _single_vopss_cfg(BOOL b_is_dsc,UINT8 u8_vopss_sel)
{
	MISC_MODULE_CLK_T  st_su_clk, st_mixer_clk;
			
    if(!b_is_dsc)
	{
		gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev.u8_sel              = g_u8_ss_ch_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = u8_vopss_sel;    
		api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev);
		//vopss VRDMA config
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_sel = g_u8_dma_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.b_enable = MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_mem_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_out_size.u16_h_size = 
			gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_out_size.u16_v_size = 
			gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u16_line_offset = 
			gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_0 =
			gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_1 = 
			gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_2 = 
			gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u16_request_len = 128; 
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_mirror_cfg = DMA_H_V_MIRROR_ALL_DISABLE;           
		api_vrdma_mirror_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_enable_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_start_addr_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_burst_num_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_line_offset_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_request_len_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);				
		// su clk src
		st_su_clk.u8_dst_module = g_u8_misc_su[u8_vopss_sel];
		st_su_clk.u8_src_clk = MISC_CLKSRC_VDSTMG1;
		api_misc_su_clkin_sel(&st_su_clk);
		
		//vopss su config
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.u8_sel = g_u8_su_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.b_enable = MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.u8_su_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		if(gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h == 0)
			gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_h = 720;
		else
			gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_h = 
				gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		if(gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v == 0)
			gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_v = 480;
		else
			gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_v = 
				gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_OutRect.u16_h = g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_OutRect.u16_v = g_stVideo_HDTx_Timing.u16_vactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u8_dis_win_en = MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_hde_st = g_stVideo_HDTx_Timing.u16_hoffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_hde_sp = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_vde_st = g_stVideo_HDTx_Timing.u16_voffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_vde_sp = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		api_su_enable_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_scaler_size_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_csc_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_video_win_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_display_win_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);	
		//mixer clk sel
		st_mixer_clk.u8_dst_module = g_u8_misc_mixer[u8_vopss_sel];
		st_mixer_clk.u8_src_clk = g_u8_misc_clksrc[VOPSS1];
		api_misc_mixer_clkin_sel(&st_mixer_clk);	
	}
	else
	{
		gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev.u8_sel 			  = g_u8_ss_ch_dsc_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = u8_vopss_sel;    
		api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[u8_vopss_sel].st_single_ss_vopss_dev);
		
		// vopss dsc config
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.u8_sel = g_u8_dsc_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.st_dsc_config_param.u16_hactive      	 	= gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_h_size;
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.st_dsc_config_param.u16_vactive       	= gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_in_size.u16_v_size;
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.st_dsc_config_param.u8_color_space    	= CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.st_dsc_config_param.u8_dsc_ratio      	= DSC_RATIO_4X;
		gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev.st_dsc_config_param.u8_dsc_data_path_clk 	=PXL_CLK_DIV2;
		api_dsc_decoder_enable(&gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev, MS_TRUE);
		api_dsc_decoder_set(&gst_single_vopss_dev[u8_vopss_sel].st_decoder_dev);
		
		gst_single_vopss_dev[u8_vopss_sel].st_single_com_rdma_dev.u8_sel = g_u8_dma_dsc_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_com_rdma_dev.st_dsc_csc_cfg.u8_csc_mode = CHIP_INTERNAL_PROCESS_COLORSPACE;
		api_dma_common_dsc_csc_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_com_rdma_dev);
		
		//vopss VRDMA config
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_sel = g_u8_dma_dsc_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.b_enable = MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_mem_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_out_size.u16_h_size 			= gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_out_size.u16_v_size 			= gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_dsc_ratio       						= DSC_RATIO_4X;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u16_line_offset 							= gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_0 = gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_0;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_1 = gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_1;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.st_vrdma_start_addr.u32_dma_start_addr_2 = gst_single_vipss_dev[u8_vopss_sel].st_single_vwdma_dev.st_vwdma_start_addr.u32_dma_start_addr_2;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u16_request_len = 128;
		gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev.u8_mirror_cfg = DMA_H_V_MIRROR_ALL_DISABLE;           
		api_vrdma_mirror_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_start_addr_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_burst_num_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_line_offset_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
	    api_vrdma_request_len_cfg(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);
		api_vrdma_enable_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_vrdma_dev);	

		//vopss su config
		st_su_clk.u8_dst_module = g_u8_misc_dsc_su[u8_vopss_sel];
		st_su_clk.u8_src_clk = MISC_CLKSRC_VDSTMG1;
		api_misc_su_clkin_sel(&st_su_clk);
		
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.u8_sel = g_u8_su_dsc_idx[u8_vopss_sel];
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.b_enable = MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.u8_su_color_space = CHIP_INTERNAL_PROCESS_COLORSPACE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_h = gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_h;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_MemRect.u16_v = gst_single_vipss_dev[u8_vopss_sel].st_single_sdn_dev.st_video_size.st_MemRect.u16_v;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_OutRect.u16_h 	= g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_size.st_OutRect.u16_v 	= g_stVideo_HDTx_Timing.u16_vactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_hde_st 		= g_stVideo_HDTx_Timing.u16_hoffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_hde_sp 		= g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_vde_st 		= g_stVideo_HDTx_Timing.u16_voffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_video_win.u16_vde_sp 		= g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u8_dis_win_en 		= MS_TRUE;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_hde_st           = g_stVideo_HDTx_Timing.u16_hoffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_hde_sp           = g_stVideo_HDTx_Timing.u16_hoffset + g_stVideo_HDTx_Timing.u16_hactive;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_vde_st           = g_stVideo_HDTx_Timing.u16_voffset;
		gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev.st_dis_win.u16_vde_sp           = g_stVideo_HDTx_Timing.u16_voffset + g_stVideo_HDTx_Timing.u16_vactive;
		api_su_enable_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_scaler_size_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_csc_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_video_win_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);
		api_su_display_win_set(&gst_single_vopss_dev[u8_vopss_sel].st_single_su_dev);	
		
		//mixer clk sel
		st_mixer_clk.u8_dst_module = g_u8_misc_mixer[u8_vopss_sel];
		st_mixer_clk.u8_src_clk = g_u8_misc_clksrc[VOPSS1];
		api_misc_mixer_clkin_sel(&st_mixer_clk);	
	}


	//vopss tx csc config
	g_st_txcsc_dev[u8_vopss_sel].u8_sel = g_u8_csc_idx[g_u8_HD_TX_PROT[u8_vopss_sel]];
	g_st_txcsc_dev[u8_vopss_sel].st_csc_config_param.csc_in_colorspace   = CHIP_INTERNAL_PROCESS_COLORSPACE;
	g_st_txcsc_dev[u8_vopss_sel].st_csc_config_param.csc_in_color_range  = CHIP_INTERNAL_PROCESS_COLORRANGE;
	g_st_txcsc_dev[u8_vopss_sel].st_csc_config_param.csc_out_colorspace  = CHIP_TX_CSC_OUT_COLORSPACE;
	g_st_txcsc_dev[u8_vopss_sel].st_csc_config_param.csc_out_color_range = CHIP_TX_CSC_OUT_COLORRANGE;
	g_st_txcsc_dev[u8_vopss_sel].st_csc_config_param.u8_pixel_repeat     = HD_PIXEL_REPEAT_X1;
	api_csc_tx_set(&g_st_txcsc_dev[u8_vopss_sel]);			
}

static VOID _app_video_single_mixer_cfg(UINT8 u8_vmixer_idx, UINT8 u8_layer)
{
	MISC_MODULE_CLK_T g_st_module_clk;
	g_st_module_clk.u8_dst_module = g_u8_misc_mixer[u8_vmixer_idx];
	g_st_module_clk.u8_clkdiv = MISC_CLKDIV_1;
	g_st_module_clk.u8_src_clk = MISC_CLKSRC_VDSTMG1;
	api_misc_mixer_clkin_sel(&g_st_module_clk);
	
	g_st_video_mixer_dev[u8_vmixer_idx].u8_video_mixer_idx = u8_vmixer_idx;
	api_video_mixer_instance_init(&g_st_video_mixer_dev[u8_vmixer_idx], u8_vmixer_idx);
	
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel = &g_st_mixer_layer_sel;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_alf = &g_st_mixer_alf_cfg;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bd_attr  = NULL;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bd_color = NULL;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_bg_color = &g_st_bg_color;
	
	g_st_video_mixer_dev[u8_vmixer_idx].u8_video_mixer_idx = u8_vmixer_idx;
	g_st_video_mixer_dev[u8_vmixer_idx].b_videomixer_en = MS_TRUE;
	api_video_mixer_bg_color_config(&g_st_video_mixer_dev[u8_vmixer_idx]);
	api_video_mixer_enable(&g_st_video_mixer_dev[u8_vmixer_idx]);
	
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer0_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer1_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer2_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer3_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer4_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer5_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer6_src_sel = VOPSS1_DSC_SU;
	g_st_video_mixer_dev[u8_vmixer_idx].pst_layer_sel->u8_layer7_src_sel = u8_layer;
	
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

static VOID _single_mode_frc_cfg(void)
{
	//VIPSS instance	
	gst_single_vipss_dev[VIPSS1].b_is_dsc = g_dsc_enable;
	gst_single_vipss_dev[VIPSS1].u8_vipss_sel = VIPSS1;
	_single_vipss_instance_init(&gst_single_vipss_dev[VIPSS1]); 

	gst_single_vipss_dev[VIPSS2].b_is_dsc = g_dsc_enable;
	gst_single_vipss_dev[VIPSS2].u8_vipss_sel = VIPSS2;
	_single_vipss_instance_init(&gst_single_vipss_dev[VIPSS2]); 

	gst_single_vipss_dev[VIPSS3].b_is_dsc = g_dsc_enable;
	gst_single_vipss_dev[VIPSS3].u8_vipss_sel = VIPSS3;
	_single_vipss_instance_init(&gst_single_vipss_dev[VIPSS3]); 

	gst_single_vipss_dev[VIPSS4].b_is_dsc = g_dsc_enable;
	gst_single_vipss_dev[VIPSS4].u8_vipss_sel = VIPSS4;
	_single_vipss_instance_init(&gst_single_vipss_dev[VIPSS4]); 
		
	//VOPSS instance
	gst_single_vopss_dev[VOPSS1].b_is_dsc = g_dsc_enable;
	gst_single_vopss_dev[VOPSS1].u8_vopss_sel = VOPSS1;
	_single_vopss_instance_init(&gst_single_vopss_dev[VOPSS1]);
	
	gst_single_vopss_dev[VOPSS2].b_is_dsc = g_dsc_enable;
	gst_single_vopss_dev[VOPSS2].u8_vopss_sel = VOPSS2;
	_single_vopss_instance_init(&gst_single_vopss_dev[VOPSS2]);

	gst_single_vopss_dev[VOPSS3].b_is_dsc = g_dsc_enable;
	gst_single_vopss_dev[VOPSS3].u8_vopss_sel = VOPSS3;
	_single_vopss_instance_init(&gst_single_vopss_dev[VOPSS3]);

	gst_single_vopss_dev[VOPSS4].b_is_dsc = g_dsc_enable;
	gst_single_vopss_dev[VOPSS4].u8_vopss_sel = VOPSS4;
	_single_vopss_instance_init(&gst_single_vopss_dev[VOPSS4]);
	
	//DMA config  
    gst_single_vipss_dev[VIPSS1].st_single_ss_vipss_dev.u8_sel = (gst_single_vipss_dev[VIPSS1].b_is_dsc)?g_u8_ss_ch_dsc_idx[VIPSS1]:g_u8_ss_ch_idx[VIPSS1];
    gst_single_vipss_dev[VIPSS1].st_single_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS1;
    api_ss_vipss_wdma_buf_sel(&gst_single_vipss_dev[VIPSS1].st_single_ss_vipss_dev);   

	gst_single_vipss_dev[VIPSS2].st_single_ss_vipss_dev.u8_sel = (gst_single_vipss_dev[VIPSS2].b_is_dsc)?g_u8_ss_ch_dsc_idx[VIPSS2]:g_u8_ss_ch_idx[VIPSS2];
    gst_single_vipss_dev[VIPSS2].st_single_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS2;
    api_ss_vipss_wdma_buf_sel(&gst_single_vipss_dev[VIPSS2].st_single_ss_vipss_dev); 

	gst_single_vipss_dev[VIPSS3].st_single_ss_vipss_dev.u8_sel = (gst_single_vipss_dev[VIPSS3].b_is_dsc)?g_u8_ss_ch_dsc_idx[VIPSS3]:g_u8_ss_ch_idx[VIPSS3];
    gst_single_vipss_dev[VIPSS3].st_single_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS3;
    api_ss_vipss_wdma_buf_sel(&gst_single_vipss_dev[VIPSS3].st_single_ss_vipss_dev); 

	gst_single_vipss_dev[VIPSS4].st_single_ss_vipss_dev.u8_sel = (gst_single_vipss_dev[VIPSS4].b_is_dsc)?g_u8_ss_ch_dsc_idx[VIPSS4]:g_u8_ss_ch_idx[VIPSS4];
    gst_single_vipss_dev[VIPSS4].st_single_ss_vipss_dev.u8_vipss_wdma_id_sel = VIPSS4;
    api_ss_vipss_wdma_buf_sel(&gst_single_vipss_dev[VIPSS4].st_single_ss_vipss_dev);
    
	gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev.u8_sel 						  = (gst_single_vopss_dev[VOPSS1].b_is_dsc)?g_u8_dma_dsc_idx[VOPSS1]:g_u8_dma_idx[VOPSS1];
	gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev.u8_dma_buf_num_sel			  = SINGLE_BUF_NUM;
	gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext 	  = MS_FALSE;
	gst_single_vopss_dev[VOPSS1].st_single_ss_vopss_dev.u8_sel = (gst_single_vopss_dev[VOPSS1].b_is_dsc)?g_u8_ss_ch_dsc_idx[VOPSS1]:g_u8_ss_ch_idx[VOPSS1];
	gst_single_vopss_dev[VOPSS1].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS1;
	api_dma_common_buf_num_set(&gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_single_vopss_dev[VOPSS1].st_single_com_rdma_dev);   
	api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[VOPSS1].st_single_ss_vopss_dev); 
	gst_single_vopss_dev[VOPSS1].st_single_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS1;
	api_ss_vopss_dst_buf_id_sel(&gst_single_vopss_dev[VOPSS1].st_single_ss_vopss_dev);
 
	gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev.u8_sel = (gst_single_vopss_dev[VOPSS2].b_is_dsc)?g_u8_dma_dsc_idx[VOPSS2]:g_u8_dma_idx[VOPSS2];
	gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev.u8_dma_buf_num_sel = SINGLE_BUF_NUM;
	gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE; 
	gst_single_vopss_dev[VOPSS2].st_single_ss_vopss_dev.u8_sel = (gst_single_vopss_dev[VOPSS2].b_is_dsc)?g_u8_ss_ch_dsc_idx[VOPSS2]:g_u8_ss_ch_idx[VOPSS2];
	gst_single_vopss_dev[VOPSS2].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS2;
	api_dma_common_buf_num_set(&gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_single_vopss_dev[VOPSS2].st_single_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[VOPSS2].st_single_ss_vopss_dev);
	gst_single_vopss_dev[VOPSS2].st_single_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS2;
	api_ss_vopss_dst_buf_id_sel(&gst_single_vopss_dev[VOPSS2].st_single_ss_vopss_dev);

	gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev.u8_sel = (gst_single_vopss_dev[VOPSS3].b_is_dsc)?g_u8_dma_dsc_idx[VOPSS3]:g_u8_dma_idx[VOPSS3];
	gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev.u8_dma_buf_num_sel = SINGLE_BUF_NUM;
	gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE;
	gst_single_vopss_dev[VOPSS3].st_single_ss_vopss_dev.u8_sel = (gst_single_vopss_dev[VOPSS3].b_is_dsc)?g_u8_ss_ch_dsc_idx[VOPSS3]:g_u8_ss_ch_idx[VOPSS3];
	gst_single_vopss_dev[VOPSS3].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS3;
	api_dma_common_buf_num_set(&gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_single_vopss_dev[VOPSS3].st_single_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[VOPSS3].st_single_ss_vopss_dev);
	gst_single_vopss_dev[VOPSS3].st_single_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS3;
	api_ss_vopss_dst_buf_id_sel(&gst_single_vopss_dev[VOPSS3].st_single_ss_vopss_dev);
	
	gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev.u8_sel = (gst_single_vopss_dev[VOPSS4].b_is_dsc)?g_u8_dma_dsc_idx[VOPSS4]:g_u8_dma_idx[VOPSS4];
	gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev.u8_dma_buf_num_sel = SINGLE_BUF_NUM;
	gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_AUTO;
	gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_AUTO; 
	gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev.st_frc_cfg.b_frc_ctl_ext = MS_FALSE;
	gst_single_vopss_dev[VOPSS4].st_single_ss_vopss_dev.u8_sel = (gst_single_vopss_dev[VOPSS4].b_is_dsc)?g_u8_ss_ch_dsc_idx[VOPSS4]:g_u8_ss_ch_idx[VOPSS4];
	gst_single_vopss_dev[VOPSS4].st_single_ss_vopss_dev.u8_vopss_src_vb_sel = VIPSS4;
	api_dma_common_buf_num_set(&gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev);
	api_dma_comom_buf_id_mode_cfg(&gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev);
	api_dma_comom_buf_frc_ctl_ext(&gst_single_vopss_dev[VOPSS4].st_single_com_rdma_dev);
	api_ss_vopss_src_vb_sel(&gst_single_vopss_dev[VOPSS4].st_single_ss_vopss_dev);
	gst_single_vopss_dev[VOPSS4].st_single_ss_vopss_dev.u8_vopss_dst_buf_id_sel = VOPSS4;
	api_ss_vopss_dst_buf_id_sel(&gst_single_vopss_dev[VOPSS4].st_single_ss_vopss_dev);									 
}

VOID single_video_mute(UINT8 u8_path, BOOL b_mute)
{
	BOOL b_int;
	gst_single_vopss_dev[u8_path].st_single_su_dev.u8_sel = g_dsc_enable ? g_u8_su_dsc_idx[u8_path]: g_u8_su_idx[u8_path];
	gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.bd_enable = b_mute;
	
	if(CHIP_INTERNAL_PROCESS_COLORSPACE == HD_COLORSPACE_RGB)
	{
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8VR = 0x00;
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8YG = 0x00;
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8UB = 0x00;
	}
	else
	{
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8VR = 128;
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8YG = 16;
		gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_color.u8UB = 128;
	}

	gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.bd_cover = MS_TRUE;
	gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_size.u16_border_h = g_stVideo_HDTx_Timing.u16_hactive;
	gst_single_vopss_dev[u8_path].st_single_su_dev.st_video_border.border_size.u16_border_v = g_stVideo_HDTx_Timing.u16_vactive;
	api_su_video_border_set(&gst_single_vopss_dev[u8_path].st_single_su_dev);  
	g_u16_timer_out = 0;
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	do
	{
		g_u16_timer_out ++;
		b_int = api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB);           
	}
	while((b_int != MS_TRUE) && (g_u16_timer_out < 500)); 	
	api_su_trigger(&gst_single_vopss_dev[u8_path].st_single_su_dev); 
}


VOID _single_video_freeze(UINT8 u8_path_sel)
{
	BOOL b_vb_h;
	DMA_COMMON_DEV_T st_dma_dev;
	//video freeze
	__disable_irq();   
	gst_single_vipss_dev[u8_path_sel].st_single_com_wdma_dev.u8_sel = g_u8_dma_dsc_idx[u8_path_sel];
	gst_single_vipss_dev[u8_path_sel].st_single_com_wdma_dev.st_frc_cfg.u8_src_buf_id_mode = DMA_BUF_SWITCH_MANU;
	gst_single_vipss_dev[u8_path_sel].st_single_com_wdma_dev.st_frc_cfg.u8_dst_buf_id_mode = DMA_BUF_SWITCH_MANU;
		 
	st_dma_dev.u8_sel = g_u8_dma_dsc_idx[u8_path_sel];
	api_dma_comom_buf_id_mode_cfg(&gst_single_vipss_dev[u8_path_sel].st_single_com_wdma_dev);
	g_u16_timer_out = 0;
	do
	{
		g_u16_timer_out ++;
		b_vb_h = api_misc_vb_status_get(g_u8_vb_path[u8_path_sel]);  
	}   
	while(b_vb_h == MS_FALSE && (g_u16_timer_out < 500));   
	
	api_dma_common_buf_id_get(&st_dma_dev); 
	u8_wdma_buf_id = st_dma_dev.st_dma_status.u8_wdma_buf_id;
	u8_rdma_buf_id = st_dma_dev.st_dma_status.u8_rdma_buf_id;
	u8_wdma_buf_id = (u8_rdma_buf_id + 1) % 3;
	st_dma_dev.st_frc_cfg.u8_src_buf_id_sel = u8_wdma_buf_id;
	st_dma_dev.st_frc_cfg.u8_dst_buf_id_sel = u8_rdma_buf_id;   
	api_dma_comom_buf_id_set(&st_dma_dev);        
	api_dma_common_trigger(&st_dma_dev);  	
    api_hd_tx_audio_mute(&g_st_hd_tx[u8_path_sel], MS_TRUE);
	__enable_irq(); 
}

VOID app_single_mode_config(VOID)
{
	UINT8 u8_i = 0;
	Bypass_Tx_Status[0] = 0;
	Bypass_Tx_Status[1] = 0;
	Bypass_Tx_Status[2] = 0;
	Bypass_Tx_Status[3] = 0;
	Vipss_Mapping_Rx[0] = g_u8_HD_RX_PROT[0];
	Vipss_Mapping_Rx[1] = g_u8_HD_RX_PROT[1];
	Vipss_Mapping_Rx[2] = g_u8_HD_RX_PROT[2];
	Vipss_Mapping_Rx[3] = g_u8_HD_RX_PROT[3];
	_single_mode_frc_cfg();				
	_single_vipss_cfg(g_dsc_enable,VIPSS1);      	
	_single_vipss_cfg(g_dsc_enable,VIPSS2); 
	_single_vipss_cfg(g_dsc_enable,VIPSS3); 
	_single_vipss_cfg(g_dsc_enable,VIPSS4); 		
	app_video_vipss_trig(g_dsc_enable?g_u8_ss_ch_dsc_idx[0]:g_u8_ss_ch_idx[0]);
	app_video_vipss_trig(g_dsc_enable?g_u8_ss_ch_dsc_idx[1]:g_u8_ss_ch_idx[1]);
	app_video_vipss_trig(g_dsc_enable?g_u8_ss_ch_dsc_idx[2]:g_u8_ss_ch_idx[2]);
	app_video_vipss_trig(g_dsc_enable?g_u8_ss_ch_dsc_idx[3]:g_u8_ss_ch_idx[3]);
	mculib_delay_ms(50); 
	_single_vopss_cfg(g_dsc_enable,VOPSS1);
	_single_vopss_cfg(g_dsc_enable,VOPSS2);
	_single_vopss_cfg(g_dsc_enable,VOPSS3);
	_single_vopss_cfg(g_dsc_enable,VOPSS4);
	for(u8_i = 0; u8_i < 4; u8_i ++)
	{
		//tx audio config
		app_tx_audio_cfg(Matrix_Tx_Mapping[u8_i], u8_i);
		
		 //tx video config
		g_st_tx_mux[u8_i].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_i]];
		g_st_tx_mux[u8_i].u8_video_mux = g_u8_misc_mixer[u8_i];
		api_misc_tx_mux_cfg(&g_st_tx_mux[u8_i]);
		
		g_st_hd_tx[u8_i].u8_txIndex = g_u8_HD_TX_PROT[u8_i];
		g_st_hd_tx[u8_i].b_is_hd = TRUE;
		g_st_hd_tx[u8_i].b_full_edid = 0;
		g_st_hd_tx[u8_i].u8_color_depth = HD_COLOR_DEPTH_24;
		g_st_hd_tx[u8_i].u8_audio_mode = HD_AUDIO_STREAM_TYPE_MAS;
		g_st_hd_tx[u8_i].u8_audio_src = FROM_HD_RX_AUDIO;
		g_st_hd_tx[u8_i].u8_video_clk_src = FROM_RX_OR_PLL_PIXEL_CLK;
		g_st_hd_tx[u8_i].u32_tmds_clk = g_stVideo_HDTx_Timing.u16_pixclk; 
		memcpy((&g_st_hd_tx[u8_i].st_infoframes_packets), &gst_inf_pack, sizeof(HD_INFOFRAMES_PACKETS_T));  
		api_hd_tx_output_en(&g_st_hd_tx[u8_i],MS_FALSE);
		api_hd_tx_config_output(&g_st_hd_tx[u8_i]); 
		api_hd_tx_output_en(&g_st_hd_tx[u8_i],MS_TRUE);
		_app_video_single_mixer_cfg(g_u8_mixer_idx[u8_i], g_dsc_enable?g_u8_mixer_layer_dsc_src[Matrix_Tx_Mapping[u8_i]]:g_u8_mixer_layer_src[Matrix_Tx_Mapping[u8_i]]);
	}
	if(g_dsc_enable)
		app_video_vopss_trig(g_u8_ss_ch_dsc_idx[0] | g_u8_ss_ch_dsc_idx[1] | g_u8_ss_ch_dsc_idx[2] | g_u8_ss_ch_dsc_idx[3]);
	else
		app_video_vopss_trig(g_u8_ss_ch_idx[0] | g_u8_ss_ch_idx[1] | g_u8_ss_ch_idx[2] | g_u8_ss_ch_idx[3]);
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}

VOID app_single_mode_rx_switch(UINT8 u8_rx, UINT8 u8_tx)
{
	BOOL b_int;
    //tx audio config
	__disable_irq(); 
	app_tx_audio_cfg(Matrix_Tx_Mapping[u8_tx], u8_tx);
	
	api_misc_int_status_clr(MISC_INTSRC_VDSTMG1VB);  
	g_u16_timer_out = 0;
	do
	{
		g_u16_timer_out ++;
		b_int = api_misc_int_status_get(MISC_INTSRC_VDSTMG1VB);           
	}
	while(b_int != MS_TRUE && (g_u16_timer_out < 500)); 
	
	g_st_video_mixer_dev[g_u8_mixer_idx[u8_tx]].pst_layer_sel->u8_layer7_src_sel = g_dsc_enable ? g_u8_mixer_layer_dsc_src[u8_rx]:g_u8_mixer_layer_src[u8_rx];
	api_video_mixer_layer_only_config(&g_st_video_mixer_dev[g_u8_mixer_idx[u8_tx]]);
	__enable_irq(); 
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F & 1 << u8_tx);
	LOG("[INFO] SEAMLESS_MATRIX_SWITCHER switch.");
	LOG2("[INFO] HD TX: ", u8_tx + 1);
	LOG2("[INFO] HD RX: ", Matrix_Tx_Mapping[u8_tx] + 1);
}

VOID app_single_output_resolution(UINT8 u8_vic)
{
    PLL_DEV_T         st_pll_dev;
    MISC_MODULE_CLK_T st_module_clk;
    VDS_TMG_DEV_T     st_timgen_dev;
    UINT8             u8_tx_id;    
    api_hd_tx_output_en(&g_st_hd_tx[0], MS_FALSE);
	api_hd_tx_output_en(&g_st_hd_tx[1], MS_FALSE);
	api_hd_tx_output_en(&g_st_hd_tx[2], MS_FALSE);
	api_hd_tx_output_en(&g_st_hd_tx[3], MS_FALSE);
    api_misc_timing_get(u8_vic, &g_stVideo_HDTx_Timing);
    st_timgen_dev.pst_timing = &g_stVideo_HDTx_Timing;
    st_timgen_dev.u8_timgen_id = TIMGEN_1;
    st_timgen_dev.b_timgen_en = MS_TRUE;
    st_timgen_dev.u8_workmode  = FreeRun_Mode;            
    
    st_pll_dev.b_osc_input = MS_TRUE;
    st_pll_dev.u8_pll_idx = PLLV1;
    st_pll_dev.u32_clk_out = st_timgen_dev.pst_timing->u16_pixclk * 10000;
    
    st_module_clk.u8_dst_module = MISC_VIDEO_MODULE_TIMGEN1;
    st_module_clk.u8_src_clk = MISC_CLKSRC_PLLV1;
    st_module_clk.u8_clkdiv = MISC_CLKDIV_1;
                                           
    //�H�U�t�m�O?�@�ǥ�?�ܪ�Rx�H?�����e���nscale      
	_single_vipss_cfg(g_dsc_enable,VIPSS1);      	
	_single_vipss_cfg(g_dsc_enable,VIPSS2); 
	_single_vipss_cfg(g_dsc_enable,VIPSS3); 
	_single_vipss_cfg(g_dsc_enable,VIPSS4); 		
	app_video_vipss_trig(g_u8_ss_ch_idx[0] | g_u8_ss_ch_idx[1] | g_u8_ss_ch_idx[2] | g_u8_ss_ch_idx[3]);
	mculib_delay_ms(50); 
	_single_vopss_cfg(g_dsc_enable,VOPSS1);
	_single_vopss_cfg(g_dsc_enable,VOPSS2);
	_single_vopss_cfg(g_dsc_enable,VOPSS3);
	_single_vopss_cfg(g_dsc_enable,VOPSS4);
	app_video_vopss_trig(g_u8_ss_ch_idx[0] | g_u8_ss_ch_idx[1] | g_u8_ss_ch_idx[2] | g_u8_ss_ch_idx[3]);
	
	for(u8_tx_id = 0; u8_tx_id < 4; u8_tx_id ++)
	{
		if(u8_tx_id == VIPSS1)
		{
			api_pll_config(&st_pll_dev);
			api_misc_timgen_clkin_sel(&st_module_clk);
			api_vds_tmg_config(&st_timgen_dev);
		}
		g_st_hd_tx[u8_tx_id].u8_txIndex = g_u8_HD_TX_PROT[u8_tx_id];
		g_st_hd_tx[u8_tx_id].b_is_hd = TRUE;
		g_st_hd_tx[u8_tx_id].b_full_edid = 0;
		g_st_hd_tx[u8_tx_id].u8_color_depth = HD_COLOR_DEPTH_24;
		g_st_hd_tx[u8_tx_id].u8_audio_mode = HD_AUDIO_STREAM_TYPE_MAS;
		g_st_hd_tx[u8_tx_id].u8_audio_src = FROM_HD_RX_AUDIO;
		g_st_hd_tx[u8_tx_id].u8_video_clk_src = FROM_RX_OR_PLL_PIXEL_CLK;
		g_st_hd_tx[u8_tx_id].u32_tmds_clk = g_stVideo_HDTx_Timing.u16_pixclk; 
		memcpy((&g_st_hd_tx[u8_tx_id].st_infoframes_packets), &gst_inf_pack, sizeof(HD_INFOFRAMES_PACKETS_T));
		api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id], MS_FALSE);	
		api_hd_tx_config_output(&g_st_hd_tx[u8_tx_id]);
		api_hd_tx_output_en(&g_st_hd_tx[u8_tx_id], MS_TRUE);
	}
	g_bOSDShown = MS_TRUE;
	sys_show_osd(g_bOSDShown, 0x0F);
}

VOID app_single_process(UINT8 u8_rx_idx)
{
    _single_vipss_cfg(g_dsc_enable,u8_rx_idx);
    app_video_vipss_trig(g_dsc_enable ? g_u8_ss_ch_dsc_idx[u8_rx_idx]:g_u8_ss_ch_idx[u8_rx_idx]);
    _single_vopss_cfg(g_dsc_enable,u8_rx_idx);
    app_video_vopss_trig(g_dsc_enable ? g_u8_ss_ch_dsc_idx[u8_rx_idx]:g_u8_ss_ch_idx[u8_rx_idx]);
	mculib_delay_ms(200);
}

VOID app_single_osd_init(VOID)
{
	UINT8 u8_idx = 0;
	for(u8_idx = 0; u8_idx < 4; u8_idx ++)
	{
		g_st_misc_osd_mux[u8_idx].b_enable = MS_TRUE;
		g_st_misc_osd_mux[u8_idx].u8_osd_module = g_u8_misc_osd_idx[u8_idx];
		g_st_misc_osd_mux[u8_idx].u8_tx_module = g_u8_misc_module_tx[g_u8_HD_TX_PROT[u8_idx]];
		api_misc_osd_timing_mux_cfg(&g_st_misc_osd_mux[u8_idx]);
	}
}

VOID app_single_osd_config(UINT8 u8_index)
{
	UINT8 u8_i = 0;
	st_str.u8_x = 0;
	st_str.u8_y = 0;
	st_font.u8_x = 0;
	st_font.u8_y = 0;
	st_osd_win_size.u8_osd_win = OSD_WIN_DISPLAY;
	st_osd_win_size.u16_w = 15;
	st_osd_win_size.u16_h = 4;
	st_osd_win_size.u16_x = 100;
	st_osd_win_size.u16_y = 100;
	app_single_osd_init();
	for(u8_i = 0; u8_i < 4; u8_i ++)
	{
		if(u8_index & 0x01)
		{
			api_osd_window_position_set(&g_st_osd_dev[u8_i], &st_osd_win_size);
			st_font.u8_y = 0;
			st_font.pu16_idx = (UINT16 *)str_osd_input_channel[Matrix_Tx_Mapping[u8_i]];
			st_font.u16_length = 5;
			api_osd_show_index(&g_st_osd_dev[u8_i], &st_font);	
			
			st_font.u8_y = 1;
			st_font.pu16_idx = (UINT16 *)str_osd_output_channel[u8_i];
			st_font.u16_length = 5;
			api_osd_show_index(&g_st_osd_dev[u8_i], &st_font);	
			
			st_str.u8_y = 2;
			set_osd_res(Matrix_Tx_Mapping[u8_i]);
			st_str.u16_len = 15;
			api_osd_show_length_string(&g_st_osd_dev[u8_i], &st_str);
			app_osd_trigger(u8_i);
		}
		u8_index >>= 1;
	}
}

#endif
