#include "videoprocess.h"
#include "app_menuprocess.h"

extern UINT8 g_u8_splitter_audio_src;
// APP_OSD_DRAWRECT_T g_st_osd_DrawRect;
BOOL b_osd_menu = MS_FALSE;														/* 菜单的状态 _by_zzw*/
UINT8 g_u8_bcsh_bri = 50;														/* Brightness（亮度） _by_zzw*/
UINT8 g_u8_bcsh_con = 50;														/* Contrast（对比度） _by_zzw*/
UINT8 g_u8_level[2] = {0, 0};													/* 菜单的级数 _by_zzw*/
UINT8 g_u8_app_audio_output[3][4] = {{0, 1, 2, 3}, {0, 1, 2, 3}, {0, 1, 2, 3}}; /* 音频输出 _by_zzw*/
UINT8 g_u8_time = 0;
UINT8 g_u8_ratio = 100;	 /* （透明度）比例 _by_zzw*/
UINT8 g_u8_audio_sw = 0; /* 音频选项状态 _by_zzw*/

/* 文字 _by_zzw*/
VOID app_osd_show_fonts(APP_OSD_SHOWFONTS_T *pst_osd_showfonts)
{
	OSD_DEV_T pst_osd_dev;	 /* OSD模块配置信息参数 _by_zzw*/
	OSD_FONT_T pst_osd_font; /* OSD Fonts显示参数 _by_zzw*/

	pst_osd_dev.u8_osd_idx = pst_osd_showfonts->u8_osd_idx;

	pst_osd_font.u8_x = pst_osd_showfonts->u8_x;
	pst_osd_font.u8_y = pst_osd_showfonts->u8_y;
	pst_osd_font.u16_length = pst_osd_showfonts->u8_len; // sizeof(pst_osd_showfonts->u16_idx)/sizeof(pst_osd_showfonts->u16_idx[0]);
	pst_osd_font.pu16_idx = (UINT16 *)(pst_osd_showfonts->u16_idx);

	api_osd_font_colortab_set(&pst_osd_dev, pst_osd_showfonts->osd_font_colortab);
	api_osd_show_index(&pst_osd_dev, &pst_osd_font);
}

// （背景）画框
VOID app_osd_DrawRect(APP_OSD_DRAWRECT_T *pst_osd_DrawRect, BOOL b_osd_en)
{
	OSD_DEV_T st_osd_dev;		  /* OSD模块配置信息参数 _by_zzw*/
	OSD_WINATTR_T st_osd_winattr; /* OSD window配置参数 _by_zzw*/
	OSD_WINSIZE_T st_osd_winsize; /* OSD 窗口位置信息 _by_zzw*/
	OSD_TRANS_T st_osd_trans;	  /* OSD 透明度设置 _by_zzw*/
	st_osd_dev.u8_osd_idx = pst_osd_DrawRect->u8_osd_idx;
	st_osd_winsize.u8_osd_win = pst_osd_DrawRect->u8_osd_win;
	st_osd_winsize.u16_w = pst_osd_DrawRect->u16_w;
	st_osd_winsize.u16_h = pst_osd_DrawRect->u16_h;
	st_osd_winsize.u16_x = pst_osd_DrawRect->u16_x;
	st_osd_winsize.u16_y = pst_osd_DrawRect->u16_y;
	api_osd_window_position_set(&st_osd_dev, &st_osd_winsize);

	st_osd_winattr.u8_osd_win = pst_osd_DrawRect->u8_osd_win;
	st_osd_winattr.u8_rounded_en = MS_TRUE;
	st_osd_winattr.u8_bd_en = MS_FALSE;
	st_osd_winattr.u8_rounded_radius = 5;
	st_osd_winattr.st_win_fillcolor.b_fill_color_en = MS_TRUE;
	st_osd_winattr.st_win_fillcolor.u8_win_id = pst_osd_DrawRect->u8_osd_win;
	st_osd_winattr.st_win_fillcolor.st_fill_start_color.u8_R = pst_osd_DrawRect->st_fill_start_color.u8_R;
	st_osd_winattr.st_win_fillcolor.st_fill_start_color.u8_G = pst_osd_DrawRect->st_fill_start_color.u8_G;
	st_osd_winattr.st_win_fillcolor.st_fill_start_color.u8_B = pst_osd_DrawRect->st_fill_start_color.u8_B;
	st_osd_winattr.st_win_fillcolor.u8_fill_color_type = OSD_FILLCOLOR_TYPE0;
	st_osd_winattr.u8_attr_rp_en = MS_FALSE;
	api_osd_window_attr_set(&st_osd_dev, &st_osd_winattr);

	st_osd_trans.u8_alpha_ratio = g_u8_ratio * 2 + 55;
	st_osd_trans.u8_trans_mode = 0;
	st_osd_trans.b_trans_en = MS_TRUE;
	api_osd_trans_set(&st_osd_dev, &st_osd_trans);

	api_osd_colorspace_set(&st_osd_dev, OSD_COLORSPACE_RGB);
	api_osd_window_enable(&st_osd_dev, pst_osd_DrawRect->u8_osd_win, b_osd_en);
}

/* 菜单显示的中文字符的编号 _by_zzw*/
UINT16 STR_OsdMenu_CN[][7] = {
	0, 105, 106, 15, 107, 108, 109, // 亮度/对比度
	0, 0, 101, 102, 103, 104, 0,	// 音频选择
	0, 0, 110, 111, 112, 113, 0,	// 系统设置
	0, 0, 151, 0, 0, 152, 0,		// 信息
	0, 96, 0, 0, 98, 0,	34,	/* 输入图像大小 _zzw*/
	0, 96, 0, 0, 98, 0,	48,	/* 输入图像位置 _zzw*/
	0, 96, 0, 0, 98, 0,	35,	/* 输入图像裁剪 _zzw*/
	
	// 0, 73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 83, 73, 90, 69,/* 输入图像大小 _zzw*/
	// 0, 73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 80, 79, 83, 73, 84, 73, 79, 78,/* 输入图像位置 _zzw*/
	// 0, 79, 85, 84, 80, 85, 84, 0, 82, 69, 83, 79, 76, 85, 84, 73, 79, 78,/* 输出分辨率 _zzw*/
	// 0, 83, 69, 76, 69, 67, 84, 0, 77, 79, 86, 69, 0, 76, 69, 78, 71, 84, 72,/* 选择移动长度 _zzw*/
	// 0, 73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 67, 82, 79, 80, 80, 73, 78, 71,/* 输入图像裁剪 _zzw*/
	// 0, 83, 65, 86, 69, 0, 83, 69, 84, 84, 73, 78, 71, 83,/* 保存设置 _zzw*/
	// 0, 82, 69, 65, 68, 0, 83, 69, 84, 84, 73, 78, 71, 83,/* 读取设置 _zzw*/
	// 0, 82, 69, 83, 69, 84, 0, 83, 69, 84, 84, 73, 78, 71, 83,/* 重置设置 _zzw*/
};
/* 模式名称的中文字符的编号 _by_zzw*/
UINT16 STR_MODE_CN[][4] = {

	155, 156, 153, 154, // 分割模式
	157, 158, 153, 154, // 拼接模式
	159, 160, 163, 164, // 无缝矩阵
	161, 162, 163, 164, // 直通矩阵
};
UINT16 STR_clear[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
UINT16 STR_Bri_CN[3] = {105, 0, 106};	// 亮度
UINT16 STR_Con_CN[3] = {107, 108, 109}; // 对比度
UINT16 STR_audio_input_CN[3][12] = {
	0,
	40,
	36,
	45,
	41,
	96,
	98,
	0,
	0,
	150,
	0,
	149,
	0,
	41,
	18,
	51,
	96,
	98,
	0,
	0,
	0,
	150,
	0,
	149,
	0,
	51,
	48,
	36,
	41,
	38,
	96,
	98,
	0,
	150,
	0,
	149,
};
UINT16 STR_hd_input_CN[12][12] = {
	0,
	40,
	36,
	45,
	41,
	0,
	41,
	46,
	17,
	0,
	149,
	0,
	0,
	40,
	36,
	45,
	41,
	0,
	41,
	46,
	18,
	0,
	149,
	0,
	0,
	40,
	36,
	45,
	41,
	0,
	41,
	46,
	19,
	0,
	149,
	0,
	0,
	40,
	36,
	45,
	41,
	0,
	41,
	46,
	20,
	0,
	149,
	0,
	0,
	41,
	18,
	51,
	0,
	41,
	46,
	17,
	0,
	0,
	149,
	0,
	0,
	41,
	18,
	51,
	0,
	41,
	46,
	18,
	0,
	0,
	149,
	0,
	0,
	41,
	18,
	51,
	0,
	41,
	46,
	19,
	0,
	0,
	149,
	0,
	0,
	41,
	18,
	51,
	0,
	41,
	46,
	20,
	0,
	0,
	149,
	0,
	51,
	48,
	36,
	41,
	38,
	0,
	41,
	46,
	17,
	0,
	149,
	0,
	51,
	48,
	36,
	41,
	38,
	0,
	41,
	46,
	18,
	0,
	149,
	0,
	51,
	48,
	36,
	41,
	38,
	0,
	41,
	46,
	19,
	0,
	149,
	0,
	51,
	48,
	36,
	41,
	38,
	0,
	41,
	46,
	20,
	0,
	149,
	0,
}; // 音频输入
UINT16 STR_audiooutput_CN[12][10] = {
	40,
	36,
	45,
	41,
	0,
	47,
	53,
	52,
	17,
	0,
	40,
	36,
	45,
	41,
	0,
	47,
	53,
	52,
	18,
	0,
	40,
	36,
	45,
	41,
	0,
	47,
	53,
	52,
	19,
	0,
	40,
	36,
	45,
	41,
	0,
	47,
	53,
	52,
	20,
	0, // hd
	41,
	18,
	51,
	0,
	47,
	53,
	52,
	17,
	0,
	0,
	41,
	18,
	51,
	0,
	47,
	53,
	52,
	18,
	0,
	0,
	41,
	18,
	51,
	0,
	47,
	53,
	52,
	19,
	0,
	0,
	41,
	18,
	51,
	0,
	47,
	53,
	52,
	20,
	0,
	0, // i2s
	51,
	48,
	36,
	41,
	38,
	0,
	47,
	53,
	52,
	17,
	51,
	48,
	36,
	41,
	38,
	0,
	47,
	53,
	52,
	18,
	51,
	48,
	36,
	41,
	38,
	0,
	47,
	53,
	52,
	19,
	51,
	48,
	36,
	41,
	38,
	0,
	47,
	53,
	52,
	20,
};												  // spdif
UINT16 STR_Dormancy_CN[4] = {138, 139, 140, 141}; // 休眠时间
UINT16 STR_Transparency_CN[3] = {142, 143, 144};  // 透明度
UINT16 STR_init_CN[3] = {145, 146, 147};		  // 初始化
UINT16 STR_time_CN[3][4] = {
	0,
	0,
	21,
	83,
	0,
	17,
	21,
	83,
	0,
	19,
	16,
	83,
}; //
UINT16 STR_Progress[8] = {
	114,
	116,
	117,
	118,
	119,
	120,
	121,
	132,
};


UINT16 STR_Input_Image_Size_Second_CN[4][10]=   /* 输入图像大小二级菜单 _zzw*/
{
	0,40,36,45,41,0,41,46,17,   //HDMI IN1
	0,40,36,45,41,0,41,46,18,   //HDMI IN2
	0,40,36,45,41,0,41,46,19,   //HDMI IN3
	0,40,36,45,41,0,41,46,20,   //HDMI IN4
};
UINT16 STR_Input_Image_Size_Third_CN[4][5] = /* 输入图像大小三级菜单 _zzw*/ 
{
    0,85,80,0,0,
	68,79,87,78,0,
	76,69,70,84,0,
	82,73,71,72,84,
};
UINT16 STR_Input_Image_Position_Second_CN[4][10]=   /* 输入图像位置二级菜单 _zzw*/
{
	0,40,36,45,41,0,41,46,17,   //HDMI IN1
	0,40,36,45,41,0,41,46,18,   //HDMI IN2
	0,40,36,45,41,0,41,46,19,   //HDMI IN3
	0,40,36,45,41,0,41,46,20,   //HDMI IN4
};
UINT16 STR_Input_Image_Position_Third_CN[4][5] = /* 输入图像位置三级菜单 _zzw*/ 
{
    0,85,80,0,0,
	68,79,87,78,0,
	76,69,70,84,0,
	82,73,71,72,84,
};
UINT16 STR_Input_Image_Cropping_Second_CN[4][10]=   /* 输入图像裁剪二级菜单 _zzw*/
{
	0,40,36,45,41,0,41,46,17,   //HDMI IN1
	0,40,36,45,41,0,41,46,18,   //HDMI IN2
	0,40,36,45,41,0,41,46,19,   //HDMI IN3
	0,40,36,45,41,0,41,46,20,   //HDMI IN4
};
UINT16 STR_Input_Image_Cropping_Third_CN[4][5] = /* 输入图像裁剪三级菜单 _zzw*/ 
{
    0,85,80,0,0,
	68,79,87,78,0,
	76,69,70,84,0,
	82,73,71,72,84,
};




// UINT16 Output_Resolution_CN[17] = { 79, 85, 84, 80, 85, 84, 0, 82, 69, 83, 79, 76, 85, 84, 73, 79, 78 };/* 输出分辨率 _zzw*/
// UINT16 Select_Move_Length_CN[18] = { 83, 69, 76, 69, 67, 84, 0, 77, 79, 86, 69, 0, 76, 69, 78, 71, 84, 72 };/* 选择移动长度 _zzw*/
// UINT16 Input_Image_Size_CN[16] = {73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 83, 73, 90, 69};/* 输入图像大小 _zzw*/
// UINT16 Input_Image_Position_CN[20] = {73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 80, 79, 83, 73, 84, 73, 79, 78};/* 输入图像位置 _zzw*/
// UINT16 Input_Image_Cropping_CN[20] = {73, 78, 80, 85, 84, 0, 73, 77, 65, 71, 69, 0, 67, 82, 79, 80, 80, 73, 78, 71};/* 输入图像裁剪 _zzw*/
// UINT16 Save_Settings_CN[13] = {83, 65, 86, 69, 0, 83, 69, 84, 84, 73, 78, 71, 83};/* 保存设置 _zzw*/
// UINT16 Read_Settings_CN[13] = {82, 69, 65, 68, 0, 83, 69, 84, 84, 73, 78, 71, 83};/* 读取设置 _zzw*/
// UINT16 Reset_Settings_CN[14] = {82, 69, 83, 69, 84, 0, 83, 69, 84, 84, 73, 78, 71, 83};/* 重置设置 _zzw*/

/* 亮度/对比度（进度条）配置函数 （进度）_by_zzw*/
VOID STR_Progress_CFG(UINT8 u8_progress)
{
	UINT8 u8_grade; /* 进度的等级  _by_zzw*/
	UINT8 u8_idx;
	u8_grade = u8_progress / 16 + 1;
	for (u8_idx = 1; u8_idx < 7; u8_idx++)
	{
		if (u8_idx < u8_grade)
			STR_Progress[u8_idx] = 115;
		else if (u8_idx > u8_grade)
			STR_Progress[u8_idx] = 131;
		else
			STR_Progress[u8_idx] = 131 - (u8_progress % 16);
	}
	if (u8_progress > 96)
		STR_Progress[7] = 233 - u8_progress;
	else
		STR_Progress[7] = 137;
}

/* 清除显示的字体 _by_zzw*/
VOID app_osd_fonts_clear(VOID)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts; /* 字体结构 _by_zzw*/
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_clear;
	st_osd_showfonts.u8_len = sizeof(STR_clear) / sizeof(STR_clear[0]);
	app_osd_show_fonts(&st_osd_showfonts);
	st_osd_showfonts.u8_y = 3;
	app_osd_show_fonts(&st_osd_showfonts);
	st_osd_showfonts.u8_y = 5;
	app_osd_show_fonts(&st_osd_showfonts);
	st_osd_showfonts.u8_y = 7;
	app_osd_show_fonts(&st_osd_showfonts);
	st_osd_showfonts.u8_y = 9;
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 休眠时间 _by_zzw*/
VOID app_osd_system_time_cfg(UINT8 u8_time)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	st_osd_showfonts.u8_x = 15;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_time_CN[u8_time];
	st_osd_showfonts.u8_len = sizeof(STR_time_CN[u8_time]) / sizeof(STR_time_CN[u8_time][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}

/* 透明度进度条 _by_zzw*/
VOID app_osd_system_Progress_cfg(UINT8 u8_ratio)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	STR_Progress_CFG(100 - u8_ratio);
	st_osd_showfonts.u8_x = 15;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_Progress;
	st_osd_showfonts.u8_len = sizeof(STR_Progress) / sizeof(STR_Progress[0]);
	app_osd_show_fonts(&st_osd_showfonts);
}

/* 配置系统设置选项-初始化 _by_zzw*/
VOID app_osd_system_cfg(VOID)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Dormancy_CN; /* 休眠 _by_zzw*/
	st_osd_showfonts.u8_len = sizeof(STR_Dormancy_CN) / sizeof(STR_Dormancy_CN[0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_Transparency_CN; /* 透明度_by_zzw*/
	st_osd_showfonts.u8_len = sizeof(STR_Transparency_CN) / sizeof(STR_Transparency_CN[0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 5;
	st_osd_showfonts.u16_idx = STR_init_CN; /* 初始化 _by_zzw*/
	st_osd_showfonts.u8_len = sizeof(STR_init_CN) / sizeof(STR_init_CN[0]);
	app_osd_show_fonts(&st_osd_showfonts);
}

/* 配置音频输入选项 _by_zzw*/
VOID app_osd_audio_in_cfg(UINT8 u8_y, UINT8 u8_audio_output)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = u8_y;
	st_osd_showfonts.u16_idx = STR_hd_input_CN[u8_audio_output];
	st_osd_showfonts.u8_len = sizeof(STR_hd_input_CN[u8_audio_output]) / sizeof(STR_hd_input_CN[u8_audio_output][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* HDMI 输出的选项， _by_zzw*/
VOID app_osd_hd_out_cfg(VOID)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[0];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[4]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[1]; // STR_Audio_input_CN[1];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[5]) / sizeof(STR_audiooutput_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[2]; // STR_Audio_input_CN[2];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[6]) / sizeof(STR_audiooutput_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 9;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[3];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[7]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/*  I2S 输出的选项 _by_zzw*/
VOID app_osd_i2s_out_cfg(VOID)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[4];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[4]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[5]; // STR_Audio_input_CN[1];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[5]) / sizeof(STR_audiooutput_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[6]; // STR_Audio_input_CN[2];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[6]) / sizeof(STR_audiooutput_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 9;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[7];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[7]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 音频输出配置函数 _by_zzw*/
VOID app_osd_spdif_out_cfg(VOID)
{

	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[8];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[4]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[9]; // STR_Audio_input_CN[1];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[5]) / sizeof(STR_audiooutput_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[10]; // STR_Audio_input_CN[2];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[6]) / sizeof(STR_audiooutput_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 9;
	st_osd_showfonts.u16_idx = STR_audiooutput_CN[11];
	st_osd_showfonts.u8_len = sizeof(STR_audiooutput_CN[7]) / sizeof(STR_audiooutput_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 音频选择 _by_zzw*/
VOID app_osd_audio_sw_cfg(UINT8 u8_sw)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_audio_input_CN[u8_sw];
	st_osd_showfonts.u8_len = sizeof(STR_audio_input_CN[u8_sw]) / sizeof(STR_audio_input_CN[u8_sw][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 配置亮度 对比度(进度条) _by_zzw*/
VOID app_osd_bri_con_cfg(UINT8 u8_bcsh_bri, UINT8 u8_bcsh_con)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	STR_Progress_CFG(u8_bcsh_bri);
	st_osd_showfonts.u8_x = 13;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Progress;
	st_osd_showfonts.u8_len = sizeof(STR_Progress) / sizeof(STR_Progress[0]);
	app_osd_show_fonts(&st_osd_showfonts);

	STR_Progress_CFG(u8_bcsh_con);
	st_osd_showfonts.u8_x = 13;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_Progress;
	st_osd_showfonts.u8_len = sizeof(STR_Progress) / sizeof(STR_Progress[0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 亮度 对比度。 _by_zzw*/
VOID app_osd_bcsh_cfg(VOID)
{
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Bri_CN;
	st_osd_showfonts.u8_len = sizeof(STR_Bri_CN) / sizeof(STR_Bri_CN[0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_Con_CN;
	st_osd_showfonts.u8_len = sizeof(STR_Con_CN) / sizeof(STR_Con_CN[0]);
	app_osd_show_fonts(&st_osd_showfonts);
}
/* 信息 _by_zzw*/
VOID app_osd_info_cfg()
{
	OSD_DEV_T st_osd_dev;
	OSD_STRING_T st_osd_string;
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_dev.u8_osd_idx = OSD_1;

	st_osd_showfonts.u8_x = 18;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_MODE_CN[g_u8_display_mode];
	st_osd_showfonts.u8_len = sizeof(STR_MODE_CN[g_u8_display_mode]) / sizeof(STR_MODE_CN[g_u8_display_mode][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_string.pstr = MS1826_4X4DEMO_SW_VERSION;
	st_osd_string.u8_x = 10;
	st_osd_string.u8_y = 3;
	api_osd_show_string(&st_osd_dev, &st_osd_string);
}






VOID app_osd_Input_Image_Size_Second_cfg(VOID)  /* 输入图像大小2级菜单 _by_zzw*/
{
  APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Second_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Second_CN[0])/sizeof(STR_Input_Image_Size_Second_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Second_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Second_CN[1])/sizeof(STR_Input_Image_Size_Second_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Second_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Second_CN[2])/sizeof(STR_Input_Image_Size_Second_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Second_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Second_CN[3])/sizeof(STR_Input_Image_Size_Second_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
	
}
VOID app_Input_Image_Size_Third_cfg(VOID)   /* 输入图像大小3级菜单 _by_zzw*/
{
	 APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Third_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Third_CN[0])/sizeof(STR_Input_Image_Size_Third_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Third_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Third_CN[1])/sizeof(STR_Input_Image_Size_Third_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Third_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Third_CN[2])/sizeof(STR_Input_Image_Size_Third_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Size_Third_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Size_Third_CN[3])/sizeof(STR_Input_Image_Size_Third_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
}
VOID app_osd_STR_Input_Image_Position_Second_CN_cfg(VOID)  /* 输入图像位置2级菜单 _by_zzw*/
{
  APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Second_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Second_CN[0])/sizeof(STR_Input_Image_Position_Second_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Second_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Second_CN[1])/sizeof(STR_Input_Image_Position_Second_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Second_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Second_CN[2])/sizeof(STR_Input_Image_Position_Second_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Second_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Second_CN[3])/sizeof(STR_Input_Image_Position_Second_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
	
}
VOID app_osd_STR_Input_Image_Position_Third_CN_cfg(VOID)   /* 输入图像位置3级菜单 _by_zzw*/
{
	 APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Third_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Third_CN[0])/sizeof(STR_Input_Image_Position_Third_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Third_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Third_CN[1])/sizeof(STR_Input_Image_Position_Third_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Third_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Third_CN[2])/sizeof(STR_Input_Image_Position_Third_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Position_Third_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Position_Third_CN[3])/sizeof(STR_Input_Image_Position_Third_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
}
VOID app_osd_Input_Image_Cropping_Second_cfg(VOID)  /* 输入图像裁剪2级菜单 _by_zzw*/
{
  APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Second_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Second_CN[0])/sizeof(STR_Input_Image_Cropping_Second_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Second_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Second_CN[1])/sizeof(STR_Input_Image_Cropping_Second_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Second_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Second_CN[2])/sizeof(STR_Input_Image_Cropping_Second_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 10;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Second_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Second_CN[3])/sizeof(STR_Input_Image_Cropping_Second_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
	
}
VOID app_osd_Input_Image_Cropping_Third_cfg(VOID)   /* 输入图像裁剪3级菜单 _by_zzw*/
{
	 APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Third_CN[0];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Third_CN[0])/sizeof(STR_Input_Image_Cropping_Third_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts); 
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 3 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Third_CN[1];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Third_CN[1])/sizeof(STR_Input_Image_Cropping_Third_CN[1][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 5 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Third_CN[2];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Third_CN[2])/sizeof(STR_Input_Image_Cropping_Third_CN[2][0]);
	app_osd_show_fonts(&st_osd_showfonts);   
	
	st_osd_showfonts.u8_x = 22;
	st_osd_showfonts.u8_y = 7 ;
	st_osd_showfonts.u16_idx = STR_Input_Image_Cropping_Third_CN[3];   
	st_osd_showfonts.u8_len = sizeof(STR_Input_Image_Cropping_Third_CN[3])/sizeof(STR_Input_Image_Cropping_Third_CN[3][0]);
	app_osd_show_fonts(&st_osd_showfonts);  
}







/* 一级菜单定义 _zzw*/
VOID app_osd_cfg(VOID)
{
	OSD_DEV_T st_osd_dev;
	OSD_WINSIZE_T st_osd_win_size;
	APP_OSD_DRAWRECT_T st_osd_drawrect;
	APP_OSD_SHOWFONTS_T st_osd_showfonts;
	st_osd_dev.u8_osd_idx = OSD_1;
	api_osd_clear(&st_osd_dev);

	api_osd_window_enable(&g_st_osd_dev[1], OSD_WIN_DISPLAY, MS_FALSE);
	api_osd_trigger(&g_st_osd_dev[1]);

	api_osd_window_enable(&g_st_osd_dev[2], OSD_WIN_DISPLAY, MS_FALSE);
	api_osd_trigger(&g_st_osd_dev[2]);

	api_osd_window_enable(&g_st_osd_dev[3], OSD_WIN_DISPLAY, MS_FALSE);
	api_osd_trigger(&g_st_osd_dev[3]);

	g_st_misc_osd_mux[0].b_enable = MS_TRUE;
	g_st_misc_osd_mux[0].u8_osd_module = MISC_VIDEO_MODULE_OSD1;
	g_st_misc_osd_mux[0].u8_tx_module = MISC_VIDEO_MODULE_TX1;
	api_misc_osd_timing_mux_cfg(&g_st_misc_osd_mux[0]); /* osd输出通路选择 _by_zzw*/

	st_osd_showfonts.u8_osd_idx = OSD_1;
	st_osd_showfonts.osd_font_colortab = OSD_PATTERN_COLOR2;

	st_osd_win_size.u8_osd_win = OSD_WIN_DISPLAY;
	st_osd_win_size.u16_w = 35;
	st_osd_win_size.u16_h = 15;
	st_osd_win_size.u16_x = (g_stVideo_HDTx_Timing.u16_hactive - 35 * 16) / 2; // 10;
	st_osd_win_size.u16_y = (g_stVideo_HDTx_Timing.u16_vactive - 15 * 18) / 2; // 10;
	api_osd_window_position_set(&g_st_osd_dev[0], &st_osd_win_size);		   /* OSD 窗口位置设置 _by_zzw*/

	st_osd_drawrect.u8_osd_idx = OSD_1;
	st_osd_drawrect.u8_osd_win = OSD_WIN_HIGHLIGHT1;
	st_osd_drawrect.st_fill_start_color.u8_R = 0xFF;
	st_osd_drawrect.st_fill_start_color.u8_G = 0xFF;
	st_osd_drawrect.st_fill_start_color.u8_B = 0x00;
	st_osd_drawrect.u16_x = 1;
	st_osd_drawrect.u16_y = 1;
	st_osd_drawrect.u16_w = 16 * 35;
	st_osd_drawrect.u16_h = 18 * 15;
	app_osd_DrawRect(&st_osd_drawrect, MS_TRUE); // 背景框

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 1;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[0];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 3;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[1];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 5;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[2];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 7;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[3];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);
/*  _zzw*/
	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 9;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[4];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 11;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[5];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);

	st_osd_showfonts.u8_x = 1;
	st_osd_showfonts.u8_y = 13;
	st_osd_showfonts.u16_idx = STR_OsdMenu_CN[6];
	st_osd_showfonts.u8_len = sizeof(STR_OsdMenu_CN[0]) / sizeof(STR_OsdMenu_CN[0][0]);
	app_osd_show_fonts(&st_osd_showfonts);
}










/* 窗口绘制选中背景 _by_zzw*/
VOID app_osd_drawrect_win(UINT8 u8_x, UINT8 u8_y, UINT8 u8_len)
{
	APP_OSD_DRAWRECT_T st_osd_drawrect;
	st_osd_drawrect.u8_osd_idx = OSD_1;
	st_osd_drawrect.u8_osd_win = OSD_WIN_HIGHLIGHT2;
	st_osd_drawrect.st_fill_start_color.u8_R = 0x80;
	st_osd_drawrect.st_fill_start_color.u8_G = 0x00; /* 选中背景 _by_zzw*/
	st_osd_drawrect.st_fill_start_color.u8_B = 0x00;
	st_osd_drawrect.u16_x = 16 * u8_x;
	st_osd_drawrect.u16_y = 9 + 18 * u8_y;
	st_osd_drawrect.u16_w = 16 * u8_len;
	st_osd_drawrect.u16_h = 18 * 2;
	app_osd_DrawRect(&st_osd_drawrect, MS_TRUE); // 背景框
}

/* 实时菜单绘画（级数，选择值） _by_zzw*/
VOID app_osd_menu_drawrect(UINT8 u8_level, UINT8 u8_y)
{
	api_misc_osd_timing_mux_cfg(&g_st_misc_osd_mux[0]); /* osd输出通路选择 _by_zzw*/
	if (u8_level == 0)
	{
		g_u8_level[0] = u8_y;
		app_osd_drawrect_win(1, u8_y * 2, 8);
		app_osd_fonts_clear();
		if (u8_y == 0)
		{
			app_osd_bcsh_cfg();
			app_osd_bri_con_cfg(g_u8_bcsh_bri, g_u8_bcsh_con);
		}
		else if (u8_y == 1)
		{

			app_osd_audio_sw_cfg(g_u8_audio_sw);
			if (g_u8_audio_sw == 0)
				app_osd_hd_out_cfg();
			else if (g_u8_audio_sw == 1)
				app_osd_i2s_out_cfg();
			else if (g_u8_audio_sw == 2)
				app_osd_spdif_out_cfg();
			app_osd_audio_in_cfg(3, g_u8_app_audio_output[g_u8_audio_sw][0]);
			app_osd_audio_in_cfg(5, g_u8_app_audio_output[g_u8_audio_sw][1]);
			app_osd_audio_in_cfg(7, g_u8_app_audio_output[g_u8_audio_sw][2]);
			app_osd_audio_in_cfg(9, g_u8_app_audio_output[g_u8_audio_sw][3]);
		}
		else if (u8_y == 2)
		{
			app_osd_system_time_cfg(g_u8_time);
			app_osd_system_Progress_cfg(g_u8_ratio);
			app_osd_system_cfg();
		}
		else if (u8_y == 3)
		{
			app_osd_info_cfg();
		}else if (u8_y == 4)
		{
			app_osd_Input_Image_Size_Second_cfg();
		}else if (u8_y == 5)
		{
			app_osd_STR_Input_Image_Position_Second_CN_cfg();
		}else if (u8_y == 6)
		{
			app_osd_Input_Image_Cropping_Second_cfg();
		}
	}

	else if (u8_level == 1) /* 进入二级菜单 _by_zzw*/
	{
		if (g_u8_level[0] == 0)
		{
			app_osd_drawrect_win(9, u8_y * 2, 15);
			app_osd_bri_con_cfg(g_u8_bcsh_bri, g_u8_bcsh_con);
		}
		else if (g_u8_level[0] == 1)
		{
			app_osd_drawrect_win(9, u8_y * 2, 24);
			app_osd_audio_sw_cfg(g_u8_audio_sw);
			if (g_u8_audio_sw == 0)
				app_osd_hd_out_cfg();
			else if (g_u8_audio_sw == 1)
				app_osd_i2s_out_cfg();
			else if (g_u8_audio_sw == 2)
				app_osd_spdif_out_cfg();
			app_osd_audio_in_cfg(3, g_u8_app_audio_output[g_u8_audio_sw][0]);
			app_osd_audio_in_cfg(5, g_u8_app_audio_output[g_u8_audio_sw][1]);
			app_osd_audio_in_cfg(7, g_u8_app_audio_output[g_u8_audio_sw][2]);
			app_osd_audio_in_cfg(9, g_u8_app_audio_output[g_u8_audio_sw][3]);
		}
		else if (g_u8_level[0] == 2)
		{
			app_osd_drawrect_win(9, u8_y * 2, 15);
			app_osd_system_time_cfg(g_u8_time);
			app_osd_system_Progress_cfg(g_u8_ratio);
		}
		else if (g_u8_level[0] == 3)
		{
			app_osd_drawrect_win(9, u8_y * 2, 15);
			app_Input_Image_Size_Third_cfg();
		}
		else if (g_u8_level[0] == 4)
		{
			app_osd_drawrect_win(9, u8_y * 2, 15);
			app_osd_STR_Input_Image_Position_Third_CN_cfg();
		}
		else if (g_u8_level[0] == 5)
		{
			app_osd_drawrect_win(9, u8_y * 2, 15);
			app_osd_Input_Image_Cropping_Third_cfg();
		}
	}
}
