#include "windows.h"
#include "bcbox_4port/tamp.h"

#include <iostream>
#include <vector>
#include <istream>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <thread>
#include <map>

#include "bcbox_4port/bcbox_4port.h"       //添加头文件
#include "bcbox_4port/HidTable.h"    //添加头文件


/// <summary>
/// bcbox的循环示例，表示了如何检测按键状态，并且如何去移动鼠标
/// </summary>
void bc_Monitor_and_Move()
{
	CELLTimestamp tamp;
	int status = BC_init(0x1a86, 0xFE00);
	if (status == 0) {
		printf("已经找到盒子 \t\n");
		BC_EnableMonitor(1);
		
		printf("打开数据监听 \t\n");
		while (true)
		{

			if (BC_Monitor_Mouse(0x01)) {
				printf("左键按下调用 BC_move(1,1);  %f \t\n", tamp.getElapsedTimeInMilliSec());
				BC_move(1,1);
			}

			if (BC_Monitor_Mouse(0x02)) {
				printf("右键键按下调用 	BC_move(-1, -1);   %f\t\n" , tamp.getElapsedTimeInMilliSec());
				BC_move(-1, -1);
			}

			if (BC_MonitorKeyboard(KEY_LEFTALT))
			{
				printf("KEY_LEFTALT键按下调用 BC_move(-1, -1); %f \t\n", tamp.getElapsedTimeInMilliSec());
				BC_move(-1, -1);
			}
		}
	}
	else {
		MessageBoxA(NULL,"没找到盒子","",MB_ICONERROR);
	}
	
}

/// <summary>
/// bcbox获取端点上传的无符号8位数数据的示例
/// </summary>
void get_Data_main()
{
	Sleep(1000);
	CELLTimestamp tamp;
	BC_init(0x1a86, 0xFE00);
	unsigned char buff[65] = { 0 };
	BC_Enable_Endp_upload(0x01);
	while (true)
	{
		sleep_hi(40);
		tamp.update();

		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			printf("%02x ", buff[i]);
		}

		printf("耗时 %.2f \t\n", tamp.getElapsedTimeInMilliSec());
	}
}


/// <summary>
/// bcbox获取端点连接情况的示例 如果已被正确识别并且连接 则对应显示0x02
/// </summary>
void Get_Endp_status()
{
	CELLTimestamp tamp;
	BC_init(0x1a86, 0xFE00);
	unsigned char buff[65] = { 0 };


	while (true)
	{
		sleep_hi(1000);
		tamp.update();

		BC_Get_EndP_Connection_Status(buff);
		printf("端点状态 : ");
		for (size_t i = 0; i < 20; i++)
		{
			printf("%02x ", buff[i]);
		}

		printf("耗时 %.2f \t\n", tamp.getElapsedTimeInMilliSec());
		if (GetAsyncKeyState(0x06))
		{

		}
	}
}

/// <summary>
/// bcbox循环检测盒子是否连接，如果没有连接 则自动连接
/// </summary>
void bc_is_connect()
{
	while (true)
	{
		bool status = BC_is_device_valid();
		printf("status %s \n", status == 1 ? "已连接" : "未连接");
		if (status == 0) {
			status = BC_init(0x1a86,0xfe00);
			if (status == 0) {
				BC_EnableMonitor(1); 
			}
		}

		Sleep(1000);
	}

}

void printhelp() {
	printf("以下是所有可用的函数列表及其描述：\n");
	printf("1. bc_Monitor_and_Move(): bcbox的循环示例，表示了如何检测按键状态，并且如何去移动鼠标\n");
	printf("2. get_Data_main(): bcbox获取端点上传的无符号8位数数据的示例\n");
	printf("3. Get_Endp_status(): bcbox获取端点连接情况的示例，如果已被正确识别并且连接，则对应显示0x02\n");
	printf("4. bc_is_connect(): bcbox循环检测盒子是否连接，如果没有连接，则自动连接\n");
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("没有提供足够的参数。请提供函数名称作为参数。\n");
		printhelp();
		return 1;
	}

	std::string arg = argv[1];
	if (arg == "bc_Monitor_and_Move") {
		bc_Monitor_and_Move();
	}
	else if (arg == "get_Data_main") {
		get_Data_main();
	}
	else if (arg == "Get_Endp_status") {
		Get_Endp_status();
	}
	else if (arg == "bc_is_connect") {
		bc_is_connect();
	}
	else {
		printf("未知的参数：%s\n", arg.c_str());
		printhelp();
		return 1;
	}

	return 0;
}
