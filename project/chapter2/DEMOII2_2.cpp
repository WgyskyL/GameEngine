#define WIN32_LEAN_AND_MEAN		// ��ʹ��MFC
	
#include <Windows.h>		// Windows ͷ�ļ�
#include <windowsx.h>		// ���������ĺ�

// ���� Windows ��������
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	// ������ʾ��Ϣ��� API�����������ھ������Ϊ NULL
	/*MessageBox(NULL, TEXT("T/*HERE CAN BE ONLY ONE!!!"),
		TEXT("MY FIRST WINDOWS PROGRAM"),
		MB_OK | MB_ICONEXCLAMATION);*/

	MessageBox(NULL, TEXT("THERE CAN BE ONLY ONE!!!"),
		TEXT("MY FIRST WINDOWS PROGRAM"),
		MB_YESNOCANCEL | MB_DEFBUTTON2);

	// �˳�����
	return (0);
}