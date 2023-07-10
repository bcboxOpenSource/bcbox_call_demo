#include "../bcbox_4port/bcbox_4port.h"
#include <Windows.h>
#include <thread>
#include <map>

class Bcbox_Mouse_adaptation
{
public:
	bool Monitoring_status = false; // �Ƿ�������״̬����
	bool connect_status = false; // ����״̬
	bool endp1_status = false; // �˿�1����״̬
	bool endp2_status = false; // �˿�2����״̬
	uint8_t mouse_status = 0x00, xamount = 0x00, xdirection = 0x00, yamount = 0x00, ydirection = 0x00, wheel_status = 0x00;

	Bcbox_Mouse_adaptation(); // ��ʼ������

	void start_jianting();
	uint8_t Get_Mouse_status_index_func();
	uint8_t Get_Mouse_wheel_index_func();
	void Get_Mouse_x_or_y_index_func(int type);
	void Save_data();
	short bytesToShort(const uint8_t value, const uint8_t direction);
	
private:

};