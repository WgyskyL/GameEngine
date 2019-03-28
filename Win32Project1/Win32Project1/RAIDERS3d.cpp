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

// 用于Windows接口常量
#define WINDOW_CLASS_NAME	TEXT("WIN3DCLASS")	// 类名
#define WINDOW_TITLE	TEXT("T3D graphics Console Ver 2.0")
#define WINDOW_WIDTH	640		// 窗口大小
#define WINDOW_HEIGHT	480

#define WINDOW_BPP	16	// 窗口位深（8，16，24等）
						// 注意： 如果使用窗口、非全屏模式
						// 位深必需与系统位深相同
						// 另外，如果 8 位颜色模式，将创建一个调色板
						// 并将其与应用程序关联起来

#define WINDOWED_APP	1	// 0 表示非窗口模式；1 表示窗口模式

///////////////////////////////////////////////////////////

#define NUM_STARS	512		// 星星数
#define NUM_TIES	32		// 飞船数

// 3D engine constants
#define NEAR_Z	10		// 近裁剪面
#define FAR_Z	2000	// 远裁剪面
#define VIEW_DISTANCE	320	// 视距
							// 投影到宽为 640 的窗口上时，视野将为 90 度

// 与玩家相关的常量
#define CROSS_VEL	8	// 十字准星的移动速度
#define PLAYER_Z_VEL	8	// 玩家沿 z 轴的虚拟移动速度

// 与飞船模型相关的常量
#define NUM_TIE_VERTS	10
#define NUM_TIE_EDGES	8

// 与爆炸结构相关的常量
#define NUM_EXPLOSIONS	(NUM_TIES)	// 爆炸结构总数

// 游戏状态
#define GAME_RUNNING	1
#define GAME_OVER	0

// 数据结构	////////////////////////////////////////////////////////////////

// 3D 点
typedef struct POIN3D_TYP
{
	USHORT color;	// 16 位颜色
	float x, y, z;	// 3D 坐标
}POINT3D, *POINT3D_PTR;

// 3D 线段，由两个指向定点列表的索引来定义线段的端点
typedef struct LINE3D_TYP
{
	USHORT color;	// 线段的 16 位颜色
	int v1, v2;		// 定义线段端点的顶点列表索引
}LINE3D, *LINE3D_PTR;

// TIE 飞船
typedef struct TIE_TYP
{
	int state;	// 飞船的状态，0 表示死亡，1 表示活着
	float x, y, z;	// 飞船的位置
	float xv, yv, zv;	// 飞船的速度
}TIE, *TIE_PTR;

// 用于表示3D速度的向量
typedef struct VEC3D_TYP
{
	float x, y, z;
}VEC3D, *VEC3D_PTR;

// 用于模拟爆炸的数据结构
typedef struct EXPL_TYP
{
	int state;	// 爆炸状态
	USHORT color;	// 颜色
	int counter;	// 爆炸计数器
		
	// 爆炸时通过随机移动构成飞船 3D 模型的线段/边来实现的
	POINT3D p1[NUM_TIE_EDGES];	// 第 n 条边的起点
	POINT3D p2[NUM_TIE_EDGES];	// 第 n 条边的终点

	VEC3D vel[NUM_TIE_EDGES];	// 炮弹碎片的速度
}EXPL, *EXPL_PTR;

// 函数原型 ///////////////////////////////////////////////////////////////////

// 游戏控制台函数
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);

// 游戏函数
void Init_Tie(int index);

// 全局变量
HWND main_window_handle = NULL;		// 用于存储窗口句柄
HINSTANCE main_instance = NULL;		// 用于存储示例
char buffer[256];	// 用于打印文件

// 飞船是由连接顶点的线段组成的
// 只有一个飞船模型，所有飞船都是根据该模型变换得到的

POINT3D tie_vlist[NUM_TIE_VERTS];	// 飞船模型的顶点列表
LINE3D tie_shape[NUM_TIE_EDGES];	// 飞船模型的边列表
TIE ties[NUM_TIES];		// 飞船数组

POINT3D stars[NUM_STARS];	// 星空

// 一些颜色
USHORT rgb_green,
rgb_white,
rgb_red,
rgb_blue;

// 与玩家相关的变量
float cross_x = 0,	// 准星的位置
cross_y = 0;

int cross_x_screen = WINDOW_WIDTH / 2,	// 准星的位置
cross_y_screen = WINDOW_HEIGHT / 2;
int target_x_screen = WINDOW_WIDTH / 2,		// 瞄准位置
target_y_screen = WINDOW_HEIGHT / 2;

//float window_client_x0 = 0;
//float window_client_y0 = 0;

int player_z_vel = 4;	// 视点的虚拟速度
int cannon_state = 0;	// 激光炮的状态
int cannon_count = 0;	// 激光炮计数器

EXPL explosions[NUM_EXPLOSIONS];	// 爆炸结构数组

int misses = 0;		// 记录逃脱的飞船数
int hits = 0;	// 记录被击落的飞船数
int score = 0;	// 分数

// 音乐和声音
int main_track_id = -1,		// 主音轨 ID
laser_id = -1,		// 激光脉冲的声音
explosion_id = -1,		// 爆炸声
flyby_id = -1;		// 飞船飞过的声音

int game_state = GAME_RUNNING;	// 游戏状态

// 函数 /////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam)
{
	// 这是系统的主消息处理函数
	PAINTSTRUCT ps;		// 用于 WM_PAINT 中
	HDC		hdc;	// 设备场景句柄

	// 是什么消息
	switch (msg) {
		case WM_CREATE:
		{
			// 执行初始化工作
			return(0);
		}break;
		case WM_PAINT:
		{
			// 开始绘制
			hdc = BeginPaint(hwnd, &ps);

			// 结束绘制
			EndPaint(hwnd, &ps);
			return(0);
		}break;
		case WM_DESTROY:
		{
			// 删除应用程序
			PostQuitMessage(0);
			return(0);
		}break;
		default:
			break;
	}

	// 处理其他消息
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}	// end WindowPorc

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	// 这个是 winmain 函数
	WNDCLASS winclass;	// 存储创建的类
	HWND	hwnd;	// 窗口句柄
	MSG		msg;	// 消息
	HDC		hdc;	// 设备场景句柄
	PAINTSTRUCT ps;		// paintstruct

	// 设置窗口类
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

	// 注册窗口类
	if (!RegisterClass(&winclass))
		return(0);

	// 创建窗口， 根据 WINDOWED_APP 是否为 true 来选择合适的窗口属性
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,		// 类
							  WINDOW_TITLE,		// 标题
							  (WINDOWED_APP ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
							  0, 0,		// x、y 坐标
							  WINDOW_WIDTH,	// 宽度
							  WINDOW_HEIGHT,	// 高度
							  NULL,		// 父窗口句柄
							  NULL,		// 菜单句柄
							  hinstance,	// 实例
							  NULL)))		// 创建参数
							  return (0);

	// 将窗口句柄和实例赋给全局变量
	main_window_handle = hwnd;
	main_instance = hinstance;

	// 调整窗口大小，使客户区的大小为 width * height
	if (WINDOWED_APP) {
		// 调整窗口大小，使客户区为实际请求的大小
		// 因为窗口应用程序中包含大量的控件和边界
		// 如果应用程序不是窗口模式，则无关紧要
		RECT window_rect = { 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1 };

		// 调用函数来调整 window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));

		// 将客户区偏移量赋给全局变量，以便在 DDraw_Flip() 中使用
		window_client_x0 = -window_rect.left;
		window_client_y0 = -window_rect.top;

		// 调用 MoveWindow() 来调整窗口大小
		MoveWindow(main_window_handle,
				   0,		// x 坐标
				   0,		// y 坐标
				   window_rect.right - window_rect.left,	// 宽度
				   window_rect.bottom - window_rect.top,	// 高度
				   FALSE);

		// 显示窗口
		ShowWindow(main_window_handle, SW_SHOW);
	}// end if windowed

	// 执行游戏特有的所有初始化
	Game_Init();

	// 禁用 CTRL-ALT_DEL、ALT_TAB
	// 如果他导致系统崩溃，将其注释掉
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, NULL, 0);

	// 进入主事件循环
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			// 检查是否为退出信息
			if (msg.message == WM_QUIT)
				break;

			// 转换加速键
			TranslateMessage(&msg);

			// 将信息发送给window proc
			DispatchMessage(&msg);
		}// end if

		// 主游戏处理工作
		Game_Main();

	}// end while

	// 关闭游戏并释放所有的资源
	Game_Shutdown();
	
	// 启用CTRL-ALT_DEL、 ALT_TAB
	// 如果这行代码导致系统崩溃，将其注释掉
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	// 返回操作系统
	return (msg.wParam);
}// end WinMain

// T3D II 游戏编程控制台函数 //////////////////////////////////////////

int Game_Init(void *parms)
{
	// 在这里执行所有游戏的初始化工作

	int index;	// 循环变量
	Open_Error_File("error.txt");

	// 启动 DirectDraw, 请根据需要替换其中的参数
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// 初始化 directinput
	DInput_Init();

	// 获取键盘控制权
	DInput_Init_Keyboard();

	// 初始化 directsound
	DSound_Init();

	// 加载声音
	explosion_id = DSound_Load_WAV("exp1.wav");
	laser_id = DSound_Load_WAV("shocker.wav");

	// 初始化 directmusic
	DMusic_Init();

	// 加载并启动主音轨
	main_track_id = DMusic_Load_MIDI("midifile2.mid");
	DMusic_Play(main_track_id);

	// 在这里调用函数来获得对其他 directinput 设备的控制权

	// 隐藏鼠标
	ShowCursor(FALSE);

	// 随机数生成器
	srand(Start_Clock());

	// 这里为初始化代码

	// 创建系统颜色
	rgb_green = RGB16Bit(0, 255, 0);
	rgb_red = RGB16Bit(255, 0, 0);
	rgb_blue = RGB16Bit(0, 0, 255);
	rgb_white = RGB16Bit(255, 255, 255);

	// 创建星空
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// 蒋星星随机的放置在一个从视点 (0, 0, -d) 到剪裁面 (0, 0, far_z) 的圆柱体内
		stars[index].x = -WINDOW_WIDTH / 2 + rand() % WINDOW_WIDTH;
		stars[index].y = -WINDOW_HEIGHT / 2 + rand() % WINDOW_HEIGHT;
		stars[index].z = NEAR_Z + rand() % (FAR_Z - NEAR_Z);

		// 设置星星的颜色
		stars[index].color = rgb_white;
	}// end for index

	// 创建飞船模型

	// 飞船的顶点列表
	POINT3D temp_tie_vlist[NUM_TIE_VERTS] =
		// 颜色和 x、y、z 坐标
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

	// 飞船的边列表
	LINE3D temp_tie_shape[NUM_TIE_EDGES] =
		// 颜色、顶点 1、顶点 2
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

	// 将模型复制到全局数组中
	for (int index = 0; index < NUM_TIE_EDGES; ++index)
	{
		tie_shape[index] = temp_tie_shape[index];
	}

	for (int index = 0; index < NUM_TIES; ++index)
	{
		Init_Tie(index);
	}

	// 成功返回
	return (1);
}// end Game_Init

//////////////////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
	// 该函数关闭游戏，释放分配的所有资源

	// 关闭

	// 在这里释放为游戏分配的所有资源

	// 关闭 directsound
	DSound_Stop_All_Sounds();
	DSound_Shutdown();

	// 关闭 directmusic
	DMusic_Delete_All_MIDI();
	DMusic_Shutdown();

	// 关闭 directinput
	DInput_Shutdown();

	// 最后关闭 directdraw
	DDraw_Shutdown();

	// 成功返回
	return (1);

}// end Game_Shutdown

//////////////////////////////////////////////////////////////

void Start_Explosion(int tie)
{
	// 根据传入的飞船模拟爆炸
	// 找到第一个可用的爆炸结构
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		if (explosions[index].state == 0) 
		{
			// 启用爆炸状态，并根据传入的飞船设置该爆炸结构

			explosions[index].state = 1;	// 启用爆炸状态
			explosions[index].counter = 0;	// 重置爆炸计数器

			// 设置爆炸颜色
			explosions[index].color = rgb_green;

			// 复制边列表， 以便模拟爆炸
			for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
			{
				// 边的起点
				explosions[index].p1[edge].x = ties[index].x + tie_vlist[tie_shape[edge].v1].x;
				explosions[index].p1[edge].y = ties[index].y + tie_vlist[tie_shape[edge].v1].y;
				explosions[index].p1[edge].z = ties[index].z + tie_vlist[tie_shape[edge].v1].z;

				// 边的终点
				explosions[index].p2[edge].x = ties[index].x + tie_vlist[tie_shape[edge].v2].x;
				explosions[index].p2[edge].y = ties[index].y + tie_vlist[tie_shape[edge].v2].y;
				explosions[index].p2[edge].z = ties[index].z + tie_vlist[tie_shape[edge].v2].z;

				// 计算边的弹道向量
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
	// 处理所有爆炸

	// 遍历爆炸结构数组，渲染它们
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		// 检查爆炸结构是否处于活动状态
		if (explosions[index].state == 0)
			continue;

		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			// 发生爆炸，更新边的起点坐标和终点坐标
			explosions[index].p1[edge].x += explosions[index].vel[edge].x;
			explosions[index].p1[edge].y += explosions[index].vel[edge].y;
			explosions[index].p1[edge].z += explosions[index].vel[edge].z;

			explosions[index].p2[edge].x += explosions[index].vel[edge].x;
			explosions[index].p2[edge].y += explosions[index].vel[edge].y;
			explosions[index].p2[edge].z += explosions[index].vel[edge].z;
		}//end for edge

		// 检查爆炸是否结束
		if (++explosions[index].counter > 100)
			explosions[index].state = explosions[index].counter = 0;
	}// end for index
}// end Process_Explosions

/////////////////////////////////////////////////////////

void Draw_Explosions(void)
{
	// 绘制所有的爆炸结构

	// 遍历所有的爆炸结构
	for (int index = 0; index < NUM_EXPLOSIONS; ++index)
	{
		// 检查爆炸结构是否处于活动状态
		if (explosions[index].state == 0)
			continue;

		// 渲染爆炸结构
		// 每个爆炸结构都是由一系列边祖成的
		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			POINT3D p1_per, p2_per;	// 用于存储透视投影点

			// 检查边是否在近裁剪面的外面
			if (explosions[index].p1[edge].z < NEAR_Z && explosions[index].p2[edge].z < NEAR_Z)
				continue;

			// 第 1 步：对每个端点进行透视变换
			p1_per.x = VIEW_DISTANCE * explosions[index].p1[edge].x / explosions[index].p1[edge].z;
			p1_per.y = VIEW_DISTANCE * explosions[index].p1[edge].y / explosions[index].p1[edge].z;
			p2_per.x = VIEW_DISTANCE * explosions[index].p2[edge].x / explosions[index].p2[edge].z;
			p2_per.y = VIEW_DISTANCE * explosions[index].p2[edge].y / explosions[index].p2[edge].z;

			// 第 2 步：计算屏幕坐标
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// 第 3 步：绘制边
			Draw_Clip_Line16(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y, explosions[index].color, back_buffer, back_lpitch);
		}// end for edge
	}// end for index
}// end Draw_Explosions

/////////////////////////////////////////////

void Move_Starfield(void)
{
	// 移动星空

	// 从技术上说，星星应该是静止的
	// 但我们将通过移动他们来模拟视点的移动
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// 移动下一颗星星
		stars[index].z -= player_z_vel;

		// 检查是否在近裁剪面的外面
		if (stars[index].z < NEAR_Z)
			stars[index].z = FAR_Z;
	}// end for index

}// end Move_Starfield

////////////////////////////////////////////////////

void Draw_Starfield(void)
{
	// 使用透视变换来绘制 3D 星星
	for (int index = 0; index < NUM_STARS; ++index)
	{
		// 绘制下一颗星星
		// 第 1 步：透视变换
		float x_per = VIEW_DISTANCE * stars[index].x / stars[index].z;
		float y_per = VIEW_DISTANCE * stars[index].y / stars[index].z;

		// 第 2 步：计算屏幕坐标
		int x_screen = WINDOW_WIDTH / 2 + x_per;
		int y_screen = WINDOW_HEIGHT / 2 - y_per;

		// 判断星星是否在屏幕外
		if (x_screen > WINDOW_WIDTH || x_screen < 0 || y_screen > WINDOW_HEIGHT || y_screen < 0)
		{
			// 进入下一颗星星
			continue;
		}// end if
		else 
		{
			// 渲染到缓存中
			((USHORT *)back_buffer)[x_screen + y_screen * (back_lpitch >> 1)]
				= stars[index].color;
		}// end else
	}// end for index
}// end Draw_Starfield

///////////////////////////////////////////

// 该函数将飞船放置到游戏世界的最远处
void Init_Tie(int index)
{
	// 将飞船放在视景体内
	ties[index].x = -WINDOW_WIDTH + rand() % (2 * WINDOW_WIDTH);
	ties[index].y = -WINDOW_HEIGHT + rand() % (2 * WINDOW_HEIGHT);
	ties[index].z = 4 * FAR_Z;

	// 初始化飞船的速度
	ties[index].xv = -4 + rand() % 8;
	ties[index].yv = -4 + rand() % 8;
	ties[index].zv = -4 - rand() % 64;
	
	// 将飞船设置为活动状态
	ties[index].state = 1;
}//end Init_Tie

//////////////////////////////////////////////

// 处理飞船并执行AI
void Process_Ties(void)
{
	// 让每艘飞船向视点移动
	for (int index = 0; index < NUM_TIES; ++index)
	{
		// 飞船是否被击落
		if (ties[index].state == 0)
			continue;

		// 进入下一艘飞船
		ties[index].x += ties[index].xv;
		ties[index].y += ties[index].yv;
		ties[index].z += ties[index].zv;

		// 检查飞船是否穿过了近剪裁面
		if (ties[index].z < NEAR_Z) 
		{
			// 重新设置该飞船
			Init_Tie(index);

			// 将逃脱的飞船数加 1
			misses++;
		}// reset tie
	}// end for index
}// end Process_Ties

////////////////////////////////////////////

// 试用透视变换以 3D 线框模式绘制飞船
void Draw_Ties(void)
{
	// 用于计算飞船的包围框，以便进行碰撞检测
	int bmin_x, bmin_y, bmax_x, bmax_y;

	// 绘制每艘飞船
	for (int index = 0; index < NUM_TIES; ++index)
	{
		// 绘制下一艘飞船

		// 是否被击落？
		if (ties[index].state == 0)
			continue;

		// 将包围框设置为不可能的值
		bmin_x = 100000;
		bmin_y = 100000;
		bmax_x = -100000;
		bmax_y = -100000;

		// 将飞船的 z 坐标除以最大的可能 z 值， 得到一个0 ~ 1 的值
		// 然后据此将 G 分量设置为0 ~ 31 的值，使得飞船越近亮度越高
		USHORT rgb_tie_color = RGB16Bit(0, (255 - 255 * (ties[index].z / (4 * FAR_Z))), 0);

		// 每艘飞船都有一系列边祖成
		for (int edge = 0; edge < NUM_TIE_EDGES; ++edge)
		{
			POINT3D p1_per, p2_per;	// 用于存储端点透视坐标

			// 第 1 步：对每个端点进行透视变换
			// 需要根据飞船相对于模型的位置相应地平移每个端点，这很重要
			p1_per.x = VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v1].x) /
				(ties[index].z + tie_vlist[tie_shape[edge].v1].z);

			p1_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v1].y) /
				(ties[index].z + tie_vlist[tie_shape[edge].v1].z);

			p2_per.x = VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v2].x) /
				(ties[index].z + tie_vlist[tie_shape[edge].v2].z);

			p2_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v2].y) /
				(ties[index].z + tie_vlist[tie_shape[edge].v2].z);

			// 第 2 步：计算屏幕坐标
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// 第 3 步：绘制边
			Draw_Clip_Line16(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y, rgb_tie_color, back_buffer, back_lpitch);

			// 根据这条边更新包围框
			int min_x = min(p1_screen_x, p2_screen_x);
			int min_y = min(p1_screen_y, p2_screen_y);

			int max_x = max(p1_screen_x, p2_screen_x);
			int max_y = max(p1_screen_y, p2_screen_y);

			bmin_x = min(bmin_x, min_x);
			bmin_y = min(bmin_y, min_y);

			bmax_x = max(bmax_x, max_x);
			bmax_y = max(bmax_y, max_y);

		}//end for edge

		// 检查飞船是否被击中
		if (cannon_state == 1) 
		{
			// 检查激光瞄准位置是否在包围框内
			if (target_x_screen > bmin_x && target_x_screen < bmax_x &&
				target_y_screen > bmin_y && target_y_screen < bmax_y) 
			{
				// 飞船被击中
				Start_Explosion(index);

				// 播放爆炸声
				DSound_Play(explosion_id);

				// 增加得分
				score += ties[index].z;

				// 将被击落的飞船数加 1
				hits++;

				// 重新初始化该飞船
				Init_Tie(index);
			}// end if

		}// end if

	}// end for index

}// end Draw_Ties

///////////////////////////////////////////

// 这是游戏的核心，将不断地被实时调用
// 就像 C 语言中的 main() 函数，游戏中所有的函数调用都是在这里进行的
int Game_Main(void *parms)
{
	// 启动定时时钟
	Start_Clock();
	
	// 清空绘制表面(drawing surface)
	DDraw_Fill_Surface(lpddsback, 0);

	// 读取键盘和其他设备输入
	DInput_Read_Keyboard();
	
	// 游戏逻辑

	if (game_state == GAME_RUNNING)
	{
		// 移动准星
		if (keyboard_state[DIK_RIGHT])
		{
			// 将准星向右移动
			cross_x += CROSS_VEL;

			// 检查是否超出了窗口
			if (cross_x > WINDOW_WIDTH / 2)
				cross_x = WINDOW_WIDTH / 2;

		}// end if

		if (keyboard_state[DIK_LEFT])
		{
			// 将准星向左移动
			cross_x -= CROSS_VEL;
			
			// 检查是否超出了窗口
			if (cross_x < -WINDOW_WIDTH / 2)
				cross_x = -WINDOW_WIDTH;

		}// end if

		if (keyboard_state[DIK_DOWN])
		{
			// 将准星向下移动
			cross_y -= CROSS_VEL;

			// 检查是否超出了窗口
			if (cross_y < -WINDOW_HEIGHT / 2)
				cross_y = -WINDOW_HEIGHT / 2;

		}// end if

		if (keyboard_state[DIK_UP])
		{
			// 将准星向上移动
			cross_y += CROSS_VEL;

			// 检查是否超出了窗口
			if (cross_y > WINDOW_HEIGHT / 2)
				cross_y = WINDOW_HEIGHT / 2;

		}// end if

		// 控制飞船的速度
		if (keyboard_state[DIK_A])
			player_z_vel++;
		else if (keyboard_state[DIK_S])
			player_z_vel--;
		
		// 检测玩家是否开火
		if (keyboard_state[DIK_SPACE] && cannon_state == 0) 
		{
			// 开火
			cannon_state = 1;
			cannon_count = 0;

			// 存储上一次的标准位置
			target_x_screen = cross_x_screen;
			target_y_screen = cross_y_screen;

			// 播放激光炮开火的声音
			DSound_Play(laser_id);

		}// end if

	}// end if game running

	// 处理激光炮，这是一个游戏状态机，在就绪、开火、冷却状态之间的切换

	// 开火阶段
	if (cannon_state == 1)
		if (++cannon_count > 15)
			cannon_state = 2;

	// 冷却阶段
	if (cannon_state == 2)
		if (++cannon_count > 20)
			cannon_state = 0;

	// 移动星星
	Move_Starfield();

	// 移动飞船并执行飞船AI
	Process_Ties();

	// 处理爆炸
	Process_Explosions();

	// 锁定后缓存，并获得指针和宽度
	DDraw_Lock_Back_Surface();

	//绘制星空
	Draw_Starfield();

	// 绘制飞船
	Draw_Ties();

	// 模拟爆炸
	Draw_Explosions();

	// 绘制准星

	// 首先计算准星的屏幕坐标
	// 注意 y 轴倒转过来了
	cross_x_screen = WINDOW_WIDTH / 2 + cross_x;
	cross_y_screen = WINDOW_HEIGHT / 2 - cross_y;

	// 使用屏幕坐标绘制准星
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

	// 绘制激光光束
	if (cannon_state == 1)
	{
		if ((rand() % 2 == 1))
		{
			// 右激光束
			Draw_Clip_Line16(WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB16Bit(0, 0, rand()), back_buffer, back_lpitch);

		}// end if
		else 
		{
			// 左激光束
			Draw_Clip_Line16(0, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB16Bit(0, 0, rand()), back_buffer, back_lpitch);
		}// end else

	}// end if

	// 渲染结束后，解除对后缓存的锁定
	DDraw_Unlock_Back_Surface();

	// 回执信息
	sprintf(buffer, "Score %d    kills %d    Escaped %d\0", score, hits, misses);
	Draw_Text_GDI(buffer, 0, 0, RGB(0, 255, 0), lpddsback);

	if (game_state == GAME_OVER)
		Draw_Text_GDI("G A M E O V E R\0", 320 - 8 * 10, 240, RGB(255, 255, 255), lpddsback);

	// 检查音乐是否播放完毕，如果是，重新播放
	if (DMusic_Status_MIDI(main_track_id) == MIDI_STOPPED)
	{
		DMusic_Play(main_track_id);
	}// end if

	// 交换前后缓存
	DDraw_Flip();

	// 同步到 30 帧/秒
	Wait_Clock(30);

	// 判断游戏是否结束
	if (misses > 100)
		game_state = GAME_OVER;

	// 检查用户是否要退出
	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}// end if

	// 成功返回
	return (1);

}// end Game_Main

////////////////////////////////////////////