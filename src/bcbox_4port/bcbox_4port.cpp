
#include "bcbox_4port.h"

hid_device * fd_bcbox;		//设备句柄
static t_status_BC bc_status;	//键鼠状态			-----全局变量 
HANDLE bc_m_hMutex_lock_write=NULL;  //多线程互斥锁		
HANDLE bc_m_hMutex_lock_read=NULL;  //多线程互斥锁		


// 高精度延时
void sleep_hi(int i)
{

	auto begint = clock();
	while (clock() - begint < i)
	{
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}

/*
必须首先调用此函数才能与盒子通信
输入：   VID，PID 板子的硬件ID值，直接显示屏上可以看到
返回值： 
		-1：找不到指定的VID和PID
		0:正常

测试OK
*/
int BC_init(unsigned short vid,unsigned short pid)
{
	hid_device_info *hid_info;
	if(bc_m_hMutex_lock_write ==NULL)
	{
		bc_m_hMutex_lock_write = CreateMutex(NULL,TRUE,"busy_write");
	}

	if (bc_m_hMutex_lock_read == NULL)
	{
		bc_m_hMutex_lock_read = CreateMutex(NULL, TRUE, "busy_read");
	}
	 
	ReleaseMutex(bc_m_hMutex_lock_write);
	ReleaseMutex(bc_m_hMutex_lock_read);

 	hid_info=hid_enumerate(vid,pid);

	do{
		if(hid_info==NULL) return -1;
		if(hid_info->usage_page==0xff00)
		{
			break;
		}else
			hid_info =hid_info->next;
	}while(1);

	fd_bcbox = hid_open_path(hid_info->path);//
	if (!fd_bcbox) {
		fd_bcbox=NULL;
		return -1;
	}
	return 0;
}

int BC_is_device_valid() {
	if (fd_bcbox == NULL)
	{
		return 0;
	}
	unsigned char buffer[65];

	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	int i = hid_write(fd_bcbox, (const unsigned char*)&buffer, sizeof(buffer));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 1 : 0;
}




static struct cmd_getregcode_t
{
	unsigned char reserved;
	unsigned char cmd;		//0x00
	unsigned char data[64];	//0X00000
}data_getRegcode;




static struct mouse_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x03
	unsigned char port;  //向哪个口发数据
	unsigned char button;//按键消息
	short x;//x
	short y;//y
	unsigned char wheel;//滚轮
	unsigned char reserv[60];//0X00000
}data_mouse = {0};

/*
/*
鼠标左键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标左键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test

*/
int BC_left(char port,unsigned char vk_key)
{	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd=0x03;
	data_mouse.port=port;
	if(vk_key)	
		data_mouse.button |=BIT0; //左键按下
	else 
		data_mouse.button &=~BIT0;//左键松开
	data_mouse.x=0;
	data_mouse.y=0;
	data_mouse.wheel=0;
	i=hid_write(fd_bcbox,(const unsigned char *)&data_mouse,65);

	if (i == 65)
	{
		if ((port & 0x03)==0x03)//向两个端口发送数据最好延迟2ms.保证对应的PC口数据被拾取。
			sleep_hi(2);
		else
			sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}

/*
/*
鼠标中键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标中键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
*/
int BC_middle(char port,unsigned char vk_key)
{	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd=0x03;
	data_mouse.port=port;
	if(vk_key)	
		data_mouse.button |=BIT2;
	else 
		data_mouse.button &=~BIT2;
	data_mouse.x=0;
	data_mouse.y=0;
	data_mouse.wheel=0;
	i=hid_write(fd_bcbox,(const unsigned char *)&data_mouse,65);
	if (i == 65)
	{
		if ((port & 0x03) == 0x03)//向两个端口发送数据最好延迟2ms.保证对应的PC口数据被拾取。
			sleep_hi(2);
		else
			sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}


/*
鼠标右键控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标右键状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
*/
int BC_right(char port,unsigned char vk_key)
{	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd=0x03;
	data_mouse.port=port;
	if(vk_key)	
		data_mouse.button |=BIT1;
	else 
		data_mouse.button &=~BIT1;
	data_mouse.x=0;
	data_mouse.y=0;
	data_mouse.wheel=0;
	i=hid_write(fd_bcbox,(const unsigned char *)&data_mouse,65);
	if (i == 65)
	{
		if ((port & 0x03) == 0x03)//向两个端口发送数据最好延迟2ms.保证对应的PC口数据被拾取。
			sleep_hi(2);
		else
			sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}


/*
鼠标移动控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
x		:x方向相对偏移量[-32767,32767]
y		:y方向相对偏移量[-32767,32767]
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
*/
int BC_move(short x, short y)
{
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	int i;
	data_mouse.reserved = 0X00;
	data_mouse.cmd = 0x03;
	data_mouse.port = 0x00; // 当前控制哪台电脑 就在哪台电脑发数据
	data_mouse.x = x;
	data_mouse.y = y;
	data_mouse.wheel = 0;
	
	i = hid_write(fd_bcbox, (const unsigned char*)&data_mouse, 65);

	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

/*
鼠标侧键1控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标侧键1状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
*/

int BC_side1(char port, unsigned char  vk_key)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd = 0x03;
	data_mouse.port = port;
	if (vk_key)
		data_mouse.button |= BIT3;
	else
		data_mouse.button &= ~BIT3;
	data_mouse.x = 0;
	data_mouse.y = 0;
	data_mouse.wheel = 0;
	i = hid_write(fd_bcbox, (const unsigned char*)&data_mouse, 65);
	if (i == 65)
	{
		if ((port & 0x03) == 0x03)//向两个端口发送数据最好延迟2ms.保证对应的PC口数据被拾取。
			sleep_hi(2);
		else
			sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}
/*
鼠标侧键2控制：
port	:往哪个端口发数据,只往port1发数据为1,只往port2发数据为2，两个端口都发数据为3
vk_key  :鼠标侧键2状态设置，0：松开  1：按下
返回值：
		-1：发送失败
		 0：发送成功
是否测试：是 --test
*/
int BC_side2(char port, unsigned char  vk_key)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd = 0x03;
	data_mouse.port = port;
	if (vk_key)
		data_mouse.button |= BIT4;
	else
		data_mouse.button &= ~BIT4;
	data_mouse.x = 0;
	data_mouse.y = 0;
	data_mouse.wheel = 0;
	i = hid_write(fd_bcbox, (const unsigned char*)&data_mouse, 65);
	if (i == 65)
	{
		if ((port & 0x03) == 0x03)//向两个端口发送数据最好延迟2ms.保证对应的PC口数据被拾取。
			sleep_hi(2);
		else
			sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}



//大端序转为小端序
void swapEndian(uint8_t* data, size_t size) {
	for (size_t i = 0; i < size / 2; ++i) {
		uint8_t temp = data[i];
		data[i] = data[size - 1 - i];
		data[size - 1 - i] = temp;
	}
}

// 只有结构为y的压枪数组
static struct press_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x10 命令符号 标识只有y坐标和时间戳类型的数据
	unsigned char date_num;   // 标识当前一共多少段数据
	unsigned char date_index;   // 标识第几段
	unsigned char end_status;   // 标识是否结束
	short reserv[28];//  传递的有符号的数据 偶数位为压枪幅度 奇数位为压枪时间戳
	unsigned char rr[4];   // 0x000000
}data_press = { 0 };


// 发送结构为y的压枪数组
int BC_Send_Press_Data(std::vector<Press_y_data> press_data)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	//结构体中实际的大小
	int reserv_size = sizeof(data_press.reserv) / 2;

	// 计算需要发送的压枪数据 2是因为结构中数据为2
	int send_num = press_data.size() * 2;

	// 计算需要发送的次数
	int send_count = (send_num / reserv_size) + (((send_num % reserv_size) > 0) ? 1 : 0);


	BC_PRINTF("reserv_size %d send_num %d  send_count %d \n", reserv_size, send_num, send_count);

	
	// 一次发送最多存储 30 / 2 也就是15次压枪数据 一份当中存储着 y time_stamp
	for (int i = 0; i < send_count; i++)
	{
		data_press.reserved = 0x00;
		data_press.cmd = 0x10;
		data_press.date_num = send_count;
		data_press.date_index = i + 1;
		data_press.end_status = (i + 1) == send_count;

		// 相当于30次循环 是因为 一次数据包大小是30 但是 偶数位为压枪幅度 奇数位为压枪时间戳
		for (int j = 0; j < sizeof(data_press.reserv) / 2; j+= 2) {
			int now_press_data_index = j / 2 + (data_press.date_index - 1) * reserv_size / 2;
			if (now_press_data_index >= press_data.size())
			{
				continue;
			}
			BC_PRINTF("data_index %d  j %d \t\n", now_press_data_index, j);
			data_press.reserv[j] = press_data[now_press_data_index].y;
			data_press.reserv[j+1] = press_data[now_press_data_index].time_stamp;

			// 将数据转换为小端序
			swapEndian(reinterpret_cast<uint8_t*>(&data_press.reserv[j]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_press.reserv[j + 1]), sizeof(short));
		}

		int return_count = hid_write(fd_bcbox, reinterpret_cast<const unsigned char*>(&data_press), sizeof(data_press));

		// 发送过后清理数据
		memset(data_press.reserv,0,sizeof(data_press.reserv));

		BC_PRINTF("return_count %d \t\n", return_count);
		if (return_count == sizeof(data_press))
		{
			sleep_hi(1); // 向一个端口发数据延迟1ms
		}
	}
	

	ReleaseMutex(bc_m_hMutex_lock_write);
	return 0;
}


// 结构带xy的数组
static struct press_xy_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x12 命令符号 标识带xy的压枪数组
	unsigned char date_num;   // 标识当前一共多少段数据
	unsigned char date_index;   // 标识第几段
	unsigned char end_status;   // 标识是否结束
	short reserv[27];//  x y time  传递的有符号的数据 偶数位为压枪幅度 奇数位为压枪时间戳
}data_xy_press = { 0 };


// 发送结构带xy的压枪数组
int BC_Send_Press_Data(std::vector<Press_xy_data> press_data)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	//结构体中实际的大小
	int reserv_size = sizeof(data_xy_press.reserv) / 2;

	// 计算需要发送的压枪数据 3是因为结构中数据长度为3
	int send_num = press_data.size() * 3;

	// 计算需要发送的次数
	int send_count = (send_num / reserv_size) + (((send_num % reserv_size) > 0) ? 1 : 0);


	BC_PRINTF("reserv_size xy %d send_num %d  send_count %d \n", reserv_size, send_num, send_count);


	// 一次发送最多存储 30 / 3 也就是10 次压枪数据 一份当中存储着 x y time_stamp
	for (int i = 0; i < send_count; i++)
	{
		data_xy_press.reserved = 0x00;
		data_xy_press.cmd = 0x12; // 带xy方向的
		data_xy_press.date_num = send_count;
		data_xy_press.date_index = i + 1;
		data_xy_press.end_status = (i + 1) == send_count;

		// 相当于30次循环 是因为 一次数据包大小是30  数据格式为 0 x 1 y 2 time
		for (int j = 0; j < (sizeof(data_xy_press.reserv) / 3 / 2); j ++) {
			int now_press_data_index = j + (data_xy_press.date_index - 1) * reserv_size / 3;
			if (now_press_data_index >= press_data.size())
			{
				BC_PRINTF("continue %d  j %d  value %d \t\n", now_press_data_index, j, (data_xy_press.date_index - 1) * reserv_size / 3);
				continue;
			}
			BC_PRINTF("data_index %d  j %d \t\n", now_press_data_index, j);

			int now_start_index = j * 3;
			data_xy_press.reserv[now_start_index] = press_data[now_press_data_index].x;
			data_xy_press.reserv[now_start_index + 1] = press_data[now_press_data_index].y;
			data_xy_press.reserv[now_start_index + 2] = press_data[now_press_data_index].time_stamp;

			// 将数据转换为小端序
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index + 1]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index + 2]), sizeof(short));
		}

		int return_count = hid_write(fd_bcbox, reinterpret_cast<const unsigned char*>(&data_xy_press), sizeof(data_xy_press));

		// 发送过后清理数据
		memset(data_xy_press.reserv, 0, sizeof(data_xy_press.reserv));

		BC_PRINTF("return_count %d \t\n", return_count);
		if (return_count == sizeof(data_xy_press))
		{
			sleep_hi(1); // 向一个端口发数据延迟1ms
		}
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 0;
}

static struct controller_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x11 控制符号
	unsigned char status;   //0x11 控制符号
	unsigned char reserv[63];
}controller_press = { 0x00 };

int BC_Enable_Press(unsigned char status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	controller_press.reserved = 0X00;
	controller_press.cmd = 0x11;
	controller_press.status = status;
	i = hid_write(fd_bcbox, (const unsigned char*)&controller_press, 65);
	if (i == 65)
	{
		sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

// 是否开启自瞄键不移动
int BC_Enable_AIM_Key_Move(unsigned char status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	controller_press.reserved = 0X00;
	controller_press.cmd = 0x13;
	controller_press.status = status;
	i = hid_write(fd_bcbox, (const unsigned char*)&controller_press, 65);
	if (i == 65)
	{
		sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


// 获取鼠标数据
int BC_Get_Mouse_Data(unsigned char* addr)
{
	unsigned char buff[65] = { 0 };
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_getRegcode.reserved = 0x00;
	data_getRegcode.cmd = 0x14;
	hid_write(fd_bcbox, (const unsigned char*)&data_getRegcode, 65);
	hid_read_timeout(fd_bcbox, buff, 65, 1000);

	memcpy(addr, buff, sizeof(buff));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 1;
}


static struct hex16_pack
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x16 处理多项流程控制
	unsigned char select;   //选择
	unsigned char status;   //状态
	unsigned char send_data[30];
	unsigned char reserv[32];
}hex16_pack_data = { 0x00 };

// 开启端点上传 开启后方可获取鼠标数据
int BC_Enable_Endp_upload(unsigned char status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	hex16_pack_data.reserved = 0X00;
	hex16_pack_data.cmd = 0x16;
	hex16_pack_data.select = 0x00;
	hex16_pack_data.status = status;
	i = hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	if (i == 65)
	{
		sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


// 获取二个端点的连接状态
int BC_Get_EndP_Connection_Status(unsigned char* addr)
{
	unsigned char buff[65] = { 0 };
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	hex16_pack_data.reserved = 0x00;
	hex16_pack_data.cmd = 0x17; // 获取数据的cmd标识符
	hex16_pack_data.select = 0x00;
	hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	hid_read_timeout(fd_bcbox, buff, 65, 1000);

	memcpy(addr, buff, sizeof(buff));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 1;
}


// 设置鼠标映射
int BC_Set_Mouse_Map(uint8_t enable_status, uint8_t mouse_status, uint8_t xamount, uint8_t xdirection, uint8_t yamount, uint8_t ydirection, uint8_t wheel_status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	hex16_pack_data.reserved = 0X00;
	hex16_pack_data.cmd = 0x16; // 统一标识符
	hex16_pack_data.select = 0x01; // 标识设置鼠标映射
	hex16_pack_data.status = enable_status; // 开启状态
	hex16_pack_data.send_data[0] = mouse_status;
	hex16_pack_data.send_data[1] = xamount;
	hex16_pack_data.send_data[2] = xdirection;
	hex16_pack_data.send_data[3] = yamount;
	hex16_pack_data.send_data[4] = ydirection;
	hex16_pack_data.send_data[5] = wheel_status;
	i = hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	if (i == 65)
	{
		sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

int BC_Get_Flash_data(unsigned char* addr)
{
	unsigned char buff[65] = { 0 };
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	hex16_pack_data.reserved = 0x00;
	hex16_pack_data.cmd = 0x17; // 获取数据的cmd标识符
	hex16_pack_data.select = 0x01; // 获取flash data

	hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	hid_read_timeout(fd_bcbox, buff, 65, 1000);

	memcpy(addr, buff, sizeof(buff));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 1;
}


// 是否开启板子自带急停
int BC_Enable_Quickly_stop(unsigned char status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	controller_press.reserved = 0X00;
	controller_press.cmd = 0x15;
	controller_press.status = status;
	i = hid_write(fd_bcbox, (const unsigned char*)&controller_press, 65);
	if (i == 65)
	{
		sleep_hi(1);//向一个端口发数据延迟1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


//键盘报告
static struct keyboard_t
{
	unsigned char reserved;
	unsigned char cmd;		//0x00
	unsigned char port;		//端口
	unsigned char ctrButton;//控制按键
	unsigned char data[61];	//0X00000
}data_keyboard = {0};





// 函数封装  只是对应windows key map
int BC_Key_is_down(int vk_key)
{
	if (vk_key == 1)
		return BC_MonitorLeft();
	else if (vk_key == 2)
		return BC_MonitorRight();
	else if (vk_key == 3)
		return BC_MonitorMiddle();
	else if (vk_key == 5)
		return BC_MonitorSide1();
	else if (vk_key == 6)
		return BC_MonitorSide2();
	return 0;
}




static HANDLE handle_listen = NULL;

static struct cmd_listen_t
{
	unsigned char reserved;
	unsigned char cmd;		//0x07
	unsigned char enable = 0x00;   //是否开启监听
	unsigned char data[64];	//0X00000
}data_listen_t;

//监听物理键鼠

DWORD WINAPI BC_ThreadListenProcess(LPVOID lpParameter)
{
	t_status_BC ret;
	int i=0;
	while (data_listen_t.enable)
	{	
		sleep_hi(1); //每隔1ms读取一次 高精度延时
		WaitForSingleObject(bc_m_hMutex_lock_read, INFINITE); // Lock the mutex here
		i=hid_read_timeout(fd_bcbox, ret.buf, 64, 10); 

		if(i == 64)
		{
			memcpy(&bc_status,ret.buf,64);
		}
		
		

		ReleaseMutex(bc_m_hMutex_lock_read);
	}
	handle_listen = NULL;//关闭监听
	return 0;
}

/*

打开或者关闭物理键鼠按键监听功能
enable 		:0 关闭监听  1：打开监听
打开后鼠标按键状态在mStatus中
键盘按键状态在kStatus中。
*/
int BC_EnableMonitor(char enable){
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_listen_t.reserved = 0x00;
	data_listen_t.cmd = 0x07;
	data_listen_t.enable = enable; //是否使能标志
	i=hid_write(fd_bcbox, (const unsigned char*)&data_listen_t, 65);
	if (i == 65) Sleep(1);
	memset(bc_status.buf, 0, sizeof(bc_status.buf));

	if (enable)//打开监听功能
	{	if (handle_listen == NULL)
		{
			DWORD lpThreadID;
			handle_listen = CreateThread(NULL, 0, BC_ThreadListenProcess, NULL, 0, &lpThreadID);
		}
	}

	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 64 ? 0 : -1;
}

/*
查询物理鼠标左键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标左键松开
			1：物理鼠标左键按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorLeft()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x01 ? 1 : 0;
}

/*
查询物理鼠标右键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标右键松开
			1：物理鼠标右键按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorRight()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x02 ? 1 : 0;
}

/*
查询物理鼠标中键状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标中键松开
			1：物理鼠标中键按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorMiddle()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x04 ? 1 : 0;
}

/*
查询物理鼠标侧键1状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标侧键1松开
			1：物理鼠标侧键1按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorSide1()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x08 ? 1 : 0;
}


/*
查询物理鼠标侧键2状态   --只有开启了BC_EnableMonitor（1）后才能使用
返回值		0：物理鼠标侧键2松开
			1：物理鼠标侧键2按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorSide2()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x10 ? 1 : 0;
}


// 请注意vk跟windows本身的vk值并不相同
int BC_Monitor_Mouse(int vk)
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & vk ? 1 : 0;
}

/*
查询物理键盘指定按键是否按下 --只有开启了BC_EnableMonitor（1）后才能使用
输入：		vk_key 要查询的HID键值  详见HID键值表

返回值		0：vk_key松开
			1：vk_key按下
			-1：监听功能未开启
是否测试：20230220 ok
*/
int BC_MonitorKeyboard(int vk_key)
{
	if (data_listen_t.enable == 0) return -1;//
	if (vk_key >= KEY_LEFTCONTROL && vk_key <= KEY_RIGHT_GUI)//控制键
	{	switch (vk_key)
		{
		case KEY_LEFTCONTROL: return bc_status.km.kstatus[0] & BIT0 ? 1 : 0;
		case KEY_LEFTSHIFT:   return bc_status.km.kstatus[0] & BIT1 ? 1 : 0;
		case KEY_LEFTALT:     return bc_status.km.kstatus[0] & BIT2 ? 1 : 0;
		case KEY_LEFT_GUI:    return bc_status.km.kstatus[0] & BIT3 ? 1 : 0;
		case KEY_RIGHTCONTROL:return bc_status.km.kstatus[0] & BIT4 ? 1 : 0;
		case KEY_RIGHTSHIFT:  return bc_status.km.kstatus[0] & BIT5 ? 1 : 0;
		case KEY_RIGHTALT:    return bc_status.km.kstatus[0] & BIT6 ? 1 : 0;
		case KEY_RIGHT_GUI:   return bc_status.km.kstatus[0] & BIT7 ? 1 : 0;
		}
	}
	else//常规键
	{	for (int i = 1; i < 11; i++)
		{	if (bc_status.km.kstatus[i] == vk_key)
			{
				return 1;
			}
		}
	}
	return 0;
}

