#include "Bcbox_Mouse_adaptation.h"

Bcbox_Mouse_adaptation::Bcbox_Mouse_adaptation()
{
	int status_init = BC_init(0x1a86, 0xFE00);
	if (status_init == 0)
	{
		BC_Enable_Endp_upload(0x01); // �����˵������ϴ�
	}


	
}



void OpenConsole()
{
	// �����µĿ���̨����
	AllocConsole();

	// ����ǰ���̸��ӵ�����̨����
	AttachConsole(GetCurrentProcessId());

	// ��ȡ��׼���롢����ʹ�������ľ��
	HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE consoleError = GetStdHandle(STD_ERROR_HANDLE);

	// ���ñ�׼���롢����ʹ���������ļ�ָ��
	FILE* inputFile = nullptr;
	FILE* outputFile = nullptr;
	FILE* errorFile = nullptr;
	freopen_s(&inputFile, "CONIN$", "r", stdin);
	freopen_s(&outputFile, "CONOUT$", "w", stdout);
	freopen_s(&errorFile, "CONOUT$", "w", stderr);

	// ���ÿ���̨�����롢�������ҳ
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

	// ����һ���߳� ����ʧȥ���Ӻ�����
	std::thread([=]() {
		unsigned char buff[65] = { 0 };
		while (this->Monitoring_status)
		{
			// ��������״̬
			this->connect_status = BC_is_device_valid();

			// ���û������ ��������
			if (!this->connect_status)
			{

				printf("�������� \n");
				int status_init = BC_init(0x1a86, 0xFE00);
				if (status_init == 0)
				{
					BC_Enable_Endp_upload(0x01); // �����˵������ϴ�
				}
			}

			// ���¶˿�����״̬
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
		

			// 2s ����һ��
			Sleep(2000);
		}

		}).detach();
}

// ��ȡ����������
uint8_t Bcbox_Mouse_adaptation::Get_Mouse_status_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // ��¼����
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("�밴������м� �ڼ䲻Ҫ�ƶ���� \t\n");

	// ��ȡ������index
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

		// �ж��Ƿ���һ��������10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 20) {
				printf("����м����� : mouse_status %d \n", (int)i);
				this->mouse_status = i; // �������״̬������
				return i;
			}
		}
		sleep_hi(20);
	}
}

// ��ȡ����������
uint8_t Bcbox_Mouse_adaptation::Get_Mouse_wheel_index_func()
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // ��¼����
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("������¹��� �ڼ䲻Ҫ�ƶ���� \t\n");

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

		// �ж��Ƿ���һ��������10
		for (size_t i = 0; i < 10; i++)
		{
			if (index_map[i] >= 10) {
				printf("���������� : wheel_status %d \n", (int)i);
				this->wheel_status = i; // ���������ֵ�����
				return i;
			}
		}
		sleep_hi(20);
	}
}

// ��ȡ����ƶ�������
void Bcbox_Mouse_adaptation::Get_Mouse_x_or_y_index_func(int type)
{
	unsigned char buff[65] = { 0 };
	std::map<int, int> index_map; // ��¼����
	// init
	for (size_t i = 0; i < 10; i++)
	{
		index_map[i] = 0;
	}

	printf("�뻺���� %s �ƶ� \t\n", type == 0 ? "�� x" : "�� y");

	// ��ȡ������index
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

		// �ж��Ƿ���һ��������10
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
	printf("д������� %02x %02x %02x %02x %02x %02x \n", mouse_status, xamount, xdirection, yamount, ydirection, wheel_status);
	BC_Set_Mouse_Map(0x01, mouse_status, xamount, xdirection, yamount, ydirection, wheel_status);
	printf("����д����� ���ڰ��¸�λ�� �鿴�Ƿ���������! \n");
}



short Bcbox_Mouse_adaptation::bytesToShort(const uint8_t value, const uint8_t direction)
{
	// ʹ��λ�Ʋ������ֽ�ת��Ϊ short
	short result = direction << 8 | value;
	return result;
}