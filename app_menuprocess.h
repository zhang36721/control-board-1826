#ifndef _APP_OSDPROCESS1_H
#define _APP_OSDPROCESS1_H
#include "app.h"

/* 字体结构 描述需要在屏幕上显示的字体的相关信息 _by_zzw*/
typedef struct _APP_OSD_SHOWFONTS_T 
{
    UINT8 u8_osd_idx;                   /**显示字体的OSD索引  */
    OSD_FONT_COLORTAB_E osd_font_colortab;
    UINT16 * u16_idx;
	UINT8 u8_len;
    UINT8 u8_x;
    UINT8 u8_y;
}APP_OSD_SHOWFONTS_T;

/* OSD上绘制矩形的相关信息 _by_zzw*/
typedef struct _APP_OSD_DRAWRECT_T
{
    UINT8 u8_osd_win;                   /**<  OSD????????OSD_WIN_E */
    UINT8 u8_osd_idx;                   /**< OSD?????????OSD_INDEX_E */
    OSD_COLOR_T st_fill_start_color;    /**<  纯色填充RGB值 */
    UINT16 u16_w;/* 矩形的宽高 _by_zzw*/
    UINT16 u16_h;
    UINT16 u16_x;/* 矩形的坐标（应该是左上角） _by_zzw*/
    UINT16 u16_y;
}APP_OSD_DRAWRECT_T;

VOID app_osd_DrawRect(APP_OSD_DRAWRECT_T * st_osd_DrawRect,BOOL b_osd_en);
VOID app_osd_show_fonts(APP_OSD_SHOWFONTS_T *pst_osd_showfonts);
VOID app_osd_cfg(VOID);

#endif
