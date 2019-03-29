// DEMOII2_3.cpp

// INCLUDE
#define MIN32_LEAN_AND_MEAN		// 不适用MFC

#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>

// 常量  ////////////////////////////////

// 用于窗口的常量
#define WINDOW_CLASS_NAME TEXT("WINCLASS1")

// 全局变量	/////////////////////////////

// 函数	/////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;		// used in WM_PAINT
	HDC			hdc;	// 设备场景句柄

	// 检测什么消息
	switch (msg)
	{
	case WM_CREATE :
	{
		// 初始化代码

		// 成功返回
		return (0);
	}break;

	case WM_PAINT :
	{
		// 验证窗口是否有效
		hdc = BeginPaint(hwnd, &ps);

		// 绘制代码
		EndPaint(hwnd, &ps);

		// return success
		return (0);
	}break;

	case WM_DESTROY :
	{
		// 关闭应用程序，这将发送一条WM_QUIT
		PostQuitMessage(0);

		// 成功返回
		return (0);
	}break;
	}// end switch

	// 处理其他消息
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}// end WindowProc

// WINMAIN //////////////////////////////////
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hpreinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX winclass;	// 用于存储创建的窗口类
	HWND		hwnd;		// 窗口句柄
	MSG			msg;		// 消息

	// 设置窗口类
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.lpszMenuName = NULL;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// 注册窗口类
	if (!RegisterClassEx(&winclass))
		return (0);

	// 创建窗口
	if (!(hwnd = CreateWindowEx(NULL,	// 扩展样式
		WINDOW_CLASS_NAME,	// 窗口类
		TEXT("You Basic Window"),		// 标题
		WS_OVERLAPPED | WS_VISIBLE,
		0, 0,		// 窗口初始位置
		400, 400,	// 窗口初始宽度和高度
		NULL,		// 父窗口句柄
		NULL,		// 菜单句柄
		hinstance,	// 应用程序实例
		NULL)))		// 创建的额外参数
		return (0);

	// 进入主循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// 转换加速器
		TranslateMessage(&msg);

		// 将消息发送给 Windowproc
		DispatchMessage(&msg);

		// 返回到 Windows 操作系统
	}// end while

	return (msg.wParam);

}// end WinMain

//////////////////////////////////
