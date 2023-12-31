﻿#include "windows.h"
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
#include "bcbox_simple_call/bcbox_simple_call.h"


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


void get_mouse_info()
{
	BC_init(0x1a86, 0xfe00);
	unsigned char buff[65] = { 0 };
	BC_Get_Mouse_report_desc(buff, 0);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Mouse_report_desc(buff, 1);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Mouse_report_desc(buff, 2);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Mouse_map(buff);

	printf("\n");
	for (int i = 0x00; i < 6; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");
}

void get_kbd_info()
{
	BC_init(0x1a86, 0xfe00);
	unsigned char buff[65] = { 0 };
	BC_Get_Kbd_report_desc(buff, 0);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Kbd_report_desc(buff, 1);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Kbd_report_desc(buff, 2);

	for (int i = 0x00; i < 64; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	BC_Get_Kbd_map(buff);

	printf("\n");
	for (int i = 0x00; i < 6; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");
}



void Send_kbd_data()
{
	int status = BC_init(0x1a86, 0xFE00);
	if (status == 0) {
		printf("已经找到盒子 \t\n");

		unsigned char key[6] = { 0 };  //默认任何按键不按下
		unsigned char ctrbtn = 0;      //默认全部控制按键均松开
		BC_keyboard(ctrbtn, key); 

		ctrbtn = BIT0;
		key[0] = KEY_A;

		BC_keyboard(ctrbtn, key); // ctrl + A 键按下
		printf("ctrl + A 键按下 \t\n");

		Sleep(200);
		
		ctrbtn = 0;
		key[0] = 0;

		BC_keyboard(ctrbtn, key); // ctrl + A 键按下
		printf("ctrl + A 松开 \t\n");

	}
	else {
		MessageBoxA(NULL,"没找到盒子","",MB_ICONERROR);
	}
}

void Move_Onec()
{
	int status = BC_init(0x1a86, 0xFE00);
	if (status == 0) {
		printf("已经找到盒子 \t\n");

		BC_move(0x10,0x20);

	}
	else {
		MessageBoxA(NULL, "没找到盒子", "", MB_ICONERROR);
	}
}

void sk_left() {
	int status = BC_init(0x1a86, 0xFE00);
	if (status == 0) {
		printf("已经找到盒子 \t\n");

		BC_left(1);

	}
	else {
		MessageBoxA(NULL, "没找到盒子", "", MB_ICONERROR);
	}
}

void printhelp() {
	printf("以下是所有可用的函数列表及其描述：\n");
	printf("1. bc_Monitor_and_Move  (): bcbox的循环示例，表示了如何检测按键状态，并且如何去移动鼠标\n");
	printf("2. get_Data_main        (): bcbox获取端点上传的无符号8位数数据的示例\n");
	printf("3. Get_Endp_status      (): bcbox获取端点连接情况的示例，如果已被正确识别并且连接，则对应显示0x02\n");
	printf("4. bc_is_connect        (): bcbox循环检测盒子是否连接，如果没有连接，则自动连接\n");
	printf("5. get_mouse_info       (): 获取鼠标信息\n");
	printf("6. get_kbd_info         (): 获取键盘信息\n");
	printf("7. Send_kbd_data        (): 发送键盘数据\n");
	printf("8. Move_Onec		(): 移动一次鼠标\n");
}


// 易语言调用bcbox的示例
void e_lang_call()
{
	bool status = bcbox_simple_init();

	if (status)
	{
		/*printf("打开成功 调用移动 \t\n");
		bcbox_simple_move(300, 30);*/
		bcbox_simple_enable_monitor();
		bcbox_left_is_down();
	}

}

int main(int argc, char* argv[])
{
	e_lang_call();
	return 0;
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
	else if (arg == "get_mouse_info") {
		get_mouse_info();
	}
	else if (arg == "get_kbd_info") {
		get_kbd_info();
	}
	else if (arg == "Send_kbd_data") {
		Send_kbd_data();
	}
	else if (arg == "Move_Onec") {
		Move_Onec();
	}
	else {
		printf("未知的参数：%s\n", arg.c_str());
		printhelp();
		return 1;
	}

	return 0;
}
