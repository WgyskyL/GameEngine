// DEMOII2_3.cpp

// INCLUDE
#define MIN32_LEAN_AND_MEAN		// ������MFC

#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>

// ����  ////////////////////////////////

// ���ڴ��ڵĳ���
#define WINDOW_CLASS_NAME TEXT("WINCLASS1")

// ȫ�ֱ���	/////////////////////////////

// ����	/////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;		// used in WM_PAINT
	HDC			hdc;	// �豸�������

	// ���ʲô��Ϣ
	switch (msg)
	{
	case WM_CREATE :
	{
		// ��ʼ������

		// �ɹ�����
		return (0);
	}break;

	case WM_PAINT :
	{
		// ��֤�����Ƿ���Ч
		hdc = BeginPaint(hwnd, &ps);

		// ���ƴ���
		EndPaint(hwnd, &ps);

		// return success
		return (0);
	}break;

	case WM_DESTROY :
	{
		// �ر�Ӧ�ó����⽫����һ��WM_QUIT
		PostQuitMessage(0);

		// �ɹ�����
		return (0);
	}break;
	}// end switch

	// ����������Ϣ
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}// end WindowProc

// WINMAIN //////////////////////////////////
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hpreinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX winclass;	// ���ڴ洢�����Ĵ�����
	HWND		hwnd;		// ���ھ��
	MSG			msg;		// ��Ϣ

	// ���ô�����
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

	// ע�ᴰ����
	if (!RegisterClassEx(&winclass))
		return (0);

	// ��������
	if (!(hwnd = CreateWindowEx(NULL,	// ��չ��ʽ
		WINDOW_CLASS_NAME,	// ������
		TEXT("You Basic Window"),		// ����
		WS_OVERLAPPED | WS_VISIBLE,
		0, 0,		// ���ڳ�ʼλ��
		400, 400,	// ���ڳ�ʼ��Ⱥ͸߶�
		NULL,		// �����ھ��
		NULL,		// �˵����
		hinstance,	// Ӧ�ó���ʵ��
		NULL)))		// �����Ķ������
		return (0);

	// ������ѭ��
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// ת��������
		TranslateMessage(&msg);

		// ����Ϣ���͸� Windowproc
		DispatchMessage(&msg);

		// ���ص� Windows ����ϵͳ
	}// end while

	return (msg.wParam);

}// end WinMain

//////////////////////////////////
