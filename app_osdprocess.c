#include "videoprocess.h"

extern BOOL b_osd_menu;

/* ????????? _by_zzw*/
VOID app_osd_clear(VOID)
{
	UINT8 u8_idx = 0, u8_mux = 0;
	OSD_STRING_T st_str;
	st_str.u8_x = 0;
	st_str.u8_y = 0;
	for(u8_idx = 0; u8_idx < 4; u8_idx ++)
	{
		st_str.pstr = "               ";
		st_str.u16_len = 15;
		api_osd_show_length_string(&g_st_osd_dev[u8_idx], &st_str);
		st_str.u8_y = 1;
		api_osd_show_length_string(&g_st_osd_dev[u8_idx], &st_str);
		st_str.u8_y = 2;
		api_osd_show_length_string(&g_st_osd_dev[u8_idx], &st_str);
		st_str.u8_y = 3;
		api_osd_show_length_string(&g_st_osd_dev[u8_idx], &st_str);
		
		for(u8_mux = 0; u8_mux < 4; u8_mux ++)
		{
			g_st_misc_osd_mux[u8_idx].b_enable = MS_FALSE;
			g_st_misc_osd_mux[u8_idx].u8_osd_module = g_u8_misc_osd_idx[u8_mux];
			g_st_misc_osd_mux[u8_idx].u8_tx_module = g_u8_misc_module_tx[u8_idx];
			api_misc_osd_timing_mux_cfg(&g_st_misc_osd_mux[u8_idx]);
		}
	}
}
/* ???OSD,?????????????????????? _by_zzw*/
VOID app_osd_init(VOID)
{
	UINT8 u8_i;
	OSD_INFO_T st_osd_info;
	OSD_TRANS_T  st_osd_tran;  
	
	st_osd_info.pu8_1bpp_data = (UINT8*)OSD_1BPP_FONT_TABLE;
	st_osd_info.pu8_2bpp_data = NULL;
	st_osd_info.pu8_4bpp_data = NULL;
	st_osd_info.pu8_colortab = (UINT8*)OSD_COLOR_TABLE;
	st_osd_info.u16_1bpp_num = FONT_1BPP_NUM;
	st_osd_info.u8_2bpp_num = FONT_2BPP_NUM;
	st_osd_info.u8_4bpp_num = FONT_4BPP_NUM;
	st_osd_info.u8_colortab_size = g_u8_color_table_size;
	st_osd_info.u8_font_height = FONT_HEIGHT;
	st_osd_info.u8_font_width = FONT_WIDTH;
	app_osd_clear();
	for(u8_i = 0; u8_i < 4; u8_i++)
	{
		api_osd_instance_init(&g_st_osd_dev[u8_i], g_u8_osd_idx[u8_i]);
		g_st_osd_dev[u8_i].u8_osd_idx = g_u8_osd_idx[u8_i];
		api_osd_init(&g_st_osd_dev[u8_i], &st_osd_info);
		st_osd_tran.b_trans_en = MS_TRUE;
		st_osd_tran.u8_alpha_ratio = 0xff;
		st_osd_tran.u8_trans_mode = 0;
		api_osd_trans_set(&g_st_osd_dev[u8_i],&st_osd_tran);
		api_osd_font_colortab_set(&g_st_osd_dev[u8_i],OSD_PATTERN_COLOR0);
		api_osd_colorspace_set(&g_st_osd_dev[u8_i], OSD_COLORSPACE_RGB);
		g_st_osd_dev[u8_i].b_osd_enable = MS_TRUE;
		api_osd_enable(&g_st_osd_dev[u8_i]);
		api_osd_clear(&g_st_osd_dev[u8_i]);
	}
	switch(g_u8_display_mode)
	{
		case MS1826_SEAMLESS_MATRIX_SWITCHER:	 
			app_single_osd_init();
			break;
		case MS1826_VIDEO_SPLITTER:
			app_splitter_osd_init();									 
			break;
		case MS1826_VIDEO_SPLICER:
			app_splicer_osd_init();	
			break;
		case MS1826_BYPASS_MATRIX_SWITCHER:
			break;
		default:
			break;
	}
}

/* ????????????????????????????????????????????? p_str ?? _by_zzw*/
VOID set_osd_res(UINT8 u8_rx_id)
{
	UINT16 u16_ch = 0;
	UINT8 u8_i = 0, u8_j = 0, u8_k = 0;
	if(api_hd_rx_get_video_timing(&g_st_hd_rx[u8_rx_id]) == ERR_OK)
	{
		if(g_st_hd_rx[u8_rx_id].u8_color_space == HD_COLORSPACE_YCBCR420)
			u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_hactive * 2;
		else
			u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_hactive;
		while(u16_ch)
		{
			u16_ch /= 10;
			u8_j++;
		}
		if(g_st_hd_rx[u8_rx_id].u8_color_space == HD_COLORSPACE_YCBCR420)
			u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_hactive * 2;
		else
			u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_hactive;
		u8_i = u8_j;
		while(u8_j)
		{
			u8_j--;
			p_str[u8_j] = u16_ch % 10 + '0';
			u16_ch /= 10;
		}
		
		p_str[u8_i] = 'x';
		u8_i ++;
		
		u8_j = 0;
		u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_vactive;
		while(u16_ch)
		{
			u16_ch /= 10;
			u8_j++;
		}
		u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_vactive;
		u8_k = u8_i + u8_j;
		while(u8_j)
		{
			u8_j--;
			p_str[u8_i + u8_j] = u16_ch % 10 + 48;
			u16_ch /= 10;
		}
		
		if(g_st_hd_rx[u8_rx_id].st_rx_video_timing.u8_polarity % 2)
			p_str[u8_k] = 'P';
		else
			p_str[u8_k] = 'I';
		
		u8_k++;
		p_str[u8_k] = '@';
		u8_k ++;
		
		u16_ch = g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_vfreq / 100;
		if(g_st_hd_rx[u8_rx_id].st_rx_video_timing.u16_vfreq % 100 > 50)
			u16_ch ++;
		p_str[u8_k] = u16_ch / 10 + 48;
		u8_k ++;
		p_str[u8_k] = u16_ch % 10 + 48;
		u8_k ++;
		p_str[u8_k] = 'H';
		u8_k ++;
		p_str[u8_k] = 'z';
		while(u8_k != 15)
		{
			u8_k ++;
			p_str[u8_k] = ' ';
		}
		st_str.pstr = p_str;
	}
	else
		st_str.pstr = "No Signal      ";
}

/* ?????OSD???? _by_zzw*/
VOID app_osd_trigger(UINT8 u8_osd_idx)
{
	g_u8OSDCount = 0;
	api_osd_window_enable(&g_st_osd_dev[u8_osd_idx], OSD_WIN_DISPLAY, MS_TRUE);
	api_osd_trigger(&g_st_osd_dev[u8_osd_idx]);
	api_osd_index_load_trigger(&g_st_osd_dev[u8_osd_idx]);	
}

/* ?????OSD?? _by_zzw*/
VOID sys_show_osd(BOOL b_show, UINT8 u8_index)
{
	if(b_show)
	{
		if(b_osd_menu == MS_FALSE)
		{
			switch(g_u8_display_mode)
			{
				case MS1826_SEAMLESS_MATRIX_SWITCHER:	 
					app_single_osd_config(u8_index);
					break;
				case MS1826_VIDEO_SPLITTER:
					app_splitter_osd_config();									 
					break;
				case MS1826_VIDEO_SPLICER:
					app_splicer_osd_config();
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
		}
	} 
	else
    {
		b_osd_menu = MS_FALSE;
		api_osd_clear(&g_st_osd_dev[0]);
		api_osd_font_colortab_set(&g_st_osd_dev[0],OSD_PATTERN_COLOR0);
		api_osd_window_enable(&g_st_osd_dev[0], OSD_WIN_DISPLAY, MS_FALSE);
		api_osd_window_enable(&g_st_osd_dev[0], OSD_WIN_HIGHLIGHT1, MS_FALSE);
		api_osd_window_enable(&g_st_osd_dev[0], OSD_WIN_HIGHLIGHT2, MS_FALSE);
		api_osd_trigger(&g_st_osd_dev[0]);
		api_osd_window_enable(&g_st_osd_dev[1], OSD_WIN_DISPLAY, MS_FALSE);
		api_osd_trigger(&g_st_osd_dev[1]);
		api_osd_window_enable(&g_st_osd_dev[2], OSD_WIN_DISPLAY, MS_FALSE);
		api_osd_trigger(&g_st_osd_dev[2]);
		api_osd_window_enable(&g_st_osd_dev[3], OSD_WIN_DISPLAY, MS_FALSE);
		api_osd_trigger(&g_st_osd_dev[3]);
		app_osd_clear();
    }
    g_u8OSDCount = 0;		
}

