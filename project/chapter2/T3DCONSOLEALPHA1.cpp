// 包含头文件	////////////////////////////////
#include <Windows.h>
#include <windowsx.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <mmsystem.h>
#include <conio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<fcntl.h>

// 常量		/////////////////////////////////////

#define WINDOW_CLASS_NAME TEXT("WIN3DCLASS")
#define WINDOW_TITLE TEXT("T3D Graphics Console Alpha 1.0")
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

// 用于异步方式读取键盘输入
#define KEY_DOWN(vk_code) (GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0)
#define KEY_UP(vk_code) (GetAsyncKeyState(vk_code) & 0x8000 ? 0 : 1)

// 函数原型		////////////////////////////////////

int Game_Init(void *parms = NULL);
int Game_Main(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);

// 全局变量		/////////////////////////////////////

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;
char buffer[256];

// 函数		////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg) 
	{
	case WM_CREATE:
	{
		return (0);
	}break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
		return (0);
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return (0);
	}break;

	}// end switch
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}// end WindowProc

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASS	winclass;
	HWND		hwnd;
	MSG			msg;
	HDC			hsc;
	PAINTSTRUCT ps;

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&winclass))
		return (0);

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL, NULL,
		hinstance,
		NULL)))
		return (0);

	main_window_handle = hwnd;
	main_instance = hinstance;

	ShowWindow(main_window_handle, SW_SHOW);

	Game_Init();

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}// end if
		Game_Main();
	}// end while

	Game_Shutdown();

	return (msg.wParam);

}// end WinMain

// T3DII 游戏编程控制台函数

int Game_Init(void *parms)
{
	return (1);
}// end Game_Init

int Game_Main(void *parms)
{
	return (1);
}// end Game_Main

int Game_Shutdown(void *parms)
{
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}// end if
	return (1);
}// end Game_Shutdown



