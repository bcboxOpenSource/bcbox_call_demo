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


//---------------------------------�ṹ�嶨��---------------------------------------------------------------
//�˽ṹ�����256�ֽ�  int ����һ��64�ֽ�
typedef struct
{
	unsigned int NewBoardFlag;  // �°��־
	unsigned int devVIDPID;	    //DEVICE��VID |PID
	unsigned int cmdVIDPID;     //cmd��VID��PID
	unsigned int PortEnable;    //����͸�����ĸ��˿�
}config_summary_t_BC;

typedef union
{
	unsigned char	 buf[64];//64�ֽ�
	config_summary_t_BC ROM;    //
}t_config_param_BC;


//����豸VIDPID����
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
	unsigned char	 buf[64];//64�ֽ�
	CONFIG_BC_MODE_t_BC cfg[3];//���û���
}t_config_mktype_BC;


typedef struct
{
	unsigned char   mstatus;	//���״̬
	unsigned char   kstatus[11];//����״̬
}STATE_BC_t;
typedef union
{
	unsigned char	 	buf[64];//64�ֽ�
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
//---------------------------------������������---------------------------------------------------------------//

void sleep_hi(int i); // �߾�����ʱ


/**********************************************************
�������ȵ��ô˺������������ͨ��
���룺   VID��PID ���ӵ�Ӳ��IDֵ���̶�Ϊ0x1a88 0xFE00�����ṩ�޸ġ���ú��Ӷ������ϣ�
����ֵ�� 
		-1���Ҳ���ָ����VID��PID
		0:����
�Ƿ���ԣ���
����ʱ�䣺20221224  
*************************************************************/
BCBOX_DLL_API  int BC_init(unsigned short vid,unsigned short pid);


 
/***********************************************************
���������ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :������״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
***********************************************************/
 BCBOX_DLL_API  int BC_left(char port,unsigned char vk_key);

 /***********************************************************
����м����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :����м�״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
***********************************************************/
 BCBOX_DLL_API  int BC_middle(char port,unsigned char vk_key);


 /***********************************************************
����Ҽ����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :����Ҽ�״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
***********************************************************/
BCBOX_DLL_API  int BC_right(char port,unsigned char vk_key);


/***********************************************************
����ƶ����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
x		:x�������ƫ����[-32767,32767]
y		:y�������ƫ����[-32767,32767]
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
***********************************************************/
BCBOX_DLL_API  int BC_move(short x,short y);


BCBOX_DLL_API  int BC_side1(char port,unsigned char  vk_key);

/***********************************************************
�����2���ƣ�
port : ���ĸ��˿ڷ�����, ֻ��port1������Ϊ1, ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key : �����2״̬���ã�0���ɿ�  1������
����ֵ��
- 1������ʧ��
0  �����ͳɹ�
�Ƿ���ԣ��� --test
***********************************************************/
BCBOX_DLL_API  int BC_side2(char port,unsigned char  vk_key);


// �Ѳ��� ���� time:2023-06-26 ���������rtosר���̼� ------------------- 
int BC_Send_Press_Data(std::vector<Press_y_data> press_data);
int BC_Send_Press_Data(std::vector<Press_xy_data> press_data);
int BC_Enable_Press(unsigned char status);
// ----------------------------------------------------------------------

// �Ѳ��� ���� time:2023-06-26 �Ƿ��²��ֹͣ��������ϴ�
int BC_Enable_AIM_Key_Move(unsigned char status);

// �Ѳ��� ���� time:2023-06-26 �Ƿ����Զ���ͣ
int BC_Enable_Quickly_stop(unsigned char status);

// �Ѳ��� ���� time:2023-06-26 ��ȡ��ǰ�������
int BC_Get_Mouse_Data(unsigned char* addr);

// �Ѳ��� ���� time:2023-06-26 �����˵������ϴ����� ����Ҫʱ��ر�
int BC_Enable_Endp_upload(unsigned char status);

// �Ѳ��� ���� time:2023-06-26 ��ȡ�˵������״̬
int BC_Get_EndP_Connection_Status(unsigned char* addr);

// �Ѳ��� ���� time:2023-06-26 ��ǰ����Ƿ���Ч ��ǰ�Ƿ����� 0 ��Ч 1 ��Ч
int BC_is_device_valid();


// �Ѳ��� ���� time:2023-06-27 ����ר�� �������ӳ��
int BC_Set_Mouse_Map(uint8_t enable_status, uint8_t mouse_status, uint8_t xamount, uint8_t xdirection, uint8_t yamount, uint8_t ydirection, uint8_t wheel_status);

// �Ѳ��� ���� time:2023-06-27 ��ȡָ�����������data 
int BC_Get_Flash_data(unsigned char* addr);


int BC_Key_is_down(int vk_key);



/**********************************************************
���룺 ������Ӽ����ع���
enable :0���رռ��   1���򿪼��
����ֵ��
		-1���Ҳ���ָ����VID��PID
		0:����
�Ƿ���ԣ���

ʹ����Ӽ��󱨸��⹦�ܺ���������״̬���ϴ�һ�ݵ�PC1�˿ڣ�������ĵ��ԣ���
���������ĵ����ܿ���ͨ��API֪����Ӽ�����ƶ����״̬�������ƹ�hookϵͳAPI��
��ȡ��Ӽ����״̬��
*************************************************************/
int BC_EnableMonitor(char enable);


/**********************************************************
��ѯ����������״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�������������ɿ�
			1����������������
			-1����������δ����
***********************************************************/
int BC_MonitorLeft();
/**********************************************************
��ѯ��������Ҽ�״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0����������Ҽ��ɿ�
			1����������Ҽ�����
			-1����������δ����
**********************************************************/
int BC_MonitorRight();
/**********************************************************
��ѯ��������м�״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0����������м��ɿ�
			1����������м�����
			-1����������δ����
**********************************************************/
int BC_MonitorMiddle();
/**********************************************************
��ѯ���������1״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�����������1�ɿ�
			1�����������1����
			-1����������δ����
**********************************************************/
int BC_MonitorSide1();
/**********************************************************
��ѯ���������2״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�����������2�ɿ�
			1�����������2����
			-1����������δ����
**********************************************************/
int BC_MonitorSide2();

int BC_Monitor_Mouse(int vk);

/**********************************************************
��ѯ�������ָ�������Ƿ��� --ֻ�п�����BC_EnableMonitor��1�������ʹ��
���룺		vk_key Ҫ��ѯ��HID��ֵ  ���HID��ֵ��

����ֵ		0��vk_key�ɿ�
			1��vk_key����
			-1����������δ����
**********************************************************/
int BC_MonitorKeyboard(int vk_key);

/****************************���º�����������ء�����Ҫ����*********************************/

#endif // !BCBOX_20230604

