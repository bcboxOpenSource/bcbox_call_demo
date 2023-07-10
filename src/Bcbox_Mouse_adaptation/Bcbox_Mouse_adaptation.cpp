#include "Bcbox_Mouse_adaptation.h"

Bcbox_Mouse_adaptation::Bcbox_Mouse_adaptation()
{
	int status_init = BC_init(0x1a86, 0xFE00);
	if (status_init == 0)
	{
		BC_Enable_Endp_upload(0x01); // 开启端点数据上传
	}


	
}



void OpenConsole()
{
	// 创建新的控制台窗口
	AllocConsole();

	// 将当前进程附加到控制台窗口
	AttachConsole(GetCurrentProcessId());

	// 获取标准输入、输出和错误输出的句柄
	HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE consoleError = GetStdHandle(STD_ERROR_HANDLE);

	// 设置标准输入、输出和错误输出的文件指针
	FILE* inputFile = nullptr;
	FILE* outputFile = nullptr;
	FILE* errorFile = nullptr;
	freopen_s(&inputFile, "CONIN$", "r", stdin);
	freopen_s(&outputFile, "CONOUT$", "w", stdout);
	freopen_s(&errorFile, "CONOUT$", "w", stderr);

	// 设置控制台的输入、输出代码页
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
}



void Bcbox_Mouse_adaptation::start_jianting()
{
	if (!this->Monitoring_status)
	{
		//OpenConsole();
		this->Monitoring_status = true;
	}

	// 启动一个线程 用于失去连接后重连
	std::thread([=]() {
		unsigned char buff[65] = { 0 };
		while (this->Monitoring_status)
		{
			// 设置连接状态
			this->connect_status = BC_is_device_valid();

			// 如果没有连接 尝试重连
			if (!this->connect_status)
			{

				printf("尝试重连 \n");
				int status_init = BC_init(0x1a86, 0xFE00);
				if (status_init == 0)
				{
					BC_Enable_Endp_upload(0x01); // 开启端点数据上传
				}
			}

			// 更新端口连接状态
			BC_Get_EndP_Connection_Status(buff);
			

		/*	for (int i = 0; i < 20; i++) {
				printf("%02x ", buff[i]);
			}

			printf("\n");*/
			
			this->endp1_status = buff[0] == 0x02;
			this->endp2_status = buff[1] == 0x02;
			/*if (this->connect_status)
			{
				
			}*/
		

			// 2s 更新一次
			Sleep(2000);
		}

		}).detach();
}

// 获取鼠标左键坐标
uint8_t Bcbox_Mouse_adaptation::Get_Mouse_status_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请按下鼠标中键 期间不要移动鼠标 \t\n");

	// 获取鼠标左键index
	while (true)
	{
		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			if (buff[i] == 0x04) {
				printf("%02x ", buff[i]);
				index_map[i]++;
				break;
			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 20) {
				printf("鼠标中键坐标 : mouse_status %d \n", (int)i);
				this->mouse_status = i; // 设置鼠标状态的坐标
				return i;
			}
		}
		sleep_hi(20);
	}
}

// 获取鼠标滚轮坐标
uint8_t Bcbox_Mouse_adaptation::Get_Mouse_wheel_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请滚动下滚轮 期间不要移动鼠标 \t\n");

	while (true)
	{
		BC_Get_Mouse_Data(buff);
		for (size_t i = 0; i < 20; i++)
		{
			if (buff[i] == 0xff) {
				printf("%02x ", buff[i]);
				index_map[i]++;
				break;
			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 10) {
				printf("鼠标滚轮坐标 : wheel_status %d \n", (int)i);
				this->wheel_status = i; // 设置鼠标滚轮的坐标
				return i;
			}
		}
		sleep_hi(20);
	}
}

// 获取鼠标移动的坐标
void Bcbox_Mouse_adaptation::Get_Mouse_x_or_y_index_func(int type)
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // 记录数组
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("请缓慢向 %s 移动 \t\n", type == 0 ? "左 x" : "上 y");

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
					break;
				}

			}

		}

		// 判断是否有一个到达了10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 50) {
				if (type == 0)
				{
					this->xdirection = (int)i;
					this->xamount = (int)i - 1;
				}
				else 
				{
					this->ydirection = (int)i;
					this->yamount = (int)i - 1;
				}
				
				return;
			}
		}
		sleep_hi(20);
	}
}

void Bcbox_Mouse_adaptation::Save_data()
{
	printf("写入的数据 %02x %02x %02x %02x %02x %02x \n", mouse_status, xamount, xdirection, yamount, ydirection, wheel_status);
	BC_Set_Mouse_Map(0x01, mouse_status, xamount, xdirection, yamount, ydirection, wheel_status);
	printf("数据写入完成 现在按下复位键 查看是否正常工作! \n");
}



short Bcbox_Mouse_adaptation::bytesToShort(const uint8_t value, const uint8_t direction)
{
	// 使用位移操作将字节转换为 short
	short result = direction << 8 | value;
	return result;
}