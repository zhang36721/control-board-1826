#ifndef _VIDEO_PRCESS_H
#define _VIDEO_PRCESS_H
#include "app.h"
#include "app_splitter_mode.h"
#include "app_splicer_mode.h"
#include "app_single_mode.h"
#include "app_bypass_mode.h"
#include "app_osdprocess.h"

#define CHIP_INTERNAL_PROCESS_COLORSPACE  	HD_COLORSPACE_YCBCR444  /*����оƬ�ڲ��������ɫ�ռ�ΪYCbCr444 _by_zzw*/
#define CHIP_INTERNAL_PROCESS_COLORRANGE  	CSC_YUV444_BT601/*����оƬ�ڲ��������ɫ��Χ _by_zzw*/

#define CHIP_TX_CSC_OUT_COLORSPACE  		HD_COLORSPACE_RGB/*����оƬ�����������ɫ�ռ�ΪRGB _by_zzw*/
#define CHIP_TX_CSC_OUT_COLORRANGE  		CSC_RGB_0_255/*����оƬ�����������ɫ��Χ _by_zzw*/

//1fps 1920x1080 RGB/YUV444: 1920*1080*3 = 6220800	YUV422: 1920*1080*2 = 4147200
//1fps 3840x2160 RGB/YUV444: 3840*2160*3 = 24883200	YUV422:	3840*2160*2 = 16588800
//MS1826 MEMORY 128M: 128*1024*1024 = 134217728

#define  MEMORY_BUF_ADDR_START       		8000000 /*�����ڴ滺��������ʼ��ַ _by_zzw*/

#define  SINGLE_BUF_MEMORY_1080P_SIZE       6220800 /* ���嵥���ڴ滺��������1080P��Ƶ�Ĵ�С _zzw*/
#define  SINGLE_BUF_MEMORY_4K_SIZE       	24883200/* ���嵥���ڴ滺��������4K��Ƶ�Ĵ�С�� _zzw*/

#define	MEMORY_4K_H_SIZE_MAX				3840/* ����4K��Ƶ��ˮƽ���ߴ� _zzw*/
#define	MEMORY_4K_V_SIZE_MAX				2160/* ����4K��Ƶ�Ĵ�ֱ���ߴ� _zzw*/

#define	MEMORY_1080P_H_SIZE_MAX				1920/*  ����1080P��Ƶ��ˮƽ���ߴ� _by_zzw*/
#define	MEMORY_1080P_V_SIZE_MAX				1080/* ����1080P��Ƶ�Ĵ�ֱ���ߴ� _by_zzw*/

typedef enum _E_VIDEO_PATTERN_CHANNEL_/* ������Ƶ����ͨ�� _by_zzw*/
{
	HD_RX_1   = 0,
	HD_RX_2   = 1,
	HD_RX_3   = 2,
	HD_RX_4   = 3,
	HD_RX_ALL = 4
}VIDEO_PATTERN_CHANNEL_E;

typedef enum _E_VIDEO_OUTPUT_CHANNEL_/* ������Ƶ���ͨ�� _by_zzw*/
{
	HD_TX_1   = 0,
	HD_TX_2   = 1,
	HD_TX_3   = 2,
	HD_TX_4   = 3,
	HD_TX_ALL = 4
}VIDEO_OUTPUT_CHANNEL_E;

VOID video_process_init(VOID);/* ��ʼ����Ƶ�������� _by_zzw*/
VOID video_process_cfg(VOID);/* ��Ƶ�������ú��� _by_zzw*/
VOID video_process(VOID);/* ִ����Ƶ���� _by_zzw*/
VOID _audio_switch_process(VOID);/* ��Ƶ�л������� _by_zzw*/
VOID _Video_port_hd_tx_init(UINT8 u8_index);/* ��ʼ��������Ƶ����˿ڡ� _by_zzw*/
VOID _video_misc_process(UINT8 u8_rx_id);/* ������Ƶ���� _by_zzw*/
VOID osd_service(VOID);/* OSD���� _by_zzw*/
VOID _video_mute(UINT8 u8_path, BOOL b_mute);/* ��Ƶ���������� _by_zzw*/
#endif//_VIDEO_PRCESS_H
