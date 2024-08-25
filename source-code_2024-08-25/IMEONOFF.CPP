#include <windows.h>
#include "winnls.h"
#include "ime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// 定数
#define WINDOW_WIDTH  (400)        // ウィンドウの幅
#define WINDOW_HEIGHT (180)        // ウィンドウの高さ
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)
#define START_BUTTON_ID (101)    // 「GetHWND」ボタンのID
#define BUTTON_ID_01    (102)    // 「IME OFF」ボタンのID
#define BUTTON_ID_02    (103)    // 「IME ON」ボタンのID
#define BUTTON_ID_03    (104)    // 「BUTTON3」ボタンのID
#define BUTTON_ID_04    (105)    // 「BUTTON4」ボタンのID
#define BUTTON_WIDTH    (80)     // ボタンの幅
#define BUTTON_HEIGHT   (30)     // ボタンの高さ
#define MARGIN           (1)     // 上下左右の余白

HWND g_hwndEdit;
HWND hwndEdit02;
HWND hwndEdit03;
HWND hwndSTARTBUTTON;
HWND hwndBUTTON01;
HWND hwndBUTTON02;
HWND hwndBUTTON03;
HWND hwndBUTTON04;

HWND aWnd;
HWND aParentWnd;
int xpos01, ypos01;

LPIMESTRUCT imePointer01;
HGLOBAL imeHandle01;

// プロトタイプ宣言
int isImmOpen(HWND imeWnd01);
void setImmOnOff(HWND imeWnd01, int AOnOff);

int editboxTextLength(HWND hWnd);
void editboxClear(HWND hWnd);
char strbuffer01[1024];
void AppendText(LPSTR str, HWND g_hwndEdit);

long  far PASCAL MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
HINSTANCE hInst01;

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
{
	MSG msg;

	if (!hPrevInst)
		if (!InitApplication(hInst))
			return FALSE;
	if (!InitInstance(hInst, showCmd))
		return FALSE;

	// メッセージループ
	while( 1 )
	{
		BOOL ret = GetMessage( &msg, NULL, 0, 0 );  // メッセージを取得する
		if( ret == 0 || ret == -1 )
		{
			// アプリケーションを終了させるメッセージが来ていたら、
			// あるいは GetMessage() が失敗したら( -1 が返されたら ）、ループを抜ける
			break;
		}
		else
		{
			// メッセージを処理する
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return (msg.wParam);
}

BOOL InitApplication(HINSTANCE hInstance){
	WNDCLASS  wc;
	wc.style = 0;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//wc.hIcon = LoadIcon(hInstance, "GenericIcon");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = "WIMEOnOffWClass";
	return RegisterClass(&wc);
}

void GetWindowsVersion(void){
	//    0x300: Windows 3.00
	//    0x30a: Windows 3.10
	//    0x30b: Windows 3.11
	//    0x35f: Windows 95 (16bit) (3.95)
	//    0x400: Windows 95 (32bit) (4.00)
	//    0x362: Windows 98 (16bit) (3.98)
	//    0x40a: Windows 98 (32bit) (4.10)
	WORD Version;

	WORD DosVersion = HIWORD(GetVersion());
	Version = LOWORD(GetVersion());
	Version = (WORD)((LOBYTE(Version)<<8) + HIBYTE(Version));
#ifdef WIN32
	if (DosVersion & 0x8000) NT=0; else NT=1;
#else
	if (Version==0x35f && DosVersion >=0x70a) Version+=3;
#endif
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
	HWND hWnd;
	GetWindowsVersion();
	hWnd = CreateWindow(
		"WIMEOnOffWClass",
		"IMEOnOff",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if(!hWnd)
	{
		MessageBox( NULL, "ウィンドウの作成に失敗しました", "エラー", MB_OK );
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

// ウィンドウプロシージャ
long  far PASCAL MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HDC hDC;
	PAINTSTRUCT ps;
	int wmId, wmEvent;


	switch( msg )
	{
		case WM_DESTROY:          // ウィンドウが破棄されるとき

                                // 終了時処理
				GlobalUnlock(imeHandle01);
				GlobalFree(imeHandle01);
				imePointer01 = NULL;

				PostQuitMessage(0);
				return 0;

		case WM_CREATE:
				//ボタンを配置
				{
				RECT rcClt;
				GetClientRect( hWnd, &rcClt);
				int xPos = ( rcClt.right - rcClt.left ) - (BUTTON_WIDTH + 10) * 1;
				int yPos = 20;
				hwndBUTTON01 = CreateWindow( "BUTTON", "IME OFF",
						WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
						hWnd, (HMENU)BUTTON_ID_01,
						hInst01, NULL);
				}

				{
				RECT rcClt;
				GetClientRect( hWnd, &rcClt);
				int xPos = ( rcClt.right - rcClt.left ) - (BUTTON_WIDTH + 10) * 2;
				int yPos = 100;
				hwndBUTTON02 = CreateWindow( "BUTTON", "IME ON",
						WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
						hWnd, (HMENU)BUTTON_ID_02,
						hInst01, NULL);
				}

				{
				RECT rcClt;
				GetClientRect( hWnd, &rcClt);
				int xPos = ( rcClt.right - rcClt.left ) - (BUTTON_WIDTH + 10) * 3;
				int yPos = 100;
				hwndBUTTON03 = CreateWindow("BUTTON", "BUTTON03",
						WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
						hWnd, (HMENU)BUTTON_ID_03,
						hInst01, NULL);
				}

				{
				RECT rcClt;
				GetClientRect( hWnd, &rcClt);
				int xPos = ( rcClt.right - rcClt.left ) - (BUTTON_WIDTH + 10) * 4;
				int yPos = 60;
				hwndBUTTON04 = CreateWindow("BUTTON", "BUTTON04",
						WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
						hWnd, (HMENU)BUTTON_ID_04,
						hInst01, NULL);
				}

				{
				RECT rcClt;
				GetClientRect( hWnd, &rcClt);
				int xPos = ( rcClt.right - rcClt.left ) - (BUTTON_WIDTH + 10) * 3;
				int yPos = 100;
				hwndSTARTBUTTON = CreateWindow("BUTTON", "GetHWND",
						WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
						hWnd, (HMENU)START_BUTTON_ID,
						hInst01, NULL);
				}

				// EditBox表示 (line)
				{
						RECT rect;
						GetClientRect(hWnd, &rect);
						hwndEdit02 = CreateWindow("EDIT", "",
								WS_CHILDWINDOW | WS_VISIBLE  | ES_AUTOVSCROLL | WS_BORDER,
								0, 20, rect.right - BUTTON_WIDTH - 20 - 10, 10 + 20, hWnd,
								NULL, hInst01, NULL);
						if(!hwndEdit02)   return -1;
				}

				// EditBox表示 (line)
				{
						RECT rect;
						GetClientRect(hWnd, &rect);
						hwndEdit03 = CreateWindow("EDIT", "",
								WS_CHILDWINDOW | WS_VISIBLE  | ES_AUTOVSCROLL | WS_BORDER,
								0, 20, rect.right - BUTTON_WIDTH - 20 - 10, 10 + 20, hWnd,
								NULL, hInst01, NULL);
						if(!hwndEdit02)   return -1;
				}
/*
				// EditBox表示 (memo)
				{
						RECT rect;
						GetClientRect(hWnd, &rect);
						g_hwndEdit = CreateWindow("EDIT", "",
								WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
								0, 100 + 20 + 20, rect.right, rect.bottom - 50 - 60 - 20, hWnd,
								NULL, hInst01, NULL);
						if(!g_hwndEdit)  return -1;
				}
*/



                                // 初期化処理
				imeHandle01 = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(IMESTRUCT));
				imePointer01 = (LPIMESTRUCT)GlobalLock(imeHandle01);



				return 0;

		case WM_KEYDOWN:
				{
					wmId = START_BUTTON_ID;
					wmEvent = 0;
					if (wp == VK_RETURN) {
						SendMessage(hWnd, WM_COMMAND, (wmEvent << 16) | (wmId & 0xffff), 0);
					}
				}
				break;
		case WM_COMMAND:
				wmId = LOWORD(wp);
				wmEvent = HIWORD(wp);
				switch( wmId )
				{
				case BUTTON_ID_01:
						{
							if (aWnd != NULL) {
								setImmOnOff(aWnd, 0);
							}
							SetFocus(hwndBUTTON02);
						}
						break;
				case BUTTON_ID_02:
						{
							if (aWnd != NULL) {
								setImmOnOff(aWnd, 1);
							}
							SetFocus(hwndBUTTON01);
						}
						break;
				case BUTTON_ID_03:
						{
							SetFocus(hWnd);
						}
						break;

				case BUTTON_ID_04:
						{
							SetFocus(hWnd);
						}
						break;

				case START_BUTTON_ID:

						{
							POINT point01;
							HWND  tempWnd;
							char  str1[256];
							GetCursorPos(&point01);
							aWnd = WindowFromPoint(point01);
							tempWnd = aWnd;
							while (tempWnd != 0) {
								aParentWnd = tempWnd;
								tempWnd = GetParent(aParentWnd);
							}
							GetWindowText(aParentWnd, str1, 255);

							editboxClear(hwndEdit02);
							AppendText(str1, hwndEdit02);
							editboxClear(hwndEdit03);
							sprintf(str1, "0x%08lx", (unsigned long)aWnd);
							AppendText(str1, hwndEdit03);
							SetFocus(hWnd);
						}
						break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);

		}
				break;


		case WM_LBUTTONUP:
				xpos01 = LOWORD(lp);
				ypos01 = HIWORD(lp);
				SetFocus(hWnd);
				return 0;

		case WM_SIZE:
			{
				int clientx, clienty;
				clientx = LOWORD(lp);
				clienty = HIWORD(lp);

				MoveWindow(g_hwndEdit, 0, 100 + 20 + 20, clientx, clienty - 50 - 60 - 20, TRUE);
				MoveWindow(hwndEdit02, 0, 20, clientx - BUTTON_WIDTH - 20 - -10 - 10, 10 + 20, TRUE);
				MoveWindow(hwndEdit03, 0, 60, clientx - BUTTON_WIDTH - 20 - -10 - 10, 10 + 20, TRUE);
				MoveWindow(hwndSTARTBUTTON, clientx - BUTTON_WIDTH - 10, 20, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
				MoveWindow(hwndBUTTON01, clientx - (BUTTON_WIDTH + 10) * 1, 100, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
				MoveWindow(hwndBUTTON02, clientx - (BUTTON_WIDTH + 10) * 2, 100, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
				MoveWindow(hwndBUTTON03, clientx - (BUTTON_WIDTH + 10) * 3, 100, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
				MoveWindow(hwndBUTTON04, clientx - (BUTTON_WIDTH + 10) * 4, 100, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
			}
			break;

		case WM_PAINT:          // 無効領域があるとき
				hDC = BeginPaint( hWnd, &ps );

				EndPaint( hWnd, &ps );
				return 0;
		}

		return DefWindowProc( hWnd, msg, wp, lp );
}

int isImmOpen(HWND imeWnd01) {
	long  a;
	int   unlockret01;
	int   ret01;

	imePointer01 = (LPIMESTRUCT)GlobalLock(imeHandle01);
	imePointer01->fnc = IME_GETOPEN;

	a = SendIMEMessageEx((HWND)imeWnd01, (LPARAM)imeHandle01);
	unlockret01 = GlobalUnlock(imeHandle01);
	imePointer01 = NULL;
	if (a == 0) ret01 = 0;
	else        ret01 = 1;

	return ret01;
}

void setImmOnOff(HWND imeWnd01, int AOnOff) {
	long a;
	int unlockret01;

	imePointer01 = (LPIMESTRUCT)GlobalLock(imeHandle01);
	if (AOnOff == 0) {
		imePointer01->wParam = 0;
	} else {
		imePointer01->wParam = 1;
	}
	imePointer01->fnc = IME_SETOPEN;
	a = SendIMEMessageEx((HWND)imeWnd01, (LPARAM)imeHandle01);
	unlockret01 = GlobalUnlock(imeHandle01);
	imePointer01 = NULL;
}

int editboxTextLength(HWND hWnd) {
	int   length01;
	length01 = (int) SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
	return length01;
}

void editboxClear(HWND hWnd) {
	SetWindowText( hWnd, "");
	SendMessage( hWnd, EM_REPLACESEL, FALSE, (LPARAM)(LPSTR)"");
}

void AppendText(LPSTR str, HWND g_hwndEdit)
{
	SendMessage(g_hwndEdit, EM_SETSEL, -1, -1);
	SendMessage(g_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)str);
	SetFocus(g_hwndEdit);
}
