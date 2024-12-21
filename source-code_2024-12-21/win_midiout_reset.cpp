#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib, "winmm.lib")

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>
#include <process.h>

#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)
#define WINDOW_WIDTH   (600)
#define WINDOW_HEIGHT  (600)
#define BUTTON_WIDTH   (80)
#define BUTTON_HEIGHT  (30)
#define GS_RESET_BUTTON_ID (100)
#define TEST01_BUTTON_ID   (101)
#define TEST02_BUTTON_ID   (102)
#define IDC_RICHEDIT       (103)
#define COMBBOX_ID         (104)
#define XG_RESET_BUTTON_ID (105)

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;

TCHAR *szTitle;
TCHAR *szWindowClass;
TCHAR Buff01_szTitle[MAX_LOADSTRING];
TCHAR Buff01_szWindowClass[MAX_LOADSTRING];

HINSTANCE   hRichLib; 
HWND    g_hwndRichEdit;
HANDLE    g_handleThread1;
HANDLE    g_handleThread2;
HANDLE    g_handleThread3;
HANDLE    g_handleThread4;
volatile int threadExecFlag1 = 0;
volatile int threadExecFlag2 = 0;
volatile int threadExecFlag3 = 0;
volatile int threadExecFlag4 = 0;
HWND    hwndCombo01;
HWND    hwndButton01;
HWND    hwndButton02;
HWND    hwndButton03;
HWND    hwndButton04;
void    *threadParam01;
int     threadEndFlag01;
HFONT   hEdit01Font01;
HFONT   hRichEdit01Font01;

int editboxTextLength(HWND hWnd);
void editboxClear(HWND hWnd);
void AppendText(LPCTSTR str, HWND hWnd);
unsigned _stdcall threadFunc01(void *threadParam01);   // GS Reset
unsigned _stdcall threadFunc02(void *threadParam01);   // TEST01
unsigned _stdcall threadFunc03(void *threadParam01);   // TEST02
unsigned _stdcall threadFunc04(void *threadParam01);   // XG Reset


HMIDIOUT   hMidiOut;
BYTE       note = 0x3C;
BYTE       velocity = 0x7f;
BYTE       program=0;
BYTE       mainVolume = 127;
uint32_t   uMidiDeviceID = 0;   // midi deviceID = 0; 一番先頭のmidiデバイス


// midiOutLongMsgを使うのに必要
MIDIHDR mhMidi;
BYTE gmReset[] = {0xf0, 0x7e, 0x7f, 0x09, 0x01, 0xf7};
BYTE gsReset[] = {0xf0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7f, 0x00, 0x41, 0xf7};
BYTE xgReset[] = {0xf0, 0x43, 0x10, 0x4c, 0x00, 0x00, 0x7e, 0x00, 0xf7};


ATOM			MyRegisterClass(HINSTANCE hInstance);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	// グローバル文字列を初期化しています。
        szTitle = Buff01_szTitle;
        szWindowClass = Buff01_szWindowClass;
        _tcscpy( szTitle,  _T("WinMidiReset"));
        _tcscpy( szWindowClass, _T("WinMidiReset"));

        MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// メイン メッセージ ループ:
	while ( 1 )
	{
                BOOL ret = GetMessage(&msg, NULL, 0, 0);
                if (ret == 0 || ret == -1) {
                        break;
                } else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
                }
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
//	wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HOGEHOGE01));
	wcex.hIcon              = LoadIcon(NULL , IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground      = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HOGEHOGE01);
        wcex.lpszMenuName       = NULL;
	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        wcex.hIconSm            = wcex.hIcon;
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

     hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);


		switch (wmId)
		{

                case GS_RESET_BUTTON_ID:
                    {
                        int byteLength;
                        HWND hWnd001, hWnd002;
                        TCHAR *str1, *str2;

                        if ((threadExecFlag1 == 1) || (threadExecFlag2 == 1) || (threadExecFlag3 == 1) || (threadExecFlag4 == 1)) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }

                        if (g_handleThread1 != NULL && WaitForSingleObject(g_handleThread1, 0) == WAIT_TIMEOUT) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }
                        if (g_handleThread1 != NULL) CloseHandle(g_handleThread1);

                        threadExecFlag1 = 1;
                        threadEndFlag01 = 0;
                        threadParam01 = NULL;
                        g_handleThread1 = (HANDLE)_beginthreadex(NULL, 0, threadFunc01, threadParam01, 0, NULL);


                    }
                    break;

                case TEST01_BUTTON_ID:
                    {
                        int byteLength;
                        HWND hWnd001, hWnd002;
                        TCHAR *str1, *str2;

                        if ((threadExecFlag1 == 1) || (threadExecFlag2 == 1) || (threadExecFlag3 == 1) || (threadExecFlag4 == 1)) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }

                        if (g_handleThread2 != NULL && WaitForSingleObject(g_handleThread2, 0) == WAIT_TIMEOUT) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }
                        if (g_handleThread2 != NULL) CloseHandle(g_handleThread2);

                        threadExecFlag2 = 1;
                        threadEndFlag01 = 0;
                        threadParam01 = NULL;
                        g_handleThread2 = (HANDLE)_beginthreadex(NULL, 0, threadFunc02, threadParam01, 0, NULL);


                    }
                    break;

                case TEST02_BUTTON_ID:
                    {
                        int byteLength;
                        HWND hWnd001, hWnd002;
                        TCHAR *str1, *str2;

                        if ((threadExecFlag1 == 1) || (threadExecFlag2 == 1) || (threadExecFlag3 == 1) || (threadExecFlag4 == 1)) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }

                        if (g_handleThread3 != NULL && WaitForSingleObject(g_handleThread3, 0) == WAIT_TIMEOUT) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }
                        if (g_handleThread3 != NULL) CloseHandle(g_handleThread3);

                        threadExecFlag3 = 1;
                        threadEndFlag01 = 0;
                        threadParam01 = NULL;
                        g_handleThread3 = (HANDLE)_beginthreadex(NULL, 0, threadFunc03, threadParam01, 0, NULL);


                    }
                    break;


                case XG_RESET_BUTTON_ID:
                    {
                        int byteLength;
                        HWND hWnd001, hWnd002;
                        TCHAR *str1, *str2;

                        if ((threadExecFlag1 == 1) || (threadExecFlag2 == 1) || (threadExecFlag3 == 1) || (threadExecFlag4 == 1)) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }

                        if (g_handleThread4 != NULL && WaitForSingleObject(g_handleThread4, 0) == WAIT_TIMEOUT) {
                            AppendText(_T("実行中です\r\n"), g_hwndRichEdit);
                            break;
                        }
                        if (g_handleThread4 != NULL) CloseHandle(g_handleThread4);

                        threadExecFlag4 = 1;
                        threadEndFlag01 = 0;
                        threadParam01 = NULL;
                        g_handleThread4 = (HANDLE)_beginthreadex(NULL, 0, threadFunc04, threadParam01, 0, NULL);


                    }
                    break;

                case COMBBOX_ID:
                    {
                        UINT midiDeviceNum;
                        MIDIOUTCAPS outcaps;
                        TCHAR    strText[256];
                        int listnum01;

                        if (wmEvent == CBN_DROPDOWN) {



                            listnum01 = SendMessage(hwndCombo01 , CB_GETCOUNT , 0 , 0);
                            for (int i = 0; i < listnum01; i++) {
                                SendMessage(hwndCombo01, CB_DELETESTRING, 0 , 0);
                            }
                            midiDeviceNum = midiInGetNumDevs();
                            if (midiDeviceNum != 0) {
                                // MIDI output device
                                for (unsigned int i = 0; i < midiOutGetNumDevs(); i++) {
                                    midiOutGetDevCaps(i, &outcaps, sizeof(MIDIOUTCAPS));
                                    wsprintf(strText, _T("%d : %s"), i, outcaps.szPname);
                                    SendMessage(hwndCombo01 , CB_ADDSTRING , 0 , (LPARAM)strText);
                                }
                            }

                        }

                        if (wmEvent == CBN_SELCHANGE) {
                                uMidiDeviceID = SendMessage(hwndCombo01, CB_GETCURSEL , 0 , 0);
                                midiOutGetDevCaps(uMidiDeviceID, &outcaps, sizeof(MIDIOUTCAPS));
                                wsprintf(strText, _T("MIDI Device No : %d,  %s を選択しました\r\n"), uMidiDeviceID, outcaps.szPname);
                                AppendText(strText, g_hwndRichEdit);
                        }
                    }
                    break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;

        case WM_SIZE:
                {
                    int    clientx, clienty;
                    clientx = LOWORD(lParam);
                    clienty = HIWORD(lParam);

                    MoveWindow(g_hwndRichEdit, 0, 60 + 20 + 20, clientx, clienty - 50 - 60 - 20 - 20, TRUE);
                    MoveWindow(hwndCombo01,  0, 20, clientx - BUTTON_WIDTH - 20 - 10, 10 + 20 + 300, TRUE);
                    MoveWindow(hwndButton01, clientx -  BUTTON_WIDTH - 10, 20, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
                    MoveWindow(hwndButton02, clientx - (BUTTON_WIDTH + 5) * 1 - 5, clienty - BUTTON_HEIGHT - 10, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
                    MoveWindow(hwndButton03, clientx - (BUTTON_WIDTH + 5) * 2 - 5, clienty - BUTTON_HEIGHT - 10, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
                }
                break;

        case WM_CREATE:

                {   // RichEditコントロールの追加
                    RECT    rect;
                    GetClientRect(hWnd, &rect);
                    hRichLib = LoadLibrary("RICHED32.DLL");
                    g_hwndRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("RICHEDIT"), _T(""),
                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_HSCROLL |
                      WS_VSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL | WS_BORDER,
                      0, 60 + 20, rect.right, rect.bottom - 50 - 60 - 20, hWnd, (HMENU)IDC_RICHEDIT, hInst, NULL);

                    hRichEdit01Font01 = CreateFont(14, 0, 0, 0,
                       FW_NORMAL, FALSE, FALSE, 0,
                        SHIFTJIS_CHARSET,
                        OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ Ｐゴシック"); 


                    SendMessage(g_hwndRichEdit, WM_SETFONT, (WPARAM)hRichEdit01Font01, MAKELPARAM(FALSE, 0));

                }

                {
                    RECT   rect;
                    GetClientRect(hWnd, &rect);
                    hwndCombo01 = CreateWindow(WC_COMBOBOX, NULL,
                        WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                       0, 20, rect.right  - BUTTON_WIDTH - 20 - 10,  10 + 20 + 300,  hWnd, (HMENU)COMBBOX_ID, hInst, NULL);

                    // Fontの作成
                    hEdit01Font01 = CreateFont(14, 0, 0, 0,
                        FW_NORMAL, FALSE, FALSE, 0,
                        SHIFTJIS_CHARSET,
                        OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ Ｐゴシック"); 

                    SendMessage(hwndCombo01, WM_SETFONT, (WPARAM)hEdit01Font01, MAKELPARAM(FALSE, 0));

                }
                {
                    RECT    rect;
                    GetClientRect(hWnd, &rect);
                    hwndButton01 = CreateWindow(WC_BUTTON, _T("GS Reset"),
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        rect.right - BUTTON_WIDTH - 10, 20,
                        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)GS_RESET_BUTTON_ID, hInst, NULL);
                }

                {
                    RECT    rect;
                    GetClientRect(hWnd, &rect);
                    hwndButton04 = CreateWindow(WC_BUTTON, _T("XG On"),
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        rect.right - BUTTON_WIDTH - 10, 20 + BUTTON_HEIGHT + 10,
                        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)XG_RESET_BUTTON_ID, hInst, NULL);
                }

                {
                    RECT    rect;
                    GetClientRect(hWnd, &rect);
                    hwndButton02 = CreateWindow(WC_BUTTON, _T("TEST01"),
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        rect.right - (BUTTON_WIDTH + 5) * 1 - 5, rect.bottom - BUTTON_HEIGHT - 10,
                        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)TEST01_BUTTON_ID, hInst, NULL);
                }

                {
                    RECT    rect;
                    GetClientRect(hWnd, &rect);
                    hwndButton03 = CreateWindow(WC_BUTTON, _T("TEST02"),
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        rect.right - (BUTTON_WIDTH + 5) * 3 - 5, rect.bottom - BUTTON_HEIGHT - 10,
                        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, (HMENU)TEST02_BUTTON_ID, hInst, NULL);
                }

                g_handleThread1 = NULL;
                g_handleThread2 = NULL;
                g_handleThread3 = NULL;

                break;
	case WM_DESTROY:

                DeleteObject(hEdit01Font01);
                DeleteObject(hRichEdit01Font01);


		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int editboxTextLength(HWND hWnd) {
    int   length01;
    length01 = (int) SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
    return length01;
}

void editboxClear(HWND hWnd) {
    SetWindowText( hWnd, _T(""));
    SendMessage( hWnd, EM_REPLACESEL, FALSE, (LPARAM)(LPCTSTR)_T(""));
}


void AppendText(LPCTSTR str, HWND hWnd) {
    SendMessage(hWnd, EM_SETSEL, -1, -1);
    SendMessage(hWnd, EM_REPLACESEL, FALSE, (LPARAM)str);
    SetFocus(hWnd);
}


#define MIDIMSG(status,channel,data1,data2) ( (DWORD)((status<<4) | channel | (data1<<8) | (data2<<16)) )

unsigned _stdcall threadFunc01(void *threadParam01) {
    TCHAR   str1[256];
    long long time01;

    // midiデバイスのオープン
    if (MMSYSERR_NOERROR != midiOutOpen(&hMidiOut, uMidiDeviceID, NULL, 0, CALLBACK_NULL)) {
        wsprintf(str1, _T("MIDIデバイスが開けません\r\n"));
        AppendText(str1, g_hwndRichEdit);
        threadExecFlag1 = 0;
        _endthreadex((unsigned) 0 );
        return 0;
    }

    ZeroMemory(&mhMidi, sizeof(mhMidi));

    /* GMリセット送信用バッファ設定 */
    mhMidi.lpData = (LPSTR)gmReset;
    mhMidi.dwBufferLength = 6;
    mhMidi.dwBytesRecorded = 6;

    midiOutPrepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    /* GMリセットメッセージ送信 */
    midiOutLongMsg(hMidiOut, &mhMidi, sizeof(mhMidi));

    wsprintf(str1, _T("GM On送信\r\n"));
    AppendText(str1, g_hwndRichEdit);

    /* GMリセット完了待機 */
    time01 = GetTickCount64();
    while ((mhMidi.dwFlags & MHDR_DONE) == 0) {
        if ((GetTickCount64() - time01) >= 100LL) break;
    }

    midiOutUnprepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));



    ZeroMemory(&mhMidi, sizeof(mhMidi));

    /* GSリセット送信用バッファ設定 */
    mhMidi.lpData = (LPSTR)gsReset;
    mhMidi.dwBufferLength = 11;
    mhMidi.dwBytesRecorded = 11;

    midiOutPrepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    /* GSリセットメッセージ送信 */
    midiOutLongMsg(hMidiOut, &mhMidi, sizeof(mhMidi));

    wsprintf(str1, _T("GS Reset送信\r\n"));
    AppendText(str1, g_hwndRichEdit);

    /* GSリセット完了待機 */
    time01 = GetTickCount64();
    while ((mhMidi.dwFlags & MHDR_DONE) == 0) {
        if ((GetTickCount64() - time01) >= 100LL) break;
    }

    midiOutUnprepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);

    threadExecFlag1 = 0;

    _endthreadex((unsigned) 0 );

    return 0;
}

unsigned _stdcall threadFunc02(void *threadParam01) {
    TCHAR   str1[256];

    // midiデバイスのオープン
    if (MMSYSERR_NOERROR != midiOutOpen(&hMidiOut, uMidiDeviceID, NULL, 0, CALLBACK_NULL)) {
        wsprintf(str1, _T("MIDIデバイスが開けません\r\n"));
        AppendText(str1, g_hwndRichEdit);
        threadExecFlag2 = 0;
        _endthreadex((unsigned) 0 );
        return 0;
    }

    // Main Volume 127  MainVolume = ControlChange# 7  ControlChange=0x0b << 4 | channelNo
    mainVolume = 127;
    midiOutShortMsg(hMidiOut,MIDIMSG(0xB,0,7,mainVolume));

    // プログラムチェンジ
    program = 0;
    midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0));

    _stprintf(str1, _T("MainVolume = %d, Program = %d, note = %d, velecity = %d\r\n"), mainVolume, program, note, velocity);
    AppendText(str1, g_hwndRichEdit);

    // 鍵盤を押す
    midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
    Sleep(3000);  //millisecond

    // 鍵盤を離す
    midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);

    threadExecFlag2 = 0;

    _endthreadex((unsigned) 0 );

    return 0;
}

unsigned _stdcall threadFunc03(void *threadParam01) {
    TCHAR   str1[256];

    // midiデバイスのオープン
    if (MMSYSERR_NOERROR != midiOutOpen(&hMidiOut, uMidiDeviceID, NULL, 0, CALLBACK_NULL)) {
        wsprintf(str1, _T("MIDIデバイスが開けません\r\n"));
        AppendText(str1, g_hwndRichEdit);
        threadExecFlag3 = 0;
        _endthreadex((unsigned) 0 );
        return 0;
    }

    // Main Volume 127
    mainVolume = 127;
    midiOutShortMsg(hMidiOut,MIDIMSG(0xB,0,7,mainVolume));

    // プログラムチェンジ
    program = 81;
    midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0));

    _stprintf(str1, _T("MainVolume = %d, Program = %d, note = %d, velecity = %d\r\n"), mainVolume, program, note, velocity);
    AppendText(str1, g_hwndRichEdit);

    // 鍵盤を押す
    midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
    Sleep(3000);  //millisecond

    // 鍵盤を離す
    midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);

    threadExecFlag3 = 0;

    _endthreadex((unsigned) 0 );

    return 0;
}

unsigned _stdcall threadFunc04(void *threadParam01) {
    TCHAR   str1[256];
    long long   time01;

    // midiデバイスのオープン
    if (MMSYSERR_NOERROR != midiOutOpen(&hMidiOut, uMidiDeviceID, NULL, 0, CALLBACK_NULL)) {
        wsprintf(str1, _T("MIDIデバイスが開けません\r\n"));
        AppendText(str1, g_hwndRichEdit);
        threadExecFlag4 = 0;
        _endthreadex((unsigned) 0 );
        return 0;
    }

    ZeroMemory(&mhMidi, sizeof(mhMidi));

    /* GMリセット送信用バッファ設定 */
    mhMidi.lpData = (LPSTR)gmReset;
    mhMidi.dwBufferLength = 6;
    mhMidi.dwBytesRecorded = 6;

    midiOutPrepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    /* GMリセットメッセージ送信 */
    midiOutLongMsg(hMidiOut, &mhMidi, sizeof(mhMidi));

    _stprintf(str1, _T("GM On送信\r\n"));
    AppendText(str1, g_hwndRichEdit);

    /* GMリセット完了待機 */
    time01 = GetTickCount64();
    while ((mhMidi.dwFlags & MHDR_DONE) == 0) {
        if ((GetTickCount64() - time01) >= 100LL) break;
    }

    midiOutUnprepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));



    ZeroMemory(&mhMidi, sizeof(mhMidi));

    /* XGリセット送信用バッファ設定 */
    mhMidi.lpData = (LPSTR)xgReset;
    mhMidi.dwBufferLength = 9;
    mhMidi.dwBytesRecorded = 9;

    midiOutPrepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    /* XGリセットメッセージ送信 */
    midiOutLongMsg(hMidiOut, &mhMidi, sizeof(mhMidi));

    _stprintf(str1, _T("XG System On送信\r\n"));
    AppendText(str1, g_hwndRichEdit);

    /* XGリセット完了待機 */
    time01 = GetTickCount64();
    while ((mhMidi.dwFlags & MHDR_DONE) == 0) {
        if ((GetTickCount64() - time01) >= 100LL) break;
    }

    midiOutUnprepareHeader(hMidiOut, &mhMidi, sizeof(mhMidi));

    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);

    threadExecFlag4 = 0;

    _endthreadex((unsigned) 0 );

    return 0;
}
