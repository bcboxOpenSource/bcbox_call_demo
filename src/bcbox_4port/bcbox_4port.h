#ifndef BCBOX_20230604
#define BCBOX_20230604



#define BCBOX_DLL_EXPORTS 

#define BC_Send_Press_Data_Debug 0
#if BC_Send_Press_Data_Debug
#define BC_PRINTF(...) printf(__VA_ARGS__)
#else
#define BC_PRINTF(...)
#endif


#ifdef BCBOX_DLL_EXPORTS
#define BCBOX_DLL_API extern "C" __declspec(dllexport)
#else
#define BCBOX_DLL_API 
#endif

#include <Windows.h>
#include <time.h>
#include <iostream>
#include <Mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include "HidTable.h"
#include "windows.h"
#include <exception>
#include <vector>


//---------------------------------结构体定义---------------------------------------------------------------
//此结构体最多256字节  int 类型一共64字节
typedef struct
{
	unsigned int NewBoardFlag;  // 新板标志
	unsigned int devVIDPID;	    //DEVICE的VID |PID
	unsigned int cmdVIDPID;     //cmd的VID和PID
	unsigned int PortEnable;    //键鼠透传到哪个端口
}config_summary_t_BC;

typedef union
{
	unsigned char	 buf[64];//64字节
	config_summary_t_BC ROM;    //
}t_config_param_BC;


//外接设备VIDPID参数
typedef struct
{
	unsigned short  VID;
	unsigned short  PID;
	unsigned short  DID;
	unsigned short  HID;
	unsigned char   mType;
	unsigned char   kType;
}CONFIG_BC_MODE_t_BC;


typedef union
{
	unsigned char	 buf[64];//64字节
	CONFIG_BC_MODE_t_BC cfg[3];//配置缓存
}t_config_mktype_BC;


typedef struct
{
	unsigned char   mstatus;	//鼠标状态
	unsigned char   kstatus[11];//键盘状态
}STATE_BC_t;
typedef union
{
	unsigned char	 	buf[64];//64字节
	STATE_BC_t			km;
}t_status_BC;

struct Press_y_data
{
	int y;
	int time_stamp;
};

struct Press_xy_data
{
	int x;
	int y;
	int time_stamp;
};


extern t_status_BC bc_status;
//---------------------------------导出函数定义---------------------------------------------------------------//

void sleep_hi(int i); // 高精度延时


/**********************************************************
必须首先调用此函数才能与盒子通信
输入：   VID，PID 板子的硬件ID值，固定为0x1a88 0xFE00（不提供修改。免得盒子都连不上）
返回值： 
		-1：找不到指定的VID和PID
		0:正常
是否测试：是
测试时间：20221224  
*************************************************************/
BCBOX_DLL_API  int BC_init(unsigned short vid,unsigned short pid);


 
/***********************************************************
鼠标左键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标左键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
***********************************************************/
 BCBOX_DLL_API  int BC_left(char port,unsigned char vk_key);

 /***********************************************************
鼠标中键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标中键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
***********************************************************/
 BCBOX_DLL_API  int BC_middle(char port,unsigned char vk_key);


 /***********************************************************
鼠标右键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标右键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
***********************************************************/
BCBOX_DLL_API  int BC_right(char port,unsigned char vk_key);


/***********************************************************
鼠标移动控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
x		:x方向相对偏移量[-32767,32767]
y		:y方向相对偏移量[-32767,32767]
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
***********************************************************/
BCBOX_DLL_API  int BC_move(short x,short y);


BCBOX_DLL_API  int BC_side1(char port,unsigned char  vk_key);

/***********************************************************
鼠标侧键2控制：
port : 往哪个端口发数据, 只往port1发数据为1, 只往port2发数据为2，两个端口都发数据为3
vk_key : 鼠标侧键2状态设置，0：松开  1：按下
返回值：
- 1：发送失败
0  ：发送成功
是否测试：是 --test
***********************************************************/
BCBOX_DLL_API  int BC_side2(char port,unsigned char  vk_key);


// 已测试 可用 time:2023-06-26 不过这个是rtos专属固件 ------------------- 
int BC_Send_Press_Data(std::vector<Press_y_data> press_data);
int BC_Send_Press_Data(std::vector<Press_xy_data> press_data);
int BC_Enable_Press(unsigned char status);
// ----------------------------------------------------------------------

// 已测试 可用 time:2023-06-26 是否按下侧键停止鼠标数据上传
int BC_Enable_AIM_Key_Move(unsigned char status);

// 已测试 可用 time:2023-06-26 是否开启自动急停
int BC_Enable_Quickly_stop(unsigned char status);

// 已测试 可用 time:2023-06-26 获取当前鼠标数据
int BC_Get_Mouse_Data(unsigned char* addr);

// 已测试 可用 time:2023-06-26 开启端点数据上传功能 不需要时则关闭
int BC_Enable_Endp_upload(unsigned char status);

// 已测试 可用 time:2023-06-26 获取端点的连接状态
int BC_Get_EndP_Connection_Status(unsigned char* addr);

// 已测试 可用 time:2023-06-26 当前句柄是否有效 当前是否连接 0 无效 1 有效
int BC_is_device_valid();


// 已测试 可用 time:2023-06-27 适配专用 设置鼠标映射
int BC_Set_Mouse_Map(uint8_t enable_status, uint8_t mouse_status, uint8_t xamount, uint8_t xdirection, uint8_t yamount, uint8_t ydirection, uint8_t wheel_status);

// 已测试 可用 time:2023-06-27 获取指定缓存区域的data 
int BC_Get_Flash_data(unsigned char* addr);


int BC_Key_is_down(int vk_key);



/**********************************************************
输入： 开关外接键鼠监控功能
enable :0：关闭监控   1：打开监控
返回值：
		-1：找不到指定的VID和PID
		0:正常
是否测试：否

使能外接键鼠报告监测功能后，物理键鼠的状态会上传一份到PC1端口（跑软件的电脑）。
因此跑软件的电脑能可以通过API知道外接键鼠的移动点击状态。可以绕过hook系统API。
读取外接键鼠的状态。
*************************************************************/
int BC_EnableMonitor(char enable);


/**********************************************************
查询物理鼠标左键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标左键松开
			1：物理鼠标左键按下
			-1：监听功能未开启
***********************************************************/
int BC_MonitorLeft();
/**********************************************************
查询物理鼠标右键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标右键松开
			1：物理鼠标右键按下
			-1：监听功能未开启
**********************************************************/
int BC_MonitorRight();
/**********************************************************
查询物理鼠标中键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标中键松开
			1：物理鼠标中键按下
			-1：监听功能未开启
**********************************************************/
int BC_MonitorMiddle();
/**********************************************************
查询物理鼠标侧键1状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标侧键1松开
			1：物理鼠标侧键1按下
			-1：监听功能未开启
**********************************************************/
int BC_MonitorSide1();
/**********************************************************
查询物理鼠标侧键2状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标侧键2松开
			1：物理鼠标侧键2按下
			-1：监听功能未开启
**********************************************************/
int BC_MonitorSide2();

int BC_Monitor_Mouse(int vk);

/**********************************************************
查询物理键盘指定按键是否按下 --只有开启了BC_EnableMonitor（1）后才能使用
输入：		vk_key 要查询的HID键值  详见HID键值表

返回值		0：vk_key松开
			1：vk_key按下
			-1：监听功能未开启
**********************************************************/
int BC_MonitorKeyboard(int vk_key);

/****************************以下函数与适配相关。不需要调用*********************************/

#endif // !BCBOX_20230604

