
#include "bcbox_4port.h"

hid_device * fd_bcbox;		//�豸���
static t_status_BC bc_status;	//����״̬			-----ȫ�ֱ��� 
HANDLE bc_m_hMutex_lock_write=NULL;  //���̻߳�����		
HANDLE bc_m_hMutex_lock_read=NULL;  //���̻߳�����		


// �߾�����ʱ
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
�������ȵ��ô˺������������ͨ��
���룺   VID��PID ���ӵ�Ӳ��IDֵ��ֱ����ʾ���Ͽ��Կ���
����ֵ�� 
		-1���Ҳ���ָ����VID��PID
		0:����

����OK
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
	unsigned char port;  //���ĸ��ڷ�����
	unsigned char button;//������Ϣ
	short x;//x
	short y;//y
	unsigned char wheel;//����
	unsigned char reserv[60];//0X00000
}data_mouse = {0};

/*
/*
���������ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :������״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test

*/
int BC_left(char port,unsigned char vk_key)
{	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_mouse.reserved = 0X00;
	data_mouse.cmd=0x03;
	data_mouse.port=port;
	if(vk_key)	
		data_mouse.button |=BIT0; //�������
	else 
		data_mouse.button &=~BIT0;//����ɿ�
	data_mouse.x=0;
	data_mouse.y=0;
	data_mouse.wheel=0;
	i=hid_write(fd_bcbox,(const unsigned char *)&data_mouse,65);

	if (i == 65)
	{
		if ((port & 0x03)==0x03)//�������˿ڷ�����������ӳ�2ms.��֤��Ӧ��PC�����ݱ�ʰȡ��
			sleep_hi(2);
		else
			sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}

/*
/*
����м����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :����м�״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
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
		if ((port & 0x03) == 0x03)//�������˿ڷ�����������ӳ�2ms.��֤��Ӧ��PC�����ݱ�ʰȡ��
			sleep_hi(2);
		else
			sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}


/*
����Ҽ����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :����Ҽ�״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
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
		if ((port & 0x03) == 0x03)//�������˿ڷ�����������ӳ�2ms.��֤��Ӧ��PC�����ݱ�ʰȡ��
			sleep_hi(2);
		else
			sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i==65?0:-1;
}


/*
����ƶ����ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
x		:x�������ƫ����[-32767,32767]
y		:y�������ƫ����[-32767,32767]
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
*/
int BC_move(short x, short y)
{
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	int i;
	data_mouse.reserved = 0X00;
	data_mouse.cmd = 0x03;
	data_mouse.port = 0x00; // ��ǰ������̨���� ������̨���Է�����
	data_mouse.x = x;
	data_mouse.y = y;
	data_mouse.wheel = 0;
	
	i = hid_write(fd_bcbox, (const unsigned char*)&data_mouse, 65);

	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

/*
�����1���ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :�����1״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
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
		if ((port & 0x03) == 0x03)//�������˿ڷ�����������ӳ�2ms.��֤��Ӧ��PC�����ݱ�ʰȡ��
			sleep_hi(2);
		else
			sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}
/*
�����2���ƣ�
port	:���ĸ��˿ڷ�����,ֻ��port1������Ϊ1,ֻ��port2������Ϊ2�������˿ڶ�������Ϊ3
vk_key  :�����2״̬���ã�0���ɿ�  1������
����ֵ��
		-1������ʧ��
		 0�����ͳɹ�
�Ƿ���ԣ��� --test
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
		if ((port & 0x03) == 0x03)//�������˿ڷ�����������ӳ�2ms.��֤��Ӧ��PC�����ݱ�ʰȡ��
			sleep_hi(2);
		else
			sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}



//�����תΪС����
void swapEndian(uint8_t* data, size_t size) {
	for (size_t i = 0; i < size / 2; ++i) {
		uint8_t temp = data[i];
		data[i] = data[size - 1 - i];
		data[size - 1 - i] = temp;
	}
}

// ֻ�нṹΪy��ѹǹ����
static struct press_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x10 ������� ��ʶֻ��y�����ʱ������͵�����
	unsigned char date_num;   // ��ʶ��ǰһ�����ٶ�����
	unsigned char date_index;   // ��ʶ�ڼ���
	unsigned char end_status;   // ��ʶ�Ƿ����
	short reserv[28];//  ���ݵ��з��ŵ����� ż��λΪѹǹ���� ����λΪѹǹʱ���
	unsigned char rr[4];   // 0x000000
}data_press = { 0 };


// ���ͽṹΪy��ѹǹ����
int BC_Send_Press_Data(std::vector<Press_y_data> press_data)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	//�ṹ����ʵ�ʵĴ�С
	int reserv_size = sizeof(data_press.reserv) / 2;

	// ������Ҫ���͵�ѹǹ���� 2����Ϊ�ṹ������Ϊ2
	int send_num = press_data.size() * 2;

	// ������Ҫ���͵Ĵ���
	int send_count = (send_num / reserv_size) + (((send_num % reserv_size) > 0) ? 1 : 0);


	BC_PRINTF("reserv_size %d send_num %d  send_count %d \n", reserv_size, send_num, send_count);

	
	// һ�η������洢 30 / 2 Ҳ����15��ѹǹ���� һ�ݵ��д洢�� y time_stamp
	for (int i = 0; i < send_count; i++)
	{
		data_press.reserved = 0x00;
		data_press.cmd = 0x10;
		data_press.date_num = send_count;
		data_press.date_index = i + 1;
		data_press.end_status = (i + 1) == send_count;

		// �൱��30��ѭ�� ����Ϊ һ�����ݰ���С��30 ���� ż��λΪѹǹ���� ����λΪѹǹʱ���
		for (int j = 0; j < sizeof(data_press.reserv) / 2; j+= 2) {
			int now_press_data_index = j / 2 + (data_press.date_index - 1) * reserv_size / 2;
			if (now_press_data_index >= press_data.size())
			{
				continue;
			}
			BC_PRINTF("data_index %d  j %d \t\n", now_press_data_index, j);
			data_press.reserv[j] = press_data[now_press_data_index].y;
			data_press.reserv[j+1] = press_data[now_press_data_index].time_stamp;

			// ������ת��ΪС����
			swapEndian(reinterpret_cast<uint8_t*>(&data_press.reserv[j]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_press.reserv[j + 1]), sizeof(short));
		}

		int return_count = hid_write(fd_bcbox, reinterpret_cast<const unsigned char*>(&data_press), sizeof(data_press));

		// ���͹�����������
		memset(data_press.reserv,0,sizeof(data_press.reserv));

		BC_PRINTF("return_count %d \t\n", return_count);
		if (return_count == sizeof(data_press))
		{
			sleep_hi(1); // ��һ���˿ڷ������ӳ�1ms
		}
	}
	

	ReleaseMutex(bc_m_hMutex_lock_write);
	return 0;
}


// �ṹ��xy������
static struct press_xy_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x12 ������� ��ʶ��xy��ѹǹ����
	unsigned char date_num;   // ��ʶ��ǰһ�����ٶ�����
	unsigned char date_index;   // ��ʶ�ڼ���
	unsigned char end_status;   // ��ʶ�Ƿ����
	short reserv[27];//  x y time  ���ݵ��з��ŵ����� ż��λΪѹǹ���� ����λΪѹǹʱ���
}data_xy_press = { 0 };


// ���ͽṹ��xy��ѹǹ����
int BC_Send_Press_Data(std::vector<Press_xy_data> press_data)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	//�ṹ����ʵ�ʵĴ�С
	int reserv_size = sizeof(data_xy_press.reserv) / 2;

	// ������Ҫ���͵�ѹǹ���� 3����Ϊ�ṹ�����ݳ���Ϊ3
	int send_num = press_data.size() * 3;

	// ������Ҫ���͵Ĵ���
	int send_count = (send_num / reserv_size) + (((send_num % reserv_size) > 0) ? 1 : 0);


	BC_PRINTF("reserv_size xy %d send_num %d  send_count %d \n", reserv_size, send_num, send_count);


	// һ�η������洢 30 / 3 Ҳ����10 ��ѹǹ���� һ�ݵ��д洢�� x y time_stamp
	for (int i = 0; i < send_count; i++)
	{
		data_xy_press.reserved = 0x00;
		data_xy_press.cmd = 0x12; // ��xy�����
		data_xy_press.date_num = send_count;
		data_xy_press.date_index = i + 1;
		data_xy_press.end_status = (i + 1) == send_count;

		// �൱��30��ѭ�� ����Ϊ һ�����ݰ���С��30  ���ݸ�ʽΪ 0 x 1 y 2 time
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

			// ������ת��ΪС����
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index + 1]), sizeof(short));
			swapEndian(reinterpret_cast<uint8_t*>(&data_xy_press.reserv[now_start_index + 2]), sizeof(short));
		}

		int return_count = hid_write(fd_bcbox, reinterpret_cast<const unsigned char*>(&data_xy_press), sizeof(data_xy_press));

		// ���͹�����������
		memset(data_xy_press.reserv, 0, sizeof(data_xy_press.reserv));

		BC_PRINTF("return_count %d \t\n", return_count);
		if (return_count == sizeof(data_xy_press))
		{
			sleep_hi(1); // ��һ���˿ڷ������ӳ�1ms
		}
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 0;
}

static struct controller_t
{
	unsigned char reserved;//0X00
	unsigned char cmd;   //0x11 ���Ʒ���
	unsigned char status;   //0x11 ���Ʒ���
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
		sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

// �Ƿ�����������ƶ�
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
		sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


// ��ȡ�������
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
	unsigned char cmd;   //0x16 ����������̿���
	unsigned char select;   //ѡ��
	unsigned char status;   //״̬
	unsigned char send_data[30];
	unsigned char reserv[32];
}hex16_pack_data = { 0x00 };

// �����˵��ϴ� �����󷽿ɻ�ȡ�������
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
		sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


// ��ȡ�����˵������״̬
int BC_Get_EndP_Connection_Status(unsigned char* addr)
{
	unsigned char buff[65] = { 0 };
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	hex16_pack_data.reserved = 0x00;
	hex16_pack_data.cmd = 0x17; // ��ȡ���ݵ�cmd��ʶ��
	hex16_pack_data.select = 0x00;
	hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	hid_read_timeout(fd_bcbox, buff, 65, 1000);

	memcpy(addr, buff, sizeof(buff));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 1;
}


// �������ӳ��
int BC_Set_Mouse_Map(uint8_t enable_status, uint8_t mouse_status, uint8_t xamount, uint8_t xdirection, uint8_t yamount, uint8_t ydirection, uint8_t wheel_status)
{
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here

	hex16_pack_data.reserved = 0X00;
	hex16_pack_data.cmd = 0x16; // ͳһ��ʶ��
	hex16_pack_data.select = 0x01; // ��ʶ�������ӳ��
	hex16_pack_data.status = enable_status; // ����״̬
	hex16_pack_data.send_data[0] = mouse_status;
	hex16_pack_data.send_data[1] = xamount;
	hex16_pack_data.send_data[2] = xdirection;
	hex16_pack_data.send_data[3] = yamount;
	hex16_pack_data.send_data[4] = ydirection;
	hex16_pack_data.send_data[5] = wheel_status;
	i = hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	if (i == 65)
	{
		sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}

int BC_Get_Flash_data(unsigned char* addr)
{
	unsigned char buff[65] = { 0 };
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	hex16_pack_data.reserved = 0x00;
	hex16_pack_data.cmd = 0x17; // ��ȡ���ݵ�cmd��ʶ��
	hex16_pack_data.select = 0x01; // ��ȡflash data

	hid_write(fd_bcbox, (const unsigned char*)&hex16_pack_data, 65);
	hid_read_timeout(fd_bcbox, buff, 65, 1000);

	memcpy(addr, buff, sizeof(buff));
	ReleaseMutex(bc_m_hMutex_lock_write);
	return 1;
}


// �Ƿ��������Դ���ͣ
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
		sleep_hi(1);//��һ���˿ڷ������ӳ�1ms
	}
	ReleaseMutex(bc_m_hMutex_lock_write);
	return i == 65 ? 0 : -1;
}


//���̱���
static struct keyboard_t
{
	unsigned char reserved;
	unsigned char cmd;		//0x00
	unsigned char port;		//�˿�
	unsigned char ctrButton;//���ư���
	unsigned char data[61];	//0X00000
}data_keyboard = {0};





// ������װ  ֻ�Ƕ�Ӧwindows key map
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
	unsigned char enable = 0x00;   //�Ƿ�������
	unsigned char data[64];	//0X00000
}data_listen_t;

//�����������

DWORD WINAPI BC_ThreadListenProcess(LPVOID lpParameter)
{
	t_status_BC ret;
	int i=0;
	while (data_listen_t.enable)
	{	
		sleep_hi(1); //ÿ��1ms��ȡһ�� �߾�����ʱ
		WaitForSingleObject(bc_m_hMutex_lock_read, INFINITE); // Lock the mutex here
		i=hid_read_timeout(fd_bcbox, ret.buf, 64, 10); 

		if(i == 64)
		{
			memcpy(&bc_status,ret.buf,64);
		}
		
		

		ReleaseMutex(bc_m_hMutex_lock_read);
	}
	handle_listen = NULL;//�رռ���
	return 0;
}

/*

�򿪻��߹ر�������󰴼���������
enable 		:0 �رռ���  1���򿪼���
�򿪺���갴��״̬��mStatus��
���̰���״̬��kStatus�С�
*/
int BC_EnableMonitor(char enable){
	int i;
	WaitForSingleObject(bc_m_hMutex_lock_write, INFINITE); // Lock the mutex here
	data_listen_t.reserved = 0x00;
	data_listen_t.cmd = 0x07;
	data_listen_t.enable = enable; //�Ƿ�ʹ�ܱ�־
	i=hid_write(fd_bcbox, (const unsigned char*)&data_listen_t, 65);
	if (i == 65) Sleep(1);
	memset(bc_status.buf, 0, sizeof(bc_status.buf));

	if (enable)//�򿪼�������
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
��ѯ����������״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�������������ɿ�
			1����������������
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorLeft()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x01 ? 1 : 0;
}

/*
��ѯ��������Ҽ�״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0����������Ҽ��ɿ�
			1����������Ҽ�����
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorRight()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x02 ? 1 : 0;
}

/*
��ѯ��������м�״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0����������м��ɿ�
			1����������м�����
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorMiddle()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x04 ? 1 : 0;
}

/*
��ѯ���������1״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�����������1�ɿ�
			1�����������1����
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorSide1()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x08 ? 1 : 0;
}


/*
��ѯ���������2״̬   --ֻ�п�����BC_EnableMonitor��1�������ʹ��
����ֵ		0�����������2�ɿ�
			1�����������2����
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorSide2()
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & 0x10 ? 1 : 0;
}


// ��ע��vk��windows�����vkֵ������ͬ
int BC_Monitor_Mouse(int vk)
{
	if (data_listen_t.enable == 0) return -1;
	return bc_status.km.mstatus & vk ? 1 : 0;
}

/*
��ѯ�������ָ�������Ƿ��� --ֻ�п�����BC_EnableMonitor��1�������ʹ��
���룺		vk_key Ҫ��ѯ��HID��ֵ  ���HID��ֵ��

����ֵ		0��vk_key�ɿ�
			1��vk_key����
			-1����������δ����
�Ƿ���ԣ�20230220 ok
*/
int BC_MonitorKeyboard(int vk_key)
{
	if (data_listen_t.enable == 0) return -1;//
	if (vk_key >= KEY_LEFTCONTROL && vk_key <= KEY_RIGHT_GUI)//���Ƽ�
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
	else//�����
	{	for (int i = 1; i < 11; i++)
		{	if (bc_status.km.kstatus[i] == vk_key)
			{
				return 1;
			}
		}
	}
	return 0;
}

