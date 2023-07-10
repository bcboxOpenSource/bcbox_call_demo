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

#define USE_XY 1

bool stop_all = false;

CELLTimestamp g_tamp;
UINT last_status = 0x00;

#if USE_XY
std::vector<Press_xy_data> Press_vector; // 记录压枪数据
void load_csv(std::string path)
{
	//清空
	Press_vector.clear();
	std::ifstream csv_data(path, std::ios::in);

	std::string line;
	std::istringstream sin;         //将整行字符串line读入到字符串istringstream中
	std::string word;

	while (std::getline(csv_data, line))
	{
		sin.clear();
		sin.str(line);


		int index = 0;
		Press_xy_data data;
		while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
		{
			if (index == 0)
			{
				data.x = std::stoi(word);
				index++;
				continue;
			}

			if (index == 1)
			{
				data.y = std::stoi(word);
				index++;
				continue;
			}

			if (index == 2)
			{
				data.time_stamp = std::stoi(word);
				index++;
				continue;
			}
		}
		Press_vector.push_back(data);
	}
	csv_data.close();
}

void start_press()
{
	int last_time = 0;
	for (auto data : Press_vector) {
		if (BC_MonitorLeft()) {
			int now_gp = data.time_stamp - last_time;
			last_time = data.time_stamp;
			g_tamp.update();
			BC_move(data.x, data.y);
			now_gp -= g_tamp.getElapsedTimeInMilliSec();
			if (now_gp > 0)
				sleep_hi(now_gp);
		}
	}
	while (true)
	{
		if (BC_MonitorLeft() == 1) {
			printf("物理鼠标左键按下   %d \r\n", clock());
		}
		else {
			break;
		}
	}
}
#else
std::vector<Press_y_data> Press_vector_only_y; // 记录压枪数据
void load_csv_y(std::string path)
{
	//清空
	Press_vector_only_y.clear();
	std::ifstream csv_data(path, std::ios::in);

	std::string line;
	std::istringstream sin;         //将整行字符串line读入到字符串istringstream中
	std::string word;

	while (std::getline(csv_data, line))
	{
		sin.clear();
		sin.str(line);


		int index = 0;
		Press_y_data data;
		while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
		{
			if (index == 0)
			{
				data.y = std::stoi(word);
				//data.y = data.y
				index++;
				continue;
			}

			if (index == 1)
			{
				data.time_stamp = std::stoi(word);
				index++;
				continue;
			}
		}
		Press_vector_only_y.push_back(data);
		//Press_vector.push_back(data);
	}
	csv_data.close();
}

void start_press()
{
	int last_time = 0;
	for (auto data : Press_vector_only_y) {
		if (BC_MonitorLeft()) {
			int now_gp = data.time_stamp - last_time;
			last_time = data.time_stamp;
			g_tamp.update();
			BC_move(0, 0, data.y);
			now_gp -= g_tamp.getElapsedTimeInMilliSec();
			if (now_gp > 0)
				sleep_hi(now_gp);
		}
	}
	while (true)
	{
		if (BC_MonitorLeft() == 1) {
			printf("物理鼠标左键按下   %d \r\n", clock());
		}
		else {
			break;
		}
	}
}
#endif // USE_XY




void new_main()
{
	CELLTimestamp tamp;
	BC_init(0x1a86, 0xFE00);
	BC_EnableMonitor(1);
	while (true)
	{
		if (GetAsyncKeyState(0x06) || BC_Key_is_down(0x06))
		{
			tamp.update();

			//BC_move(2, 0, 1);
			BC_move(4, 0);
			printf("耗时 %.2f \t\n", tamp.getElapsedTimeInMilliSec());
		}
	}
}

short bytesToShort(const uint8_t value, const uint8_t direction)
{
	// 使用位移操作将字节转换为 short
	short result = direction << 8 | value;
	return result;
}

void get_mouse_Data_main()
{
	printf("%d \n", bytesToShort(0x00,0xff));
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
		//printf("鼠标状态 : ");
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

void Test_Flash_Mouse_Map()
{
	CELLTimestamp tamp;
	int status = BC_init(0x1a86, 0xFE00);
	printf("status %d \n", status);
	unsigned char buff[65] = { 0 };
	sleep_hi(50);
	BC_Get_Flash_data(buff);

	for (size_t i = 0; i < 20; i++)
	{
		printf("%02x ", buff[i]);
	}

	system("pause");
}

// 获取鼠标左键坐标
uint8_t Get_Mouse_status_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请按下鼠标左键 期间不要移动鼠标 \t\n");

	// 获取鼠标左键index
	while (true)
	{
		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			if (buff[i] == 0x01) {
				printf("%02x ", buff[i]);
				index_map[i]++;
			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 10) {
				printf("鼠标左键坐标 : mouse_status %d \n", (int)i);
				return i;
			}
		}
		sleep_hi(20);
	}
}

// 获取鼠标滚轮坐标
uint8_t Get_Mouse_wheel_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请滚动下滚轮 期间不要移动鼠标 \t\n");

	// 获取鼠标左键index
	while (true)
	{
		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			if (buff[i] == 0xff) {
				printf("%02x ", buff[i]);
				index_map[i]++;
			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 10) {
				printf("鼠标滚轮坐标 : wheel_status %d \n", (int)i);
				return i;
			}
		}
		sleep_hi(20);
	}
}

// 获取鼠标移动的坐标
void Get_Mouse_x_or_y_index_func(int type, uint8_t& xamount, uint8_t& xdirection)
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请缓慢向 %s 移动 \t\n", type == 0 ? "左" : "上");

	// 获取鼠标左键index
	while (true)
	{
		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			if (buff[i] == 0xff) {
				short value = bytesToShort(buff[i - 1], buff[i]);
				if (value < 0 && value > -5) {
					printf("%d ", value);
					index_map[i]++;
				}
				
			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 100) {
				xdirection = (int)i;
				xamount = (int)i - 1;
				return ;
			}
		}
		sleep_hi(20);
	}
}

#include "Bcbox_Mouse_adaptation/Bcbox_Mouse_adaptation.h"

// 鼠标适配操作
void Mouse_adaptation()
{

	Bcbox_Mouse_adaptation bma;
	bma.start_jianting();
	while (!bma.connect_status)
	{

	}
	bma.Get_Mouse_status_index_func();
	bma.Get_Mouse_wheel_index_func();
	bma.Get_Mouse_x_or_y_index_func(0);
	bma.Get_Mouse_x_or_y_index_func(1);
	bma.Save_data();

	system("pause");
	
}

int main()
{
	Mouse_adaptation();
	return 0;


	BC_init(0x1a86, 0xFE00);
	BC_Enable_Quickly_stop(1);
	//BC_Enable_AIM_Key_Move(1);

	while (true)
	{
		/*BC_move(0x02,0x09,0x00);
		Sleep(10);*/
	}

	

	Test_Flash_Mouse_Map();
	return 0;

	get_mouse_Data_main();
	return 0;

	Get_Endp_status();
	return 0;


	std::thread(get_mouse_Data_main).detach();
	std::thread(Get_Endp_status).detach();


	UINT a = 0x00;
	UINT b = 0x01;
	

	printf("0x%02x \t\n", a |= b);
	CELLTimestamp tamp;
	Sleep(23);
	printf("这是BCBOX4PORT的调用demo 请自己单步运行\n");
	if (BC_init(0x1a86, 0xFE00))
	{
		printf("找不到设备 \n");
		return 0;
	}

	printf("开启监听硬件键鼠按键状态\r\n");
	BC_EnableMonitor(1);
	int sum = 0;
	while (1)
	{
		if (BC_MonitorSide1() == 1) {

			////printf("物理鼠标侧键2键按下 调用鼠标移动  %d \r\n", clock());
			/*tamp.update();
			BC_move(2, 1, 0);
			sum += 1;
			printf("耗时 %.2f  sum%d \t\n", tamp.getElapsedTimeInMilliSec(), sum);*/

			

		}
		if (BC_MonitorSide2() == 1 || GetAsyncKeyState(0x06)) {
			tamp.update();
			sum += 4;
			printf("耗时 %.2f  sum%d \t\n", tamp.getElapsedTimeInMilliSec(), sum);
		}
		if (BC_MonitorKeyboard(KEY_A) == 1) {
			//printf("物理键盘a按下\r\n");
		}

		if (BC_MonitorKeyboard(KEY_LEFTSHIFT) == 1)
		{
			//BC_move(2, 4, 0);
			int status = BC_Enable_AIM_Key_Move(0x00);
			printf("键盘左Shift按下 status %d\r\n", status);
			
			Sleep(400);
		}
			

		if (BC_MonitorKeyboard(KEY_LEFTCONTROL) == 1 && BC_MonitorKeyboard(KEY_C) == 1)
		{

			last_status = !last_status;
			BC_Enable_Press(last_status);
			printf("你按下了ctrl + c 两个按键  %02x \r\n", last_status);
			Sleep(400);
		}


		if (BC_MonitorKeyboard(KEY_LEFTCONTROL) == 1 && BC_MonitorKeyboard(KEY_Z) == 1)
		{

			load_csv(".\\pressdata.csv");
			BC_Send_Press_Data(Press_vector);
			printf("你加载了数据 \r\n");
			Sleep(400);
		}

		

		//if (BC_MonitorKeyboard(KEY_LEFTCONTROL) == 1 && BC_MonitorKeyboard(KEY_V) == 1)
		//{
		//	
		//	stop_all = !stop_all;
		//	//printf("你按下了ctrl + V 两个按键  %02x \r\n", stop_all);
		//	Sleep(400);
		//}

		//printf("run ? \n");
		//sleep_hi(5);

	}

}

