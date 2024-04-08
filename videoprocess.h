#ifndef _VIDEO_PRCESS_H
#define _VIDEO_PRCESS_H
#include "app.h"
#include "app_splitter_mode.h"
#include "app_splicer_mode.h"
#include "app_single_mode.h"
#include "app_bypass_mode.h"
#include "app_osdprocess.h"

#define CHIP_INTERNAL_PROCESS_COLORSPACE  	HD_COLORSPACE_YCBCR444  /*定义芯片内部处理的颜色空间为YCbCr444 _by_zzw*/
#define CHIP_INTERNAL_PROCESS_COLORRANGE  	CSC_YUV444_BT601/*定义芯片内部处理的颜色范围 _by_zzw*/

#define CHIP_TX_CSC_OUT_COLORSPACE  		HD_COLORSPACE_RGB/*定义芯片传输输出的颜色空间为RGB _by_zzw*/
#define CHIP_TX_CSC_OUT_COLORRANGE  		CSC_RGB_0_255/*定义芯片传输输出的颜色范围 _by_zzw*/

//1fps 1920x1080 RGB/YUV444: 1920*1080*3 = 6220800	YUV422: 1920*1080*2 = 4147200
//1fps 3840x2160 RGB/YUV444: 3840*2160*3 = 24883200	YUV422:	3840*2160*2 = 16588800
//MS1826 MEMORY 128M: 128*1024*1024 = 134217728

#define  MEMORY_BUF_ADDR_START       		8000000 /*定义内存缓冲区的起始地址 _by_zzw*/

#define  SINGLE_BUF_MEMORY_1080P_SIZE       6220800 /* 定义单个内存缓冲区用于1080P视频的大小 _zzw*/
#define  SINGLE_BUF_MEMORY_4K_SIZE       	24883200/* 定义单个内存缓冲区用于4K视频的大小。 _zzw*/

#define	MEMORY_4K_H_SIZE_MAX				3840/* 定义4K视频的水平最大尺寸 _zzw*/
#define	MEMORY_4K_V_SIZE_MAX				2160/* 定义4K视频的垂直最大尺寸 _zzw*/

#define	MEMORY_1080P_H_SIZE_MAX				1920/*  定义1080P视频的水平最大尺寸 _by_zzw*/
#define	MEMORY_1080P_V_SIZE_MAX				1080/* 定义1080P视频的垂直最大尺寸 _by_zzw*/

typedef enum _E_VIDEO_PATTERN_CHANNEL_/* 定义视频输入通道 _by_zzw*/
{
	HD_RX_1   = 0,
	HD_RX_2   = 1,
	HD_RX_3   = 2,
	HD_RX_4   = 3,
	HD_RX_ALL = 4
}VIDEO_PATTERN_CHANNEL_E;

typedef enum _E_VIDEO_OUTPUT_CHANNEL_/* 定义视频输出通道 _by_zzw*/
{
	HD_TX_1   = 0,
	HD_TX_2   = 1,
	HD_TX_3   = 2,
	HD_TX_4   = 3,
	HD_TX_ALL = 4
}VIDEO_OUTPUT_CHANNEL_E;

VOID video_process_init(VOID);/* 初始化视频处理函数。 _by_zzw*/
VOID video_process_cfg(VOID);/* 视频处理配置函数 _by_zzw*/
VOID video_process(VOID);/* 执行视频处理 _by_zzw*/
VOID _audio_switch_process(VOID);/* 音频切换处理函数 _by_zzw*/
VOID _Video_port_hd_tx_init(UINT8 u8_index);/* 初始化高清视频传输端口。 _by_zzw*/
VOID _video_misc_process(UINT8 u8_rx_id);/* 其他视频处理 _by_zzw*/
VOID osd_service(VOID);/* OSD服务 _by_zzw*/
VOID _video_mute(UINT8 u8_path, BOOL b_mute);/* 视频静音处理函数 _by_zzw*/
#endif//_VIDEO_PRCESS_H
