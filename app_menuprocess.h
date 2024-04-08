#ifndef _APP_OSDPROCESS1_H
#define _APP_OSDPROCESS1_H
#include "app.h"

/* ����ṹ ������Ҫ����Ļ����ʾ������������Ϣ _by_zzw*/
typedef struct _APP_OSD_SHOWFONTS_T 
{
    UINT8 u8_osd_idx;                   /**��ʾ�����OSD����  */
    OSD_FONT_COLORTAB_E osd_font_colortab;
    UINT16 * u16_idx;
	UINT8 u8_len;
    UINT8 u8_x;
    UINT8 u8_y;
}APP_OSD_SHOWFONTS_T;

/* OSD�ϻ��ƾ��ε������Ϣ _by_zzw*/
typedef struct _APP_OSD_DRAWRECT_T
{
    UINT8 u8_osd_win;                   /**<  OSD????????OSD_WIN_E */
    UINT8 u8_osd_idx;                   /**< OSD?????????OSD_INDEX_E */
    OSD_COLOR_T st_fill_start_color;    /**<  ��ɫ���RGBֵ */
    UINT16 u16_w;/* ���εĿ�� _by_zzw*/
    UINT16 u16_h;
    UINT16 u16_x;/* ���ε����꣨Ӧ�������Ͻǣ� _by_zzw*/
    UINT16 u16_y;
}APP_OSD_DRAWRECT_T;

VOID app_osd_DrawRect(APP_OSD_DRAWRECT_T * st_osd_DrawRect,BOOL b_osd_en);
VOID app_osd_show_fonts(APP_OSD_SHOWFONTS_T *pst_osd_showfonts);
VOID app_osd_cfg(VOID);

#endif
