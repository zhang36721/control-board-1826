#ifndef _APP_BYPASS_MODE_H_
#define _APP_BYPASS_MODE_H_

#include "app.h"

VOID video_bypass_tx_cfg(UINT8 u8_index);
VOID switch_bypass_channel(UINT8 u8_rx_sel, UINT8 u8_tx_sel);
VOID app_bypass_osd_init(VOID);
#endif
