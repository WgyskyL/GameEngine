// INCLUDES  ////////////////////////////////////////////////

#define INITGUID
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<windowsx.h>
#include<mmsystem.h>
#include<iostream>
#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#include<memory>
#include<string>
#include<stdarg.h>
#include<math.h>
#include<io.h>

#include<ddraw.h>
#include<dsound.h>
#include<dmksctrl.h>
#include<dmusicc.h>
#include<dmusics.h>
#include<dinput.h>
#include "t3dlib1.h"
#include "t3dlib2.h"
#include "t3dlib3.h"

// DEFINES  ////////////////////////////////////////////////

// ����Windows�ӿڳ���
#define WINDOW_CLASS_NAME	TEXT("WIN3DCLASS")	// ����
#define WINDOW_TITLE	TEXT("T3D graphics Console Ver 2.0")
#define WINDOW_WIDTH	640		// ���ڴ�С
#define WINDOW_HEIGHT	480

#define WINDOW_BPP	16	// ����λ�8��16��24�ȣ�
						// ע�⣺ ���ʹ�ô��ڡ���ȫ��ģʽ
						// λ�������ϵͳλ����ͬ
						// ���⣬��� 8 λ��ɫģʽ��������һ����ɫ��
						// ��������Ӧ�ó����������

#define WINDOWED_APP	1	// 0 ��ʾ�Ǵ���ģʽ��1 ��ʾ����ģʽ

///////////////////////////////////////////////////////////

#define NUM_STARS	512		// ������
#define NUM_TIES	32		// �ɴ���

// 3D engine constants
#define NEAR_Z	10		// ���ü���
#define FAR_Z	2000	// Զ�ü���
#define VIEW_DISTANCE	320	// �Ӿ�
							// ͶӰ����Ϊ 640 �Ĵ�����ʱ����Ұ��Ϊ 90 ��

// �������صĳ���
#define CROSS_VEL	8	// ʮ��׼�ǵ��ƶ��ٶ�
#define PLAYER_Z_VEL	8	// ����� z ��������ƶ��ٶ�

// ��ɴ�ģ����صĳ���
#define NUM_TIE_VERTS	10
#define NUM_TIE_EDGES	8

// �뱬ը�ṹ��صĳ���
#define NUM_EXPLOSIONS	(NUM_TIES)	// ��ը�ṹ����

// ��Ϸ״̬
#define GAME_RUNNING	1
#define GAME_OVER	0

// ���ݽṹ	////////////////////////////////////////////////////////////////

// 3D ��
typedef struct POIN3D_TYP
{
	USHORT color;	// 16 λ��ɫ
	float x, y, z;	// 3D ����
}POINT3D, *POINT3D_PTR;

// 3D �߶Σ�������ָ�򶨵��б�������������߶εĶ˵�
typedef struct LINE3D_TYP
{
	USHORT color;	// �߶ε� 16 λ��ɫ
	int v1, v2;		// �����߶ζ˵�Ķ����б�����
}LINE3D, *LINE3D_PTR;

// TIE �ɴ�
typedef struct TIE_TYP
{
	int state;	// �ɴ���״̬��0 ��ʾ������1 ��ʾ����
	float x, y, z;	// �ɴ���λ��
	float xv, yv, zv;	// �ɴ����ٶ�
}TIE, *TIE_PTR;

// ���ڱ�ʾ3D�ٶȵ�����
typedef struct VEC3D_TYP
{
	float x, y, z;
}VEC3D, *VEC3D_PTR;

// ����ģ�ⱬը�����ݽṹ
typedef struct EXPL_TYP
{
	int state;	// ��ը״̬
	USHORT color;	// ��ɫ
	int counter;	// ��ը������
		
	// ��ըʱͨ������ƶ����ɷɴ� 3D ģ�͵��߶�/����ʵ�ֵ�
	POINT3D p1[NUM_TIE_EDGES];	// �� n ���ߵ����
	POINT3D p2[NUM_TIE_EDGES];	// �� n ���ߵ��յ�

	VEC3D vel[NUM_TIE_EDGES];	// �ڵ���Ƭ���ٶ�
}EXPL, *EXPL_PTR;

// ����ԭ�� ///////////////////////////////////////////////////////////////////

// ��Ϸ����̨����
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);

// ��Ϸ����
void Init_Tie(int index);

// ȫ�ֱ���
HWND main_window_handle = NULL;		// ���ڴ洢���ھ��
HINSTANCE main_instance = NULL;		// ���ڴ洢ʾ��
char buffer[256];	// ���ڴ�ӡ�ļ�

// �ɴ��������Ӷ�����߶���ɵ�
// ֻ��һ���ɴ�ģ�ͣ����зɴ����Ǹ��ݸ�ģ�ͱ任�õ���

POINT3D tie_vlist[NUM_TIE_VERTS];	// �ɴ�ģ�͵Ķ����б�
LINE3D tie_shape[NUM_TIE_EDGES];	// �ɴ�ģ�͵ı��б�
TIE ties[NUM_TIES];		// �ɴ�����

POINT3D stars[NUM_STARS];	// �ǿ�

// һЩ��ɫ
USHORT rgb_green,
rgb_white,
rgb_red,
rgb_blue;

// �������صı���
float cross_x = 0,	// ׼�ǵ�λ��
cross_y = 0;

int cross_x_screen = WINDOW_WIDTH / 2,	// ׼�ǵ�λ��
cross_y_screen = WINDOW_HEIGHT / 2;
int target_x_screen = WINDOW_WIDTH / 2,		// ��׼λ��
target_y_screen = WINDOW_HEIGHT / 2;

//float window_client_x0 = 0;
//float window_client_y0 = 0;

int player_z_vel = 4;	// �ӵ�������ٶ�
int cannon_state = 0;	// �����ڵ�״̬
int cannon_count = 0;	// �����ڼ�����

EXPL explosions[NUM_EXPLOSIONS];	// ��ը�ṹ����

int misses = 0;		// ��¼���ѵķɴ���
int hits = 0;	// ��¼������ķɴ���
int score = 0;	// ����

// ���ֺ�����
int main_track_id = -1,		// ������ ID
laser_id = -1,		// �������������
explosion_id = -1,		// ��ը��
flyby_id = -1;		// �ɴ��ɹ�������

int game_state = GAME_RUNNING;	// ��Ϸ״̬

// ���� /////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam)
{
	// ����ϵͳ������Ϣ������
	PAINTSTRUCT ps;		// ���� WM_PAINT ��
	HDC		hdc;	// �豸�������

	// ��ʲô��Ϣ
	switch (msg) {
		case WM_CREATE:
		{
			// ִ�г�ʼ������
			return(0);
		}break;
		case WM_PAINT:
		{
			// ��ʼ����
			hdc = BeginPaint(hwnd, &ps);

			// ��������
			EndPaint(hwnd, &ps);
			return(0);
		}break;
		case WM_DESTROY:
		{
			// ɾ��Ӧ�ó���
			PostQuitMessage(0);
			return(0);
		}break;
		default:
			break;
	}

	// ����������Ϣ
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}	// end WindowPorc

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	// ����� winmain ����
	WNDCLASS winclass;	// �洢��������
	HWND	hwnd;	// ���ھ��
	MSG		msg;	// ��Ϣ
	HDC		hdc;	// �豸�������
	PAINTSTRUCT ps;		// paintstruct

	// ���ô�����
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

	// ע�ᴰ����
	if (!RegisterClass(&winclass))
		return(0);

	// �������ڣ� ���� WINDOWED_APP �Ƿ�Ϊ true ��ѡ����ʵĴ�������
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,		// ��
							  WINDOW_TITLE,		// ����
							  (WINDOWED_APP ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
							  0, 0,		// x��y ����
							  WINDOW_WIDTH,	// ���
							  WINDOW_HEIGHT,	// �߶�
							  NULL,		// �����ھ��
							  NULL,		// �˵����
							  hinstance,	// ʵ��
							  NULL)))		// ��������
							  return (0);

	// �����ھ����ʵ������ȫ�ֱ���
	main_window_handle = hwnd;
	main_instance = hinstance;

	// �������ڴ�С��ʹ�ͻ����Ĵ�СΪ width * height
	if (WINDOWED_APP) {
		// �������ڴ�С��ʹ�ͻ���Ϊʵ������Ĵ�С
		// ��Ϊ����Ӧ�ó����а��������Ŀؼ��ͱ߽�
		// ���Ӧ�ó����Ǵ���ģʽ�����޹ؽ�Ҫ
		RECT window_rect = { 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1 };

		// ���ú��������� window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));

		// ���ͻ���ƫ��������ȫ�ֱ������Ա��� DDraw_Flip() ��ʹ��
		window_client_x0 = -window_rect.left;
		window_client_y0 = -window_rect.top;

		// ���� MoveWindow() ���������ڴ�С
		MoveWindow(main_window_handle,
				   0,		// x ����
				   0,		// y ����
				   window_rect.right - window_rect.left,	// ���
				   window_rect.bottom - window_rect.top,	// �߶�
				   FALSE);

		// ��ʾ����
		ShowWindow(main_window_handle, SW_SHOW);
	}// end if windowed

	// ִ����Ϸ���е����г�ʼ��
	Game_Init();

	// ���� CTRL-ALT_DEL��ALT_TAB
	// ���������ϵͳ����������ע�͵�
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, NULL, 0);

	// �������¼�ѭ��
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			// ����Ƿ�Ϊ�˳���Ϣ
			if (msg.message == WM_QUIT)
				break;

			// ת�����ټ�
			TranslateMessage(&msg);

			// ����Ϣ���͸�window proc
			DispatchMessage(&msg);
		}// end if

		// ����Ϸ������
		Game_Main();

	}// end while

	// �ر���Ϸ���ͷ����е���Դ
	Game_Shutdown();
	
	// ����CTRL-ALT_DEL�� ALT_TAB
	// ������д��뵼��ϵͳ����������ע�͵�
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	// ���ز���ϵͳ
	return (msg.wParam);
}// end WinMain

// T3D II ��Ϸ��̿���̨���� //////////////////////////////////////////

int Game_Init(void *parms)
{
	// ������ִ��������Ϸ�ĳ�ʼ������

	int index;	// ѭ������
	Open_Error_File("error.txt");

	// ���� DirectDraw, �������Ҫ�滻���еĲ���
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// ��ʼ�� directinput
	DInput_Init();

	// ��ȡ���̿���Ȩ
	DInput_Init_Keyboard();

	// ��ʼ�� directsound
	DSound_Init();

	// ��������
	explosion_id = DSound_Load_WAV("exp1.wav");
	laser_id = DSound_Load_WAV("shocker.wav");

	// ��ʼ�� directmusic
	DMusic_Init();

	// ���ز�����������
	main_track_id = DMusic_Load_MIDI("midifile2.mid");
	DMusic_Play(main_track_id);

	// ��������ú�������ö����� directinput �豸�Ŀ���Ȩ

	// �������
	ShowCursor(FALSE);

	// �����������
	srand(Start_Clock());

	// ����Ϊ��ʼ������

	// ����ϵͳ��ɫ
	rgb_green = RGB16Bit(0, 255, 0);
	rgb_red = RGB16Bit(255, 0, 0);
	rgb_blue = RGB16Bit(0, 0, 255);
	rgb_white = RGB16Bit(255, 255, 255);

	// �����ǿ�
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// ����������ķ�����һ�����ӵ� (0, 0, -d) �������� (0, 0, far_z) ��Բ������
		stars[index].x = -WINDOW_WIDTH / 2 + rand() % WINDOW_WIDTH;
		stars[index].y = -WINDOW_HEIGHT / 2 + rand() % WINDOW_HEIGHT;
		stars[index].z = NEAR_Z + rand() % (FAR_Z - NEAR_Z);

		// �������ǵ���ɫ
		stars[index].color = rgb_white;
	}// end for index

	// �����ɴ�ģ��

	// �ɴ��Ķ����б�
	POINT3D temp_tie_vlist[NUM_TIE_VERTS] =
		// ��ɫ�� x��y��z ����
	{
		{ rgb_white, -40, 40, 0 },	// p0
		{ rgb_white, -40, 0, 0 },	// p1
		{ rgb_white, -40, -40, 0 },	// p2
		{ rgb_white, -10, 0, 0 },	// p3
		{ rgb_white, 0, 20, 0 },	// p4
		{ rgb_white, 10, 0, 0 },	// p5
		{ rgb_white, 0, -20, 0 },	// p6
		{ rgb_white, 40, 40, 0 },	// p7
		{ rgb_white, 40, 0, 0 },	// p8
		{rgb_white, 40, -40, 0},	// p9
	};

	for (int index = 0; index < NUM_TIE_VERTS; ++index)
	{
		tie_vlist[index] = temp_tie_vlist[index];
	}

	// �ɴ��ı��б�
	LINE3D temp_tie_shape[NUM_TIE_EDGES] =
		// ��ɫ������ 1������ 2
	{
		{ rgb_white, 0, 2 },	// 10
		{ rgb_white, 1, 3 },	// 11
		{ rgb_white, 3, 4 },	// 12
		{ rgb_white, 4, 5 },	// 13
		{ rgb_white, 5, 6 },	// 14
		{ rgb_white, 6, 3 },	// 15
		{ rgb_white, 5, 8 },	// 16
		{ rgb_white, 7, 9},		// 17
	};

	// ��ģ�͸��Ƶ�ȫ��������
	for (int index = 0; index < NUM_TIE_EDGES; ++index)
	{
		tie_shape[index] = temp_tie_shape[index];
	}

	for (int index = 0; index < NUM_TIES; ++index)
	{
		Init_Tie(index);
	}

	// �ɹ�����
	return (1);
}// end Game_Init

//////////////////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
	// �ú����ر���Ϸ���ͷŷ����������Դ

	// �ر�

	// �������ͷ�Ϊ��Ϸ�����������Դ

	// �ر� directsound
	DSound_Stop_All_Sounds();
	DSound_Shutdown();

	// �ر� directmusic
	DMusic_Delete_All_MIDI();
	DMusic_Shutdown();

	// �ر� directinput
	DInput_Shutdown();

	// ���ر� directdraw
	DDraw_Shutdown();

	// �ɹ�����
	return (1);

}// end Game_Shutdown

//////////////////////////////////////////////////////////////

void Start_Explosion(int tie)
{
	// ���ݴ���ķɴ�ģ�ⱬը
	// �ҵ���һ�����õı�ը�ṹ
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		if (explosions[index].state == 0) 
		{
			// ���ñ�ը״̬�������ݴ���ķɴ����øñ�ը�ṹ

			explosions[index].state = 1;	// ���ñ�ը״̬
			explosions[index].counter = 0;	// ���ñ�ը������

			// ���ñ�ը��ɫ
			explosions[index].color = rgb_green;

			// ���Ʊ��б� �Ա�ģ�ⱬը
			for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
			{
				// �ߵ����
				explosions[index].p1[edge].x = ties[index].x + tie_vlist[tie_shape[edge].v1].x;
				explosions[index].p1[edge].y = ties[index].y + tie_vlist[tie_shape[edge].v1].y;
				explosions[index].p1[edge].z = ties[index].z + tie_vlist[tie_shape[edge].v1].z;

				// �ߵ��յ�
				explosions[index].p2[edge].x = ties[index].x + tie_vlist[tie_shape[edge].v2].x;
				explosions[index].p2[edge].y = ties[index].y + tie_vlist[tie_shape[edge].v2].y;
				explosions[index].p2[edge].z = ties[index].z + tie_vlist[tie_shape[edge].v2].z;

				// ����ߵĵ�������
				explosions[index].vel[edge].x = ties[index].xv - 8 + rand() % 16;
				explosions[index].vel[edge].y = ties[index].yv - 8 + rand() % 16;
				explosions[index].vel[edge].z = -3 + rand() % 4;
			}// end for edge
			return;
		}// end if found
	}// end for index
}// end Start_Explosion

////////////////////////////////////////////////////////////////////

void Process_Explosions(void)
{
	// �������б�ը

	// ������ը�ṹ���飬��Ⱦ����
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		// ��鱬ը�ṹ�Ƿ��ڻ״̬
		if (explosions[index].state == 0)
			continue;

		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			// ������ը�����±ߵ����������յ�����
			explosions[index].p1[edge].x += explosions[index].vel[edge].x;
			explosions[index].p1[edge].y += explosions[index].vel[edge].y;
			explosions[index].p1[edge].z += explosions[index].vel[edge].z;

			explosions[index].p2[edge].x += explosions[index].vel[edge].x;
			explosions[index].p2[edge].y += explosions[index].vel[edge].y;
			explosions[index].p2[edge].z += explosions[index].vel[edge].z;
		}//end for edge

		// ��鱬ը�Ƿ����
		if (++explosions[index].counter > 100)
			explosions[index].state = explosions[index].counter = 0;
	}// end for index
}// end Process_Explosions

/////////////////////////////////////////////////////////

void Draw_Explosions(void)
{
	// �������еı�ը�ṹ

	// �������еı�ը�ṹ
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		// ��鱬ը�ṹ�Ƿ��ڻ״̬
		if (explosions[index].state == 0)
			continue;

		// ��Ⱦ��ը�ṹ
		// ÿ����ը�ṹ������һϵ�б���ɵ�
		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			POINT3D p1_per, p2_per;	// ���ڴ洢͸��ͶӰ��

			// �����Ƿ��ڽ��ü��������
			if (explosions[index].p1[edge].z < NEAR_Z && explosions[index].p2[edge].z < NEAR_Z)
				continue;

			// �� 1 ������ÿ���˵����͸�ӱ任
			p1_per.x = VIEW_DISTANCE * explosions[index].p1[edge].x / explosions[index].p1[edge].z;
			p1_per.y = VIEW_DISTANCE * explosions[index].p1[edge].y / explosions[index].p1[edge].z;
			p2_per.x = VIEW_DISTANCE * explosions[index].p2[edge].x / explosions[index].p2[edge].z;
			p2_per.y = VIEW_DISTANCE * explosions[index].p2[edge].y / explosions[index].p2[edge].z;

			// �� 2 ����������Ļ����
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// �� 3 �������Ʊ�
			Draw_Clip_Line16(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y, explosions[index].color, back_buffer, back_lpitch);
		}// end for edge
	}// end for index
}// end Draw_Explosions

/////////////////////////////////////////////

void Move_Starfield(void)
{
	// �ƶ��ǿ�

	// �Ӽ�����˵������Ӧ���Ǿ�ֹ��
	// �����ǽ�ͨ���ƶ�������ģ���ӵ���ƶ�
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// �ƶ���һ������
		stars[index].z -= player_z_vel;

		// ����Ƿ��ڽ��ü��������
		if (stars[index].z < NEAR_Z)
			stars[index].z = FAR_Z;
	}// end for index

}// end Move_Starfield

////////////////////////////////////////////////////

void Draw_Starfield(void)
{
	// ʹ��͸�ӱ任������ 3D ����
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// ������һ������
		// �� 1 ����͸�ӱ任
		float x_per = VIEW_DISTANCE * stars[index].x / stars[index].z;
		float y_per = VIEW_DISTANCE * stars[index].y / stars[index].z;

		// �� 2 ����������Ļ����
		int x_screen = WINDOW_WIDTH / 2 + x_per;
		int y_screen = WINDOW_HEIGHT / 2 - y_per;

		// �ж������Ƿ�����Ļ��
		if (x_screen > WINDOW_WIDTH || x_screen < 0 || y_screen > WINDOW_HEIGHT || y_screen < 0)
		{
			// ������һ������
			continue;
		}// end if
		else 
		{
			// ��Ⱦ��������
			((USHORT *)back_buffer)[x_screen + y_screen * (back_lpitch >> 1)]
				= stars[index].color;
		}// end else
	}// end for index
}// end Draw_Starfield

///////////////////////////////////////////

// �ú������ɴ����õ���Ϸ�������Զ��
void Init_Tie(int index)
{
	// ���ɴ������Ӿ�����
	ties[index].x = -WINDOW_WIDTH + rand() % (2 * WINDOW_WIDTH);
	ties[index].y = -WINDOW_HEIGHT + rand() % (2 * WINDOW_HEIGHT);
	ties[index].z = 4 * FAR_Z;

	// ��ʼ���ɴ����ٶ�
	ties[index].xv = -4 + rand() % 8;
	ties[index].yv = -4 + rand() % 8;
	ties[index].zv = -4 - rand() % 64;
	
	// ���ɴ�����Ϊ�״̬
	ties[index].state = 1;
}//end Init_Tie

//////////////////////////////////////////////

// ����ɴ���ִ��AI
void Process_Ties(void)
{
	// ��ÿ�ҷɴ����ӵ��ƶ�
	for (int index = 0; index < NUM_TIES; ++index)
	{
		// �ɴ��Ƿ񱻻���
		if (ties[index].state == 0)
			continue;

		// ������һ�ҷɴ�
		ties[index].x += ties[index].xv;
		ties[index].y += ties[index].yv;
		ties[index].z += ties[index].zv;

		// ���ɴ��Ƿ񴩹��˽�������
		if (ties[index].z < NEAR_Z) 
		{
			// �������ø÷ɴ�
			Init_Tie(index);

			// �����ѵķɴ����� 1
			misses++;
		}// reset tie
	}// end for index
}// end Process_Ties

////////////////////////////////////////////

// ����͸�ӱ任�� 3D �߿�ģʽ���Ʒɴ�
void Draw_Ties(void)
{
	// ���ڼ���ɴ��İ�Χ���Ա������ײ���
	int bmin_x, bmin_y, bmax_x, bmax_y;

	// ����ÿ�ҷɴ�
	for (int index = 0; index < NUM_TIES; ++index)
	{
		// ������һ�ҷɴ�

		// �Ƿ񱻻��䣿
		if (ties[index].state == 0)
			continue;

		// ����Χ������Ϊ�����ܵ�ֵ
		bmin_x = 100000;
		bmin_y = 100000;
		bmax_x = -100000;
		bmax_y = -100000;

		// ���ɴ��� z ����������Ŀ��� z ֵ�� �õ�һ��0 ~ 1 ��ֵ
		// Ȼ��ݴ˽� G ��������Ϊ0 ~ 31 ��ֵ��ʹ�÷ɴ�Խ������Խ��
		USHORT rgb_tie_color = RGB16Bit(0, (255 - 255 * (ties[index].z / (4 * FAR_Z))), 0);

		// ÿ�ҷɴ�����һϵ�б����
		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			POINT3D p1_per, p2_per;	// ���ڴ洢�˵�͸������

			// �� 1 ������ÿ���˵����͸�ӱ任
			// ��Ҫ���ݷɴ������ģ�͵�λ����Ӧ��ƽ��ÿ���˵㣬�����Ҫ
			p1_per.x = VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v1].x) /
				(ties[index].z + tie_vlist[tie_shape[edge].v1].z);

			p1_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v1].y) /
				(ties[index].z + tie_vlist[tie_shape[edge].v1].z);

			p2_per.x = VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v2].x) /
				(ties[index].z + tie_vlist[tie_shape[edge].v2].z);

			p2_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v2].y) /
				(ties[index].z + tie_vlist[tie_shape[edge].v2].z);

			// �� 2 ����������Ļ����
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// �� 3 �������Ʊ�
			Draw_Clip_Line16(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y, rgb_tie_color, back_buffer, back_lpitch);

			// ���������߸��°�Χ��
			int min_x = min(p1_screen_x, p2_screen_x);
			int min_y = min(p1_screen_y, p2_screen_y);

			int max_x = max(p1_screen_x, p2_screen_x);
			int max_y = max(p1_screen_y, p2_screen_y);

			bmin_x = min(bmin_x, min_x);
			bmin_y = min(bmin_y, min_y);

			bmax_x = max(bmax_x, max_x);
			bmax_y = max(bmax_y, max_y);

		}//end for edge

		// ���ɴ��Ƿ񱻻���
		if (cannon_state == 1) 
		{
			// ��鼤����׼λ���Ƿ��ڰ�Χ����
			if (target_x_screen > bmin_x && target_x_screen < bmax_x &&
				target_y_screen > bmin_y && target_y_screen < bmax_y) 
			{
				// �ɴ�������
				Start_Explosion(index);

				// ���ű�ը��
				DSound_Play(explosion_id);

				// ���ӵ÷�
				score += ties[index].z;

				// ��������ķɴ����� 1
				hits++;

				// ���³�ʼ���÷ɴ�
				Init_Tie(index);
			}// end if

		}// end if

	}// end for index

}// end Draw_Ties

///////////////////////////////////////////

// ������Ϸ�ĺ��ģ������ϵر�ʵʱ����
// ���� C �����е� main() ��������Ϸ�����еĺ������ö�����������е�
int Game_Main(void *parms)
{
	// ������ʱʱ��
	Start_Clock();
	
	// ��ջ��Ʊ���(drawing surface)
	DDraw_Fill_Surface(lpddsback, 0);

	// ��ȡ���̺������豸����
	DInput_Read_Keyboard();
	
	// ��Ϸ�߼�

	if (game_state == GAME_RUNNING)
	{
		// �ƶ�׼��
		if (keyboard_state[DIK_RIGHT])
		{
			// ��׼�������ƶ�
			cross_x += CROSS_VEL;

			// ����Ƿ񳬳��˴���
			if (cross_x > WINDOW_WIDTH / 2)
				cross_x = WINDOW_WIDTH / 2;

		}// end if

		if (keyboard_state[DIK_LEFT])
		{
			// ��׼�������ƶ�
			cross_x -= CROSS_VEL;
			
			// ����Ƿ񳬳��˴���
			if (cross_x < -WINDOW_WIDTH / 2)
				cross_x = -WINDOW_WIDTH;

		}// end if

		if (keyboard_state[DIK_DOWN])
		{
			// ��׼�������ƶ�
			cross_y -= CROSS_VEL;

			// ����Ƿ񳬳��˴���
			if (cross_y < -WINDOW_HEIGHT / 2)
				cross_y = -WINDOW_HEIGHT / 2;

		}// end if

		if (keyboard_state[DIK_UP])
		{
			// ��׼�������ƶ�
			cross_y += CROSS_VEL;

			// ����Ƿ񳬳��˴���
			if (cross_y > WINDOW_HEIGHT / 2)
				cross_y = WINDOW_HEIGHT / 2;

		}// end if

		// ���Ʒɴ����ٶ�
		if (keyboard_state[DIK_A])
			player_z_vel++;
		else if (keyboard_state[DIK_S])
			player_z_vel--;
		
		// �������Ƿ񿪻�
		if (keyboard_state[DIK_SPACE] && cannon_state == 0) 
		{
			// ����
			cannon_state = 1;
			cannon_count = 0;

			// �洢��һ�εı�׼λ��
			target_x_screen = cross_x_screen;
			target_y_screen = cross_y_screen;

			// ���ż����ڿ��������
			DSound_Play(laser_id);

		}// end if

	}// end if game running

	// �������ڣ�����һ����Ϸ״̬�����ھ�����������ȴ״̬֮����л�

	// ����׶�
	if (cannon_state == 1)
		if (++cannon_count > 15)
			cannon_state = 2;

	// ��ȴ�׶�
	if (cannon_state == 2)
		if (++cannon_count > 20)
			cannon_state = 0;

	// �ƶ�����
	Move_Starfield();

	// �ƶ��ɴ���ִ�зɴ�AI
	Process_Ties();

	// ����ը
	Process_Explosions();

	// �����󻺴棬�����ָ��Ϳ��
	DDraw_Lock_Back_Surface();

	//�����ǿ�
	Draw_Starfield();

	// ���Ʒɴ�
	Draw_Ties();

	// ģ�ⱬը
	Draw_Explosions();

	// ����׼��

	// ���ȼ���׼�ǵ���Ļ����
	// ע�� y �ᵹת������
	cross_x_screen = WINDOW_WIDTH / 2 + cross_x;
	cross_y_screen = WINDOW_HEIGHT / 2 - cross_y;

	// ʹ����Ļ�������׼��
	Draw_Clip_Line16(cross_x_screen - 16, cross_y_screen,
		cross_x_screen + 16, cross_y_screen,
		rgb_red, back_buffer, back_lpitch);

	Draw_Clip_Line16(cross_x_screen, cross_y_screen - 16,
		cross_x_screen, cross_y_screen + 16,
		rgb_red, back_buffer, back_lpitch);

	Draw_Clip_Line16(cross_x_screen - 16, cross_y_screen - 4,
		cross_x_screen - 16, cross_y_screen + 4,
		rgb_red, back_buffer, back_lpitch);

	Draw_Clip_Line16(cross_x_screen + 16, cross_y_screen - 4,
		cross_x_screen + 16, cross_y_screen + 4,
		rgb_red, back_buffer, back_lpitch);

	// ���Ƽ������
	if (cannon_state == 1)
	{
		if ((rand() % 2 == 1))
		{
			// �Ҽ�����
			Draw_Clip_Line16(WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB16Bit(0, 0, rand()), back_buffer, back_lpitch);

		}// end if
		else 
		{
			// �󼤹���
			Draw_Clip_Line16(0, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB16Bit(0, 0, rand()), back_buffer, back_lpitch);
		}// end else

	}// end if

	// ��Ⱦ�����󣬽���Ժ󻺴������
	DDraw_Unlock_Back_Surface();

	// ��ִ��Ϣ
	sprintf(buffer, "Score %d    kills %d    Escaped %d\0", score, hits, misses);
	Draw_Text_GDI(buffer, 0, 0, RGB(0, 255, 0), lpddsback);

	if (game_state == GAME_OVER)
		Draw_Text_GDI("G A M E O V E R\0", 320 - 8 * 10, 240, RGB(255, 255, 255), lpddsback);

	// ��������Ƿ񲥷���ϣ�����ǣ����²���
	if (DMusic_Status_MIDI(main_track_id) == MIDI_STOPPED)
	{
		DMusic_Play(main_track_id);
	}// end if

	// ����ǰ�󻺴�
	DDraw_Flip();

	// ͬ���� 30 ֡/��
	Wait_Clock(30);

	// �ж���Ϸ�Ƿ����
	if (misses > 100)
		game_state = GAME_OVER;

	// ����û��Ƿ�Ҫ�˳�
	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}// end if

	// �ɹ�����
	return (1);

}// end Game_Main

////////////////////////////////////////////