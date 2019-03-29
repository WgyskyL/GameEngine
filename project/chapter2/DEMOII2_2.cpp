#define WIN32_LEAN_AND_MEAN		// 不使用MFC
	
#include <Windows.h>		// Windows 头文件
#include <windowsx.h>		// 包含大量的宏

// 所有 Windows 程序的入口
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	// 调用显示消息框的 API，并将父窗口句柄设置为 NULL
	/*MessageBox(NULL, TEXT("T/*HERE CAN BE ONLY ONE!!!"),
		TEXT("MY FIRST WINDOWS PROGRAM"),
		MB_OK | MB_ICONEXCLAMATION);*/

	MessageBox(NULL, TEXT("THERE CAN BE ONLY ONE!!!"),
		TEXT("MY FIRST WINDOWS PROGRAM"),
		MB_YESNOCANCEL | MB_DEFBUTTON2);

	// 退出程序
	return (0);
}