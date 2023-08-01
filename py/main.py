import ctypes
import time
from ctypes import *
from hid_table import *

MOUSE_LEFT = 0x01
MOUSE_RIGHT = 0x02
MOUSE_MIDDLE = 0x04
MOUSE_Side1 = 0x08
MOUSE_Side2 = 0x10


# ctypes.c_short
# 加载DLL
BC_main = ctypes.CDLL('.\\Bcbox_4Port_Call_demo.dll')

def BC_MonitorKeyboard(vk_key, data):

    if vk_key >= KEY_LEFT_CONTROL and vk_key <= KEY_RIGHT_GUI: #控制键
        if vk_key == KEY_LEFT_CONTROL:
            return True if data[1] & BIT0 else False
        elif vk_key == KEY_LEFT_SHIFT:
            return True if data[1] & BIT1 else False
        elif vk_key == KEY_LEFT_ALT:
            return True if data[1] & BIT2 else False
        elif vk_key == KEY_LEFT_GUI:
            return True if data[1] & BIT3 else False
        elif vk_key == KEY_RIGHT_CONTROL:
            return True if data[1] & BIT4 else False
        elif vk_key == KEY_RIGHT_SHIFT:
            return True if data[1] & BIT5 else False
        elif vk_key == KEY_RIGHT_ALT:
            return True if data[1] & BIT6 else False
        elif vk_key == KEY_RIGHT_GUI:
            return True if data[1] & BIT7 else False

    else: #常规键
        for i in range(2, 12):
            if data[i] == vk_key:
                return True
    return False



def Dll_init():
    # 定义BC_init函数的参数类型和返回类型
    BC_main.BC_init.argtypes = [ctypes.c_short, ctypes.c_short]
    BC_main.BC_init.restype = ctypes.c_int

    # 定义BC_move函数的参数类型和返回类型
    BC_main.BC_move.argtypes = [ctypes.c_short, ctypes.c_short]
    BC_main.BC_move.restype = ctypes.c_int

    # 定义 BC_left 函数的参数类型和返回类型
    BC_main.BC_left.argtypes = [ctypes.c_uint8]
    BC_main.BC_left.restype = ctypes.c_int

    BC_main.BC_right.argtypes = [ctypes.c_uint8]
    BC_main.BC_right.restype = ctypes.c_int

    BC_main.BC_middle.argtypes = [ctypes.c_uint8]
    BC_main.BC_middle.restype = ctypes.c_int

    BC_main.BC_side1.argtypes = [ctypes.c_uint8]
    BC_main.BC_side1.restype = ctypes.c_int

    BC_main.BC_side2.argtypes = [ctypes.c_uint8]
    BC_main.BC_side2.restype = ctypes.c_int

    # 开启监听
    BC_main.BC_EnableMonitor_Dll.argtypes = [ctypes.c_char]
    BC_main.BC_EnableMonitor_Dll.restype = ctypes.c_int

    # 获取上传数据
    BC_main.BC_Monitor_Data.argtypes = [POINTER(c_uint8)]
    BC_main.BC_Monitor_Data.restype = ctypes.c_int


def bc_left_down():
    print("调用左键点击代码")
    status = BC_main.BC_left(1)  # 左键按下
    print("调用代码BC_left(1) 状态 ", "成功" if status == 0 else "失败")
    time.sleep(0.4)
    status = BC_main.BC_left(0)  # 左键抬起
    print("调用代码BC_left(0) 状态 ", "成功" if status == 0 else "失败")


def bc_right_down():
    print("调用右键点击代码")
    status = BC_main.BC_right(1)  # 左键按下
    print("调用代码BC_right(1) 状态 ", "成功" if status == 0 else "失败")
    time.sleep(0.4)
    status = BC_main.BC_right(0)  # 左键抬起
    print("调用代码BC_right(0) 状态 ", "成功" if status == 0 else "失败")


def bc_move():
    print("调用移动代码")
    status = BC_main.BC_move(2, 4)
    print("调用代码BC_move(2, 4) 状态 ", "成功" if status == 0 else "失败")
    time.sleep(0.4)


def bc_monitor_mouse(data, key):
    if data[0] & key:
        return True
    else:
        return False


def bc_enable_monitor_dll_main():
    BC_main.BC_EnableMonitor_Dll(1)  # 开启监听

    # 创建一个uint8_t数组
    data = (c_uint8 * 64)()  # 创建一个长度为64的uint8_t数组

    while True:
        # 调用函数
        result = BC_main.BC_Monitor_Data(data)

        # 检查函数返回值
        if result == 1:
            # 如果函数返回1，表示成功读取数据，打印数据
            # print(list(data))
            if bc_monitor_mouse(data, MOUSE_LEFT):
                print("左键按下")
            if bc_monitor_mouse(data, MOUSE_RIGHT):
                print("右键按下")
            if bc_monitor_mouse(data, MOUSE_MIDDLE):
                print("中键按下")
            if bc_monitor_mouse(data, MOUSE_Side1):
                print("侧键1按下")
            if bc_monitor_mouse(data, MOUSE_Side2):
                print("侧键2按下")

            if BC_MonitorKeyboard(KEY_LEFT_ALT, data):
                print("KEY_LEFT_ALT 按下")
        else:
            print("Failed to read data")


# 请自行单步调试
Dll_init()
result = BC_main.BC_init(0x1a86, 0xfe00)
if result == 0:
    print("bcbox_初始化成功")  # 输出初始化状态

    bc_move()

    # 左键键按下示例
    bc_left_down()

    # 右键按下示例
    bc_right_down()

    # 开启监听键鼠状态
    bc_enable_monitor_dll_main()




else:
    print("bcbox_初始化失败")  # 输出初始化状态
