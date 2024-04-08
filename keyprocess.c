#include "mculib_common.h"
#include "app.h"
#include "videoprocess.h"

BOOL b_power = MS_TRUE;
UINT8 g_u8_rotate = 0;
UINT8 g_u8_osd_menu_level = 0;/* 菜单级数 _by_zzw*/
UINT8 g_u8_osd_menu_y = 0;/* 菜单纵位 _by_zzw*/

extern UINT8 g_u8_splitter_audio_src;
extern BOOL b_osd_menu;
extern VOID app_osd_cfg(VOID);
extern VOID app_osd_menu_drawrect(UINT8 u8_level,UINT8 u8_y);
extern UINT8 g_u8_level[2];
extern UINT8 g_u8_bcsh_bri;	
extern UINT8 g_u8_bcsh_con;
extern UINT8 g_u8_app_audio_output[3][4];
extern UINT8 g_u8_time;
extern UINT8 g_u8_ratio;
extern UINT8 g_u8OSDCount;
extern UINT8 g_u8_audio_sw;

/* 红外遥控器按键码（u8_ir_key）映射到设备内部的按键码（u8_key） _by_zzw*/
UINT8 _remap_ir_key(UINT8 u8_ir_key)
{
	UINT8 u8_key = KEY_NONE;

	switch (u8_ir_key)
	{
				
		case IR_KEY_AUDIO_MUTE:
			u8_key = KEY_AUDIO_MUTE;/* 静音 _by_zzw*/
			break;
		case IR_KEY_POWER:
			u8_key = KEY_POWER;/* 电源 _by_zzw*/
			break;
		case IR_KEY_NUM_1:
			u8_key = KEY_NUM_1;/* 1 _by_zzw*/
			break;
		case IR_KEY_NUM_2:
			u8_key = KEY_NUM_2;/* 2 _by_zzw*/
			break;
		case IR_KEY_NUM_3:
			u8_key = KEY_NUM_3;/* 3 _by_zzw*/
			break;
		case IR_KEY_NUM_4:
			u8_key = KEY_NUM_4;/* 4 _by_zzw*/
			break;
		case IR_KEY_NUM_5:
			u8_key = KEY_NUM_5;/* 5 _by_zzw*/
			break;
		case IR_KEY_NUM_6:
			u8_key = KEY_NUM_6;/* 6 _by_zzw*/
			break;
		case IR_KEY_NUM_7:
			u8_key = KEY_NUM_7;/* 7 _by_zzw*/
			break;
		case IR_KEY_NUM_8:
			u8_key = KEY_NUM_8;/* 8 _by_zzw*/
			break;
		case IR_KEY_NUM_9:
			u8_key = KEY_NUM_9;/* 9 _by_zzw*/
			break;
		case IR_KEY_T:
			u8_key = KEY_RESOLU;/* 输出分辨率开关（T键） _by_zzw*/
			break;
		case IR_KEY_NUM_0:
			u8_key = KEY_DEFAULT;/* 0 _by_zzw*/
			break;
		case IR_KEY_SETUP:
			u8_key = KEY_MENU;/* 设置（菜单键盘） _by_zzw*/
			break;
		case IR_KEY_HOMEPAGE:
			u8_key = KEY_FUNCTION;/* 主页 _by_zzw*/
			break;
		case IR_KEY_UP:
			u8_key = KEY_UP;/* 上 _by_zzw*/
			break;
		case IR_KEY_MENU:
			u8_key = KEY_MODE;/* 模式（三杠键） _by_zzw*/
			break;
		case IR_KEY_LEFT:
			u8_key = KEY_LEFT;/* 左 _by_zzw*/
			break;
		case IR_KEY_OK:
			u8_key = KEY_OK;/* 确认 _by_zzw*/
			break;
		case IR_KEY_RIGHT:
			u8_key = KEY_RIGHT;/* 右 _by_zzw*/
			break;
		case IR_KEY_RETURN:
			u8_key = KEY_RETURN;/* 返回 _by_zzw*/
			break;
		case IR_KEY_DOWN:
			u8_key = KEY_DOWN;/* 下 _by_zzw*/
			break;
		case IR_KEY_SOURCE:
			u8_key = KEY_RX_VIDEO_SEL;/* 视频输入源 _by_zzw*/
			break;
		case IR_KEY_VOLUME_ADD:
			u8_key = KEY_AUDIO_ADD;/* 音量增加 _by_zzw*/
			break;
		case IR_KEY_VOLUME_SUB:
			u8_key = KEY_AUDIO_SUB;/* 音量减少 _by_zzw*/
			break;
		default:
			u8_key = 0xFF;
	}
	return u8_key;
}




/* 按键逻辑处理 _by_zzw*/
VOID _key_process_0(UINT8 u8_key)
{
	UINT8 u8_i = 0;
	if(b_power && b_osd_menu == MS_FALSE)
	{
		g_bOSDShown = MS_FALSE;
		sys_show_osd(g_bOSDShown, 0x0F);
	}
	switch (u8_key)
	{
		/* 按1 _by_zzw*/
		case KEY_NUM_1:
			switch (g_u8_display_mode)/* 匹配显示模式 _by_zzw*/
			{
				case MS1826_VIDEO_SPLITTER:
					if(Bypass_Tx_Status[0] == MS_FALSE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode ++;
						if(g_st_app_splitter_dev.u8_splitter_mode > MODE_SIGNLE_4)
							g_st_app_splitter_dev.u8_splitter_mode = MODE_SIGNLE_1;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					else
					{
						Matrix_Tx_Mapping[0] ++;
						if(Matrix_Tx_Mapping[0] > 3)
							Matrix_Tx_Mapping[0] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[0], 0);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.u8_tx_mirror = gst_splicer_app_dev.u8_tx_mirror ^ SPLICER_TX1_MIRROR;            
					app_splicer_mirror_config(HD_TX_A, gst_splicer_app_dev.u8_tx_mirror & SPLICER_TX1_MIRROR);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[0] ++;
					if(Matrix_Tx_Mapping[0] > 3)
						Matrix_Tx_Mapping[0] = 0;
					app_single_mode_rx_switch(Matrix_Tx_Mapping[0], 0);
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[0] ++;
					if(Matrix_Tx_Mapping[0] > 3)
						Matrix_Tx_Mapping[0] = 0;
					switch_bypass_channel(Matrix_Tx_Mapping[0], 0);
					break;
				default:
					break;
			}
			LOG("[INFO] HD TX1 SWITCH.");
			break;
			/* 按2 _by_zzw*/
		case KEY_NUM_2:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					if(Bypass_Tx_Status[1] == MS_FALSE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode ++;
						if(g_st_app_splitter_dev.u8_splitter_mode > MODE_SIGNLE_4)
							g_st_app_splitter_dev.u8_splitter_mode = MODE_SIGNLE_1;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					else
					{
						Matrix_Tx_Mapping[1] ++;
						if(Matrix_Tx_Mapping[1] > 3)
							Matrix_Tx_Mapping[1] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[1], 1);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.u8_tx_mirror = gst_splicer_app_dev.u8_tx_mirror ^ SPLICER_TX2_MIRROR;            
					app_splicer_mirror_config(HD_TX_B, gst_splicer_app_dev.u8_tx_mirror & SPLICER_TX2_MIRROR);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[1] ++;
					if(Matrix_Tx_Mapping[1] > 3)
						Matrix_Tx_Mapping[1] = 0;
					app_single_mode_rx_switch(Matrix_Tx_Mapping[1], 1);
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[1] ++;
					if(Matrix_Tx_Mapping[1] > 3)
						Matrix_Tx_Mapping[1] = 0;
					switch_bypass_channel(Matrix_Tx_Mapping[1], 1);
					break;
				default:
					break;
			}
			LOG("[INFO] HD TX2 SWITCH.");
			break;
			/* 按3 _by_zzw*/
		case KEY_NUM_3:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					if(Bypass_Tx_Status[2] == MS_FALSE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode ++;
						if(g_st_app_splitter_dev.u8_splitter_mode > MODE_SIGNLE_4)
							g_st_app_splitter_dev.u8_splitter_mode = MODE_SIGNLE_1;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					else
					{
						Matrix_Tx_Mapping[2] ++;
						if(Matrix_Tx_Mapping[2] > 3)
							Matrix_Tx_Mapping[2] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[2], 2);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_splicer_mode < SPLICER_MODE_1X3)
					{
						Matrix_Tx_Mapping[2] ++;
						if(Matrix_Tx_Mapping[2] > 3)
							Matrix_Tx_Mapping[2] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[2], 2);
					}
					else
					{
						gst_splicer_app_dev.u8_tx_mirror = gst_splicer_app_dev.u8_tx_mirror ^ SPLICER_TX3_MIRROR;            
						app_splicer_mirror_config(HD_TX_C, gst_splicer_app_dev.u8_tx_mirror & SPLICER_TX3_MIRROR);
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[2] ++;
					if(Matrix_Tx_Mapping[2] > 3)
						Matrix_Tx_Mapping[2] = 0;
					app_single_mode_rx_switch(Matrix_Tx_Mapping[2], 2);
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[2] ++;
					if(Matrix_Tx_Mapping[2] > 3)
						Matrix_Tx_Mapping[2] = 0;
					switch_bypass_channel(Matrix_Tx_Mapping[2], 2);
					break;
				default:
					break;
			}
			LOG("[INFO] HD TX3 SWITCH.");
			break;
			/* 按4 _by_zzw*/
		case KEY_NUM_4:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					if(Bypass_Tx_Status[3] == MS_FALSE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode ++;
						if(g_st_app_splitter_dev.u8_splitter_mode > MODE_SIGNLE_4)
							g_st_app_splitter_dev.u8_splitter_mode = MODE_SIGNLE_1;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					else
					{
						Matrix_Tx_Mapping[3] ++;
						if(Matrix_Tx_Mapping[3] > 3)
							Matrix_Tx_Mapping[3] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[3], 3);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_splicer_mode < SPLICER_MODE_2X2)
					{
						Matrix_Tx_Mapping[3] ++;
						if(Matrix_Tx_Mapping[3] > 3)
							Matrix_Tx_Mapping[3] = 0;
						switch_bypass_channel(Matrix_Tx_Mapping[3], 3);
					}
					else
					{
						gst_splicer_app_dev.u8_tx_mirror = gst_splicer_app_dev.u8_tx_mirror ^ SPLICER_TX4_MIRROR;            
						app_splicer_mirror_config(HD_TX_D, gst_splicer_app_dev.u8_tx_mirror & SPLICER_TX4_MIRROR);
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[3] ++;
					if(Matrix_Tx_Mapping[3] > 3)
						Matrix_Tx_Mapping[3] = 0;
					app_single_mode_rx_switch(Matrix_Tx_Mapping[3], 3);
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					Matrix_Tx_Mapping[3] ++;
					if(Matrix_Tx_Mapping[3] > 3)
						Matrix_Tx_Mapping[3] = 0;
					switch_bypass_channel(Matrix_Tx_Mapping[3], 3);
					break;
				default:
					break;   
			}
			LOG("[INFO] HD TX4 SWITCH.");
			break;
			/* 按主页 _by_zzw*/
		case KEY_FUNCTION:
			_video_mute(HD_RX_A, MS_TRUE);
			_video_mute(HD_RX_B, MS_TRUE);
			_video_mute(HD_RX_C, MS_TRUE);
			_video_mute(HD_RX_D, MS_TRUE);
			g_bOSDShown = MS_FALSE;
			sys_show_osd(g_bOSDShown, 0x0F);
            g_u8_display_mode_bak = g_u8_display_mode;
			g_u8_display_mode ++;
			if(g_u8_display_mode > MS1826_BYPASS_MATRIX_SWITCHER)
				g_u8_display_mode = MS1826_VIDEO_SPLITTER;              
     	                                
            switch (g_u8_display_mode)
            {
                case MS1826_VIDEO_SPLITTER:                   
                    g_st_app_splitter_dev.b_splitter_sw = MS_FALSE;
                    app_splitter_mode_config(&g_st_app_splitter_dev);
                    for(u8_i = 0; u8_i < 4; u8_i ++)
                    {
                        if(g_u8_HD_TX_PROT[g_st_app_splitter_dev.u8_dst_sel] != g_u8_HD_TX_PROT[u8_i])
                        {
                            switch_bypass_channel(Matrix_Tx_Mapping[u8_i], u8_i);
                        }
                    }
					LOG("[INFO] MS1826 function is VIDEO_SPLITTER.");
                    break;
                case MS1826_VIDEO_SPLICER:
                    gst_splicer_app_dev.b_splicer_sw = MS_FALSE;
                    gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
                    app_splicer_mode_config(&gst_splicer_app_dev);
                    switch(gst_splicer_app_dev.u8_splicer_mode)
                    {
                        case SPLICER_MODE_1X2: 
                        case SPLICER_MODE_2X1: 
                            switch_bypass_channel(Matrix_Tx_Mapping[2], 2);
                            switch_bypass_channel(Matrix_Tx_Mapping[3], 3);
                            break;
                        case SPLICER_MODE_1X3:    
                        case SPLICER_MODE_3X1:
                            switch_bypass_channel(Matrix_Tx_Mapping[3], 3);
                            break;
                    }
					LOG("[INFO] MS1826 function is VIDEO_SPLICER.");
                    break;
                case MS1826_SEAMLESS_MATRIX_SWITCHER:
                    app_single_mode_config();
					LOG("[INFO] MS1826 function is SEAMLESS_MATRIX_SWITCHER.");
                    break;
                case MS1826_BYPASS_MATRIX_SWITCHER:
                    for(u8_i = 0; u8_i < 4; u8_i ++)
                    {
                        if(Bypass_Tx_Status[u8_i] == 0)
                        {
                            Bypass_Tx_Status[u8_i] = 1;
                            switch_bypass_channel(Matrix_Tx_Mapping[u8_i], u8_i);
                        }
                    }
					LOG("[INFO] MS1826 function is BYPASS_MATRIX_SWITCHER.");
                    break;
                default:
                    break;
            }   
            api_hd_tx_output_en(&g_st_hd_tx[HD_TX_A], MS_TRUE);
            api_hd_tx_output_en(&g_st_hd_tx[HD_TX_B], MS_TRUE);
            api_hd_tx_output_en(&g_st_hd_tx[HD_TX_C], MS_TRUE);
            api_hd_tx_output_en(&g_st_hd_tx[HD_TX_D], MS_TRUE);
            LOG("[INFO] Work mode switch.");
			break;
			/* 分割模式（三杠键） _by_zzw*/
		case KEY_MODE:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
					g_st_app_splitter_dev.u8_splitter_mode ++;
					if(g_st_app_splitter_dev.u8_splitter_mode > MODE_FOUR_EQUALIZE)
						g_st_app_splitter_dev.u8_splitter_mode = MODE_SIGNLE_1;
					app_splitter_mode_config(&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
					gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
					gst_splicer_app_dev.u8_splicer_mode ++;
					if(gst_splicer_app_dev.u8_splicer_mode > SPLICER_MODE_4X1)
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_1X2;
										
					app_splicer_mode_config(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] SPLICER MODE SWITCH.");
			break;
			/* 应该是静音键 _by_zzw*/
		case KEY_AUDIO:
		/* 按音量增加 _by_zzw*/
		case KEY_AUDIO_ADD:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					app_splitter_mode_audio_src();
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.u8_src_sel ++;
					if(gst_splicer_app_dev.u8_src_sel > 3)
						gst_splicer_app_dev.u8_src_sel = 0;
					app_splicer_rx_smeless_switch(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] AUDIO CHANNAL ADD.");
			break;
			/* 输出分辨率选择（T键） _by_zzw*/
		case KEY_RESOLU:
			g_bOSDShown = MS_FALSE;
			sys_show_osd(g_bOSDShown, 0x0F);
		
			g_u8_output_index ++;
			if(g_u8_output_index > 2)
				g_u8_output_index = 0;
			
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					app_splitter_output_resolution(g_arrOutputTable[g_u8_output_index],&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					app_splicer_output_resolution(g_arrOutputTable[g_u8_output_index], &gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					app_single_output_resolution(g_arrOutputTable[g_u8_output_index]);
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:         
					break;
				default:
					break;
			}
			LOG("[INFO] OUTPUT RESOLUTION SWITCH.");
			break;
			/* 电源键盘（红色） _by_zzw*/
		case KEY_POWER:
			b_power = !b_power;
			if(b_power)
			{
				PORT_SetBits(POWER_ON_OFF_PORT, POWER_ON_OFF_PIN);
				HD_POWERON_LED(TRUE);
				HD_POWEROFF_LED(FALSE);
				LOG("[INFO] POWER ON.");
				PORT_ResetBits(RESET_PORT, RESET_PIN);
				Ddl_Delay1ms(2);
				PORT_SetBits(RESET_PORT, RESET_PIN);
				Ddl_Delay1ms(1);
				video_process_init();
			}
			else
			{
				api_hd_tx_audio_mute(&g_st_hd_tx[0], MS_TRUE);
				api_hd_tx_audio_mute(&g_st_hd_tx[1], MS_TRUE);
				api_hd_tx_audio_mute(&g_st_hd_tx[2], MS_TRUE);
				api_hd_tx_audio_mute(&g_st_hd_tx[3], MS_TRUE);
				app_digital_audio_cfg(0, g_u8_i2s_idx[0], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_i2s_idx[1], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_i2s_idx[2], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_i2s_idx[3], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_spdif_idx[0], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_spdif_idx[1], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_spdif_idx[2], MS_FALSE);
				app_digital_audio_cfg(0, g_u8_spdif_idx[3], MS_FALSE);
				
				PORT_ResetBits(POWER_ON_OFF_PORT, POWER_ON_OFF_PIN);
				HD_POWERON_LED(FALSE);
				HD_POWEROFF_LED(TRUE);
				HD_RX_LED1(FALSE);
				HD_RX_LED2(FALSE);
				HD_RX_LED3(FALSE);
				HD_RX_LED4(FALSE);
				HD_TX_LED1(FALSE);
				HD_TX_LED2(FALSE);
				HD_TX_LED3(FALSE);
				HD_TX_LED4(FALSE);
				LOG("[INFO] POWER OFF.");
			}
			break;
			/* 上 _by_zzw*/
		case KEY_UP:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 0)
				{
					g_u8_osd_menu_y--;
					if(g_u8_osd_menu_y > 6)
						g_u8_osd_menu_y = 6;
					app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
					app_osd_trigger(OSD_1);
				}
				else if(g_u8_osd_menu_level == 1)
				{
					if(g_u8_level[0] == 0)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 1)
							g_u8_osd_menu_y = 1;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 1)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 4;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 2)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 2)
							g_u8_osd_menu_y = 2;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 3)
					{
						;
					}
					else if(g_u8_level[0] == 4)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 4;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 5)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 4;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 6)
					{
						g_u8_osd_menu_y--;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 4;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
				}
			}
			else
			{ 
				switch (g_u8_display_mode)
				{
					case MS1826_VIDEO_SPLITTER:
						break;
					case MS1826_VIDEO_SPLICER:
						if(u8_v_border_fusion != 0)
						{
						u8_v_border_fusion --;
								gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
							app_splicer_mode_config(&gst_splicer_app_dev);
						}
						break;
					case MS1826_SEAMLESS_MATRIX_SWITCHER:
						break;
					case MS1826_BYPASS_MATRIX_SWITCHER:         
						break;
					default:
						break;
				}
			}
			LOG("[INFO] UP.");
			break;
			/* 下键 _by_zzw*/
		case KEY_DOWN:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 0)
				{
					g_u8_osd_menu_y++;
					if(g_u8_osd_menu_y > 6)
						g_u8_osd_menu_y = 0;
					app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
					app_osd_trigger(OSD_1);
				}
				else if(g_u8_osd_menu_level == 1)
				{
					if(g_u8_level[0] == 0)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 1)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 1)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 2)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 2)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 3)
					{
						;
					}
					else if(g_u8_level[0] == 4)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 5)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
					else if(g_u8_level[0] == 6)
					{
						g_u8_osd_menu_y++;
						if(g_u8_osd_menu_y > 4)
							g_u8_osd_menu_y = 0;
						app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
						app_osd_trigger(OSD_1);
					}
				}
			}
			else
			{
				switch (g_u8_display_mode)
				{
					case MS1826_VIDEO_SPLITTER:
						break;
					case MS1826_VIDEO_SPLICER:
						if(u8_v_border_fusion != 255)
						{
							u8_v_border_fusion ++;
							gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
							app_splicer_mode_config(&gst_splicer_app_dev);
						}
						break;
					case MS1826_SEAMLESS_MATRIX_SWITCHER:
						break;
					case MS1826_BYPASS_MATRIX_SWITCHER:         
						break;
					default:
						break;
				}
			}
			LOG("[INFO] DOWN.");
			break;
			/* 左键 _by_zzw*/
		case KEY_LEFT:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 1)
				{
					if(g_u8_level[0] == 0)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_bcsh_bri--;
							if(g_u8_bcsh_bri > 100)
								g_u8_bcsh_bri = 0;
						}
						else if(g_u8_osd_menu_y == 1)
						{
							g_u8_bcsh_con--;
							if(g_u8_bcsh_con > 100)
								g_u8_bcsh_con = 0;
						}

						app_bcsh_adjust(VEHC_VOPSS_1);
						app_bcsh_adjust(VEHC_VOPSS_2);
						app_bcsh_adjust(VEHC_VOPSS_3);
						app_bcsh_adjust(VEHC_VOPSS_4);
					}
					else if(g_u8_level[0] == 1)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_audio_sw--;
							if(g_u8_audio_sw > 2)
								g_u8_audio_sw = 2;
						}
						else
						{
							g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] --;
							
							if(g_u8_audio_sw == 0)
							{
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] > 11)
									g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] = 11;
								
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] < 4)
									app_hd_input_hd_output_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_osd_menu_y - 1);
								else if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] >= 4 && g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] < 8)
									app_digital_input_hd_output_cfg(g_u8_i2s_audio_idx[g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] - 4],g_u8_osd_menu_y - 1);
								else if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] >= 8 )
									app_digital_input_hd_output_cfg(g_u8_spdif_audio_idx[g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] - 8],g_u8_osd_menu_y - 1);
							}
							else
							{
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] > 3)
									g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] = 3;
								
								if(g_u8_audio_sw == 1)
									app_digital_audio_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_i2s_idx[g_u8_osd_menu_y - 1],MS_TRUE);
								else if(g_u8_audio_sw == 2)
									app_digital_audio_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_spdif_idx[g_u8_osd_menu_y - 1],MS_TRUE);
							}
						}
					}
					else if(g_u8_level[0] == 2)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_time -- ;
							if(g_u8_time > 2)
								g_u8_time = 2;
						}
						else if(g_u8_osd_menu_y == 1)
						{
							g_u8_ratio++;
							if(g_u8_ratio > 100)
								g_u8_ratio = 100;
						}
					}
					app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
					app_osd_trigger(OSD_1);
				}
			}
			else
			{
				switch (g_u8_display_mode)
				{
					case MS1826_VIDEO_SPLITTER:
						break;
					case MS1826_VIDEO_SPLICER:
						if(u8_h_border_fusion != 0)
						{
							u8_h_border_fusion --;
							gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
							app_splicer_mode_config(&gst_splicer_app_dev);
						}
						break;
					case MS1826_SEAMLESS_MATRIX_SWITCHER:
						break;
					case MS1826_BYPASS_MATRIX_SWITCHER:         
						break;
					default:
						break;
				}
			}
			LOG("[INFO] LEFT.");
			break;
			/* 右键 _by_zzw*/
		case KEY_RIGHT:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 1)
				{
					if(g_u8_level[0] == 0)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_bcsh_bri++;
							if(g_u8_bcsh_bri > 100)
								g_u8_bcsh_bri = 100;
						}
						else if(g_u8_osd_menu_y == 1)
						{
							g_u8_bcsh_con++;
							if(g_u8_bcsh_con > 100)
								g_u8_bcsh_con = 100;
						}

						app_bcsh_adjust(VEHC_VOPSS_1);
						app_bcsh_adjust(VEHC_VOPSS_2);
						app_bcsh_adjust(VEHC_VOPSS_3);
						app_bcsh_adjust(VEHC_VOPSS_4);
					}
					else if(g_u8_level[0] == 1)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_audio_sw++;
							if(g_u8_audio_sw > 2)
								g_u8_audio_sw = 0;
						}
						else
						{
							g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] ++;
							
							if(g_u8_audio_sw == 0)
							{
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] > 11)
									g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] = 0;
								
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] < 4)
									app_hd_input_hd_output_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_osd_menu_y - 1);
								else if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] >= 4 && g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] < 8)
									app_digital_input_hd_output_cfg(g_u8_i2s_audio_idx[g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] - 4],g_u8_osd_menu_y - 1);
								else if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] >= 8 )
									app_digital_input_hd_output_cfg(g_u8_spdif_audio_idx[g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] - 8],g_u8_osd_menu_y - 1);
							}
							else
							{
								if(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] > 3)
									g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1] = 0;
								
								if(g_u8_audio_sw == 1)
									app_digital_audio_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_i2s_idx[g_u8_osd_menu_y - 1],MS_TRUE);
								else if(g_u8_audio_sw == 2)
									app_digital_audio_cfg(g_u8_app_audio_output[g_u8_audio_sw][g_u8_osd_menu_y - 1],g_u8_spdif_idx[g_u8_osd_menu_y - 1],MS_TRUE);
							}
						}
					}
					else if(g_u8_level[0] == 2)
					{
						if(g_u8_osd_menu_y == 0)
						{
							g_u8_time ++ ;
							if(g_u8_time > 2)
								g_u8_time = 0;
						}
						else if(g_u8_osd_menu_y == 1)
						{
							g_u8_ratio--;
							if(g_u8_ratio < 20)
								g_u8_ratio = 20;
						}
					}
					app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_osd_menu_y);
					app_osd_trigger(OSD_1);
				}
			}
			else
			{
				switch (g_u8_display_mode)
				{
					case MS1826_VIDEO_SPLITTER:
						break;
					case MS1826_VIDEO_SPLICER:
						if(u8_h_border_fusion != 255)
						{
							u8_h_border_fusion ++;
							gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
							app_splicer_mode_config(&gst_splicer_app_dev);
						}
						break;
					case MS1826_SEAMLESS_MATRIX_SWITCHER:
						break;
					case MS1826_BYPASS_MATRIX_SWITCHER:         
						break;
						default:
						break;
				}
			}
			LOG("[INFO] RIGHT.");
			break;
			/* ok键 _by_zzw*/
		case KEY_OK:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 0 && g_u8_osd_menu_y < 6)
				{
					g_u8_osd_menu_level = 1;
					g_u8_osd_menu_y = 0;
					app_osd_menu_drawrect(g_u8_osd_menu_level,0);
					app_osd_trigger(OSD_1);
				}
				if(g_u8_osd_menu_level == 1)
				{
					LOG1("！！菜单级数！！",g_u8_osd_menu_level);
					LOG1("g_u8_osd_menu_y纵聚焦:",g_u8_osd_menu_y);
					if(g_u8_osd_menu_y == 2 && g_u8_level[0] == 2)
					{
						mculib_user_reset();
						NVIC_SystemReset();/* 不知道是什么函数 应该没用 _by_zzw*/
					}
				}
			}
			else
			{
				switch (g_u8_display_mode)
				{
					case MS1826_VIDEO_SPLITTER:
						if(g_st_app_splitter_dev.u8_splitter_mode < MODE_DUAL_UP1_DOWN2)
						{
							g_u8_rotate++;
							if(g_u8_rotate > 3)
								g_u8_rotate = 0;
							app_splitter_rx_rotate(g_st_app_splitter_dev.u8_splitter_mode, g_u8_rotate);
						}
						break;
					case MS1826_VIDEO_SPLICER:
						break;
					case MS1826_SEAMLESS_MATRIX_SWITCHER:
						break;
					case MS1826_BYPASS_MATRIX_SWITCHER:
						break;
					default:
						break;
				}
			}
			LOG("[INFO] OK.");
			break;
			/* 返回键 _by_zzw*/
		case KEY_RETURN:
			if(b_osd_menu == MS_TRUE)
			{
				if(g_u8_osd_menu_level == 1)
				{
					g_u8_osd_menu_y = g_u8_level[0];
					g_u8_osd_menu_level = 0;
					app_osd_menu_drawrect(g_u8_osd_menu_level,g_u8_level[0]);
					app_osd_trigger(OSD_1);
				}
				else if(g_u8_osd_menu_level == 0)
				{
					b_osd_menu = MS_FALSE;
//					api_osd_window_enable(&g_st_osd_dev[0], OSD_WIN_DISPLAY, MS_FALSE);
//					api_osd_trigger(&g_st_osd_dev[0]);
					g_bOSDShown = MS_FALSE;
					sys_show_osd(g_bOSDShown, 0x0F);
					g_bOSDShown = MS_TRUE;
					sys_show_osd(g_bOSDShown, 0x0F);
				}
			}
		
			LOG("[INFO] RETURN.");
			break;
			/* 菜单键（设置键） _by_zzw*/
		case KEY_MENU:
			if(b_osd_menu == MS_FALSE && g_u8_display_mode != MS1826_BYPASS_MATRIX_SWITCHER)
			{
				LOG1("当前模式：",g_u8_display_mode);
				b_osd_menu = MS_TRUE;
				g_u8_osd_menu_level = 0;
				g_u8_osd_menu_y = 0;
				app_osd_cfg();
				app_osd_menu_drawrect(0,0);
				app_osd_trigger(OSD_1);
			}else{
				LOG1("菜单未打开，当前模式：",g_u8_display_mode)
			}
			LOG("[INFO] MENU.");
			break;

		case KEY_RX_VIDEO_SEL:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.u8_src_sel ++;
					if(gst_splicer_app_dev.u8_src_sel > 3)
						gst_splicer_app_dev.u8_src_sel = 0;
					app_splicer_rx_smeless_switch(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] RX VIDEO SEL.");
			break;
			/* 静音键 _by_zzw*/
		case KEY_AUDIO_MUTE:
			b_audio_mute = !b_audio_mute;
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					app_tx_audio_cfg(g_u8_splitter_audio_src, g_st_app_splitter_dev.u8_dst_sel);
					app_tx_audio_cfg(Matrix_Tx_Mapping[1], 1);
					app_tx_audio_cfg(Matrix_Tx_Mapping[2], 2);
					app_tx_audio_cfg(Matrix_Tx_Mapping[3], 3);
					break;
				case MS1826_VIDEO_SPLICER:
					app_tx_audio_cfg(gst_splicer_app_dev.u8_src_sel, 0);
					app_tx_audio_cfg(gst_splicer_app_dev.u8_src_sel, 1);
					switch(gst_splicer_app_dev.u8_splicer_mode)
					{
						case SPLICER_MODE_1X1:
						case SPLICER_MODE_2X2: 
						case SPLICER_MODE_1X4:	
						case SPLICER_MODE_4X1:
							app_tx_audio_cfg(gst_splicer_app_dev.u8_src_sel, 2);
							app_tx_audio_cfg(gst_splicer_app_dev.u8_src_sel, 3);
							break;
						case SPLICER_MODE_1X2:
						case SPLICER_MODE_2X1:
							app_tx_audio_cfg(Matrix_Tx_Mapping[2], 2);
							app_tx_audio_cfg(Matrix_Tx_Mapping[3], 3);
							break;
						case SPLICER_MODE_1X3:
						case SPLICER_MODE_3X1:
							app_tx_audio_cfg(Matrix_Tx_Mapping[3], 3);
							break;
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
				case MS1826_BYPASS_MATRIX_SWITCHER:
					app_tx_audio_cfg(Matrix_Tx_Mapping[0], 0);
					app_tx_audio_cfg(Matrix_Tx_Mapping[1], 1);
					app_tx_audio_cfg(Matrix_Tx_Mapping[2], 2);
					app_tx_audio_cfg(Matrix_Tx_Mapping[3], 3);
					break;
				default:
					break;
			}
			LOG("[INFO] AUDIO MUTE.");
			break;
			/* 0键 _by_zzw*/
		case KEY_DEFAULT:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					if(g_st_app_splitter_dev.u8_splitter_mode != MODE_FOUR_EQUALIZE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode = MODE_FOUR_EQUALIZE;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_2X2)
					{
						gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
						gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_2X2;			
						app_splicer_mode_config(&gst_splicer_app_dev);
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] Mode default.");
			break;
		case KEY_NUM_5:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
					g_st_app_splitter_dev.u8_splitter_mode ++;
					if(g_st_app_splitter_dev.u8_splitter_mode > MODE_DUAL_UP3_DOWN4_4_3 || g_st_app_splitter_dev.u8_splitter_mode < MODE_DUAL_UP1_DOWN2)
						g_st_app_splitter_dev.u8_splitter_mode = MODE_DUAL_UP1_DOWN2;
					app_splitter_mode_config(&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_1X1)
					{
						gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
						gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_1X1;			
						app_splicer_mode_config(&gst_splicer_app_dev);
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] KEY_NUM_5.");
			break;
		case KEY_NUM_6:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
					g_st_app_splitter_dev.u8_splitter_mode ++;
					if(g_st_app_splitter_dev.u8_splitter_mode > MODE_DUAL_LEFT3_RIGHT4_4_3 || g_st_app_splitter_dev.u8_splitter_mode < MODE_DUAL_LEFT1_RIGHT2)
						g_st_app_splitter_dev.u8_splitter_mode = MODE_DUAL_LEFT1_RIGHT2;
					app_splitter_mode_config(&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
					gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_1X2)
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_1X2;			
					}
					else
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_2X1;	
					}
					app_splicer_mode_config(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] KEY_NUM_6.");
			break;
		case KEY_NUM_7:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
					g_st_app_splitter_dev.u8_splitter_mode ++;
					if(g_st_app_splitter_dev.u8_splitter_mode > MODE_PIP_43_1_3 || g_st_app_splitter_dev.u8_splitter_mode < MODE_PIP_12_1_3)
						g_st_app_splitter_dev.u8_splitter_mode = MODE_PIP_12_1_3;
					app_splitter_mode_config(&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
					gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_1X3)
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_1X3;			
					}
					else
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_3X1;	
					}
					app_splicer_mode_config(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] KEY_NUM_7.");
			break;
		case KEY_NUM_8:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					if(g_st_app_splitter_dev.u8_splitter_mode != MODE_THREE_EQUALIZE)
					{
						g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
						g_st_app_splitter_dev.u8_splitter_mode = MODE_THREE_EQUALIZE;
						app_splitter_mode_config(&g_st_app_splitter_dev);
					}
					break;
				case MS1826_VIDEO_SPLICER:
					gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
					gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_1X4)
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_1X4;			
					}
					else
					{
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_4X1;	
					}
					app_splicer_mode_config(&gst_splicer_app_dev);
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] KEY_NUM_8.");
			break;
		case KEY_NUM_9:
			switch (g_u8_display_mode)
			{
				case MS1826_VIDEO_SPLITTER:
					g_st_app_splitter_dev.b_splitter_sw = MS_TRUE;
					g_st_app_splitter_dev.u8_splitter_mode ++;
					if(g_st_app_splitter_dev.u8_splitter_mode > MODE_FOUR_EQUALIZE || g_st_app_splitter_dev.u8_splitter_mode < MODE_FOUR_MAIN1_SUB234)
						g_st_app_splitter_dev.u8_splitter_mode = MODE_FOUR_MAIN1_SUB234;
					app_splitter_mode_config(&g_st_app_splitter_dev);
					break;
				case MS1826_VIDEO_SPLICER:
					if(gst_splicer_app_dev.u8_splicer_mode != SPLICER_MODE_2X2)
					{
						gst_splicer_app_dev.b_splicer_sw = MS_TRUE;
						gst_splicer_app_dev.u8_tx_mirror = MS_FALSE;
						gst_splicer_app_dev.u8_splicer_mode = SPLICER_MODE_2X2;			
						app_splicer_mode_config(&gst_splicer_app_dev);
					}
					break;
				case MS1826_SEAMLESS_MATRIX_SWITCHER:
					break;
				case MS1826_BYPASS_MATRIX_SWITCHER:
					break;
				default:
					break;
			}
			LOG("[INFO] KEY_NUM_9.");
			break;
		default:
			break;
	}



	if(b_power)
	{
		if(g_st_hd_rx[0].u8_rx_state == RX_STATE_RUNNING)
			_video_mute(0, FALSE);
		else
			_video_mute(0, TRUE);
		if(g_st_hd_rx[1].u8_rx_state == RX_STATE_RUNNING)
			_video_mute(1, FALSE);
		else
			_video_mute(1, TRUE);
		if(g_st_hd_rx[2].u8_rx_state == RX_STATE_RUNNING)
			_video_mute(2, FALSE);
		else
			_video_mute(2, TRUE);
		if(g_st_hd_rx[3].u8_rx_state == RX_STATE_RUNNING)
			_video_mute(3, FALSE);
		else
			_video_mute(3, TRUE);
	}
	mculib_user_save();
}





/* 处理红外遥控器按键事件 _by_zzw*/
VOID ir_key_service(VOID)
{
	UINT8 u8_ir_remap = 0;
	UINT8 u8_key = mculib_ir_detect();
	if (u8_key == IR_KEY_OTHER)
	{
		return;
	}
	u8_ir_remap = _remap_ir_key(u8_key);
	LOG1("Ir_u8_key = ", u8_key);
	_key_process_0(u8_ir_remap);

}


/* 处理按键事件 _by_zzw*/
VOID key_service(VOID)
{
	UINT8 u8key = mculib_key_detect();
	if (u8key != KEY_NONE)
	{
		LOG2("[KEY] u8key = ", u8key);
		_key_process_0(u8key);
	}
}
