#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// 定数
#define WINDOW_WIDTH    (640)        // ウィンドウの幅
#define WINDOW_HEIGHT   (700)        // ウィンドウの高さ
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)
#define HAT01_WIDTH     (640)
#define HAT01_HEIGHT    (515)
#define BITMAP_WIDTH    (640)
#define BITMAP_HEIGHT   (480)
#define TIMER_ID        (100)       // 作成するタイマの識別ID
#define TIMER_ELAPSE    (100)       // WM_TIMERの発生間隔
#define START_BUTTON_ID (100)       // 「開始」ボタンのID
#define BUTTON_WIDTH     (80)       // 「等倍」ボタンの幅
#define BUTTON_HEIGHT    (30)       // 「等倍」ボタンの高さ
#define MARGIN            (1)       // 上下左右の余白

// プロトタイプ宣言
long  far PASCAL MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

HINSTANCE hInst01;


struct LocalVariable01 {
    HBITMAP hBitmap;
    HBITMAP hOldBitmap;
    HDC     hBuffer;
    HDC      *gra1;
    HWND    g_hwndEdit;
    HANDLE  g_hat01Thread;
    HINSTANCE   hRichLib; 


    int flag01;

    long xdot, ydot;
    int flag1;
    int endFlag1;
    int startFlag1;

    HWND hWnd01;

    DWORD startTick, endTick;

    struct LocalVariable02  *pLV0002;

};

struct LocalVariable01  *global_pLV;

struct LocalVariable02 {
    long     i, x, y;
    double   dr, r, z, sx, sy;
    long     xmax, ymax;
    double   d[160];

    int      execflag1;
    int      setparamflag1;
    HDC      *gra1; 

    long     inum;
};

void Hat01_INIT(LocalVariable01  *pLV);                                       //初期化ルーチン
void hat01_start(LocalVariable01  *pLV);                                     //初期化ルーチンその２
void Hat01FuncInit(LocalVariable02  *pLV2, long xdot1, long ydot1, HDC  *gra2);
void hat01_run(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV);  //タイマーイベント処理ルーチン
void hat01(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV);
void hat01End(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV);   //終了ルーチン


char strbuffer01[1024];
void AppendText(LPSTR str, LocalVariable01  *pLV) ;                           // EditBox文字出力ルーチン


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

    while( 1 )
    {
        BOOL ret = GetMessage( &msg, NULL, 0, 0 );  // メッセージを取得する
        if( ret == 0 || ret == -1 )
        {
            break;
        }
        else
        {
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
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "WHat01WClass";
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
    GetWindowsVersion();
    HWND hWnd;
    hWnd = CreateWindow(
        "WHat01WClass",
        "WHAT01",
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

long  far PASCAL MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hDC;
    PAINTSTRUCT ps;
        int wmId, wmEvent;
        struct LocalVariable01  *pLV;

        pLV = global_pLV;


    switch( msg )
    {
        case WM_DESTROY:
                SelectObject(pLV->hBuffer, pLV->hOldBitmap);
                DeleteDC(pLV->hBuffer);
                DeleteObject(pLV->hBitmap);
                pLV->gra1 = NULL;

                if (pLV != NULL)   free((void *)pLV);
                global_pLV = NULL;

                PostQuitMessage(0);
                return 0;

        case WM_CREATE:

                pLV = (LocalVariable01  *)malloc(sizeof(LocalVariable01));
                if (pLV != 0) {
                        pLV->flag01 = 0;
                } else {
                        MessageBox( hWnd, "必要なメモリが確保できませんでした。", "エラー", MB_OK);
                        return -1;
                }



                //ボタンを配置
                {
                RECT rcClt;
                GetClientRect( hWnd, &rcClt);
                int xPos = (( rcClt.right - rcClt.left ) / 5) * 3 + BUTTON_WIDTH / 2;
                int yPos = HAT01_HEIGHT - BUTTON_HEIGHT - MARGIN;
                HWND hwndBtn = CreateWindow("BUTTON", "開始",
                        WS_TABSTOP| WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        xPos, yPos, BUTTON_WIDTH, BUTTON_HEIGHT,
                        hWnd, (HMENU)START_BUTTON_ID,
                        hInst01, NULL);
                }

                // EditBox表示
                {
                        RECT rect;
                        GetClientRect(hWnd, &rect);
                        pLV->g_hwndEdit = CreateWindow("EDIT", "",
                                WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                                0, HAT01_HEIGHT + MARGIN, rect.right, rect.bottom - (HAT01_HEIGHT + MARGIN), hWnd,
                                NULL, hInst01, NULL);
                        if(!pLV->g_hwndEdit)  return -1;
                }


                // ダブルバッファの作成
                hDC = GetDC(hWnd);
                pLV->hBitmap = CreateCompatibleBitmap(hDC, BITMAP_WIDTH, BITMAP_HEIGHT);
                pLV->hBuffer = CreateCompatibleDC(hDC);

                pLV->gra1 = &(pLV->hBuffer);
                pLV->hOldBitmap = (HBITMAP)SelectObject(*(pLV->gra1), pLV->hBitmap);
                ReleaseDC(hWnd, hDC);

                HPEN hpen05, holdpen05;
                HBRUSH hbrush05, holdbrush05;

                hpen05 = CreatePen( PS_SOLID, 1, RGB( 0,255,255));
                hbrush05 = CreateSolidBrush( RGB( 0,255,255));
                holdpen05 = SelectObject(*(pLV->gra1), hpen05);
                holdbrush05 = SelectObject(*(pLV->gra1), hbrush05);
                Rectangle(*(pLV->gra1), 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);
                SelectObject(*(pLV->gra1), holdpen05);
                SelectObject(*(pLV->gra1), holdbrush05);
                DeleteObject(hpen05);
                DeleteObject(hbrush05);

                global_pLV = pLV;

                Hat01_INIT(pLV);


                return 0;






        case WM_TIMER:               // タイマイベントメッセージ

                if( wp != TIMER_ID )
                {
                     break;          // 識別IDが一致しないタイマイベントメッセージはDefWindowProc()に任せる
                }

                if (pLV != NULL) {

                        hat01_run(pLV->pLV0002, pLV);
                        if (pLV->endFlag1 == 1) {
                            KillTimer(hWnd, TIMER_ID);
                        }
                 }

                InvalidateRect( hWnd, NULL, FALSE );

                return 0;

        // ボタン処理
        case WM_COMMAND:
                wmId = LOWORD(wp);
                wmEvent = HIWORD(wp);
                switch( wmId )
                {
		case START_BUTTON_ID:

			if (pLV->flag1 == 0) {
                            hat01_start(pLV);
                            SetTimer( hWnd, TIMER_ID, TIMER_ELAPSE, NULL );
                        } else {
                            AppendText("実行中です\r\n", pLV);
                        }

                        break;

        default:
            return DefWindowProc(hWnd, msg, wp, lp);

        }
                break;


        case WM_LBUTTONUP:
                // マウスボタンが離された時の処理

                return 0;



	case WM_PAINT:    // 無効領域があるとき
                // ダブルバッファの内容を描画
                hDC = BeginPaint( hWnd, &ps );
                BitBlt(hDC, 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT, *(pLV->gra1), 0, 0, SRCCOPY);
                EndPaint( hWnd, &ps );

                return 0;
        }


        return DefWindowProc( hWnd, msg, wp, lp );
}

void Hat01_INIT(LocalVariable01  *pLV){

    pLV->xdot = BITMAP_WIDTH;
    pLV->ydot = BITMAP_HEIGHT;

    pLV->flag1 = 0;
    pLV->endFlag1 = 0;
    pLV->startFlag1 = 0;
}


void  hat01_start(LocalVariable01  *pLV) {

    if (pLV->flag1 == 0) {

        // EditBoxにパラメータを表示
        sprintf(strbuffer01, "xdot = %ld    ydot = %ld\r\n",
              pLV->xdot, pLV->ydot);
        AppendText(strbuffer01, pLV);



        pLV->flag1 = 1;            //実行中
        pLV->endFlag1 = 0;

        pLV->startTick = (DWORD)GetTickCount();

        struct LocalVariable02  *pLV2;
        pLV2 = (LocalVariable02  *)malloc( sizeof(LocalVariable02));
        pLV->pLV0002 = pLV2;
        pLV2->execflag1 = 0;
        pLV2->setparamflag1 = 0;
        pLV2->inum = 0;

        Hat01FuncInit(pLV2, pLV->xdot, pLV->ydot, pLV->gra1);
        pLV->startFlag1 = 1;
    }
}

void Hat01FuncInit(LocalVariable02  *pLV2, long xdot1, long ydot1, HDC  *gra2){
    int i;

    pLV2->xmax = xdot1;
    pLV2->ymax = ydot1;
    pLV2->gra1 = gra2;
    for (i = 0; i < 160; i++) {
        pLV2->d[i] = 100.0;
    }
    pLV2->setparamflag1 = 1;

}

// Timerイベント処理内で呼んでいる関数
void hat01_run(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV){
    if (pLV->flag1 == 1) {
            if (pLV2->setparamflag1 == 1) {
                    hat01(pLV2, pLV);
            }
    }
}

// メインルーチン
void hat01(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV) {

    int     zz, px, py, f;
    int     xgap1, ygap1, colr, colg, colb;
    double  sx, sy;

    xgap1 = 4;;                // xの増分値
    ygap1 = 6;                 // yの増分値
    HPEN hpen05, holdpen05;
    HBRUSH hbrush05, holdbrush05;

    if (pLV2->inum == 0) {

        pLV2->execflag1 = 1;
        pLV2->dr = 3.141592653589793 / 180.0;
        pLV2->y = -180;
        pLV2->x = -180;

        // BITMAPクリア
        hpen05 = CreatePen( PS_SOLID, 1, RGB( 0,0,0));
        hbrush05 = CreateSolidBrush( RGB( 0,0,0));
        holdpen05 = SelectObject(*(pLV2->gra1), hpen05);
        holdbrush05 = SelectObject(*(pLV2->gra1), hbrush05);
        Rectangle(*(pLV2->gra1), 0, 0, (int)pLV2->xmax, (int)pLV2->ymax);
        SelectObject(*(pLV2->gra1), holdpen05);
        SelectObject(*(pLV2->gra1), holdbrush05);
        DeleteObject(hpen05);
        DeleteObject(hbrush05);
    }

    for (long i01 = 0; i01 < 50; i01++){

        pLV2->r = pLV2->dr * sqrt((double)pLV2->x * (double)pLV2->x + (double)pLV2->y * (double)pLV2->y);
        pLV2->z = 100 * cos(pLV2->r) - 30 * cos(3*pLV2->r);
        sx = 80 + pLV2->x / 3 - pLV2->y / 6;
        sy = 40 - pLV2->y / 6 - pLV2->z / 4;

        if (!((sx < 0) || (sx >= 160))) {
            if (!(pLV2->d[(int)sx] <= sy)) {

                zz = (int)((pLV2->z + 100) * 0.035) + 1;
                f = 0;
                colr = 0;
                colg = 0;
                colb = 0;

                if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                    colb = 255;
                    f = 1;
                }
                if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                    colr = 255;
                    f = 1;
                }
                if ((zz >= 4)) {
                    colg = 255;
                    f = 1;
                }
                px = (int)(sx * 2.0 * pLV2->xmax / 320.0);
                py = (int)(sy * 2.0 * pLV2->ymax / 200.0);

                if (f == 1)
                {
                    hpen05 = CreatePen( PS_SOLID, 1, RGB( colr, colg, colb));
                    hbrush05 = CreateSolidBrush(RGB( colr, colg, colb));
                    holdpen05 = SelectObject(*(pLV2->gra1), hpen05);
                    holdbrush05 = SelectObject(*(pLV2->gra1), hbrush05);

                    Rectangle( *(pLV2->gra1), px, py, px + 3, py + 3);

                    SelectObject(*(pLV2->gra1), holdpen05);
                    SelectObject(*(pLV2->gra1), holdbrush05);
                    DeleteObject(hpen05);
                    DeleteObject(hbrush05);
                }
                pLV2->d[(int)sx] = sy;
            }
        }
        pLV2->x = pLV2->x + xgap1;

        if(pLV2->x >= 180) {
            pLV2->x = -180;
            pLV2->y = pLV2->y + ygap1;
        }

        if ((pLV2->y >= 180)) {
            pLV2->execflag1 = 0;
            hat01End(pLV2, pLV);
            break;
        } else {
           pLV2->inum++;
        }
    }
}

// 終了処理
void hat01End(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV){
    if (pLV2->execflag1 == 0) {
            pLV->endFlag1 = 1;
    }
    if (pLV2 != NULL) {
          free((void *)pLV2);
          pLV->pLV0002 = NULL;
    }
    pLV->flag1 = 0;

    pLV->endTick = (DWORD)GetTickCount();
    sprintf(strbuffer01, "処理時間は %ld msです。\r\n", pLV->endTick - pLV->startTick);
    AppendText(strbuffer01, pLV);
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

void AppendText(LPSTR str, LocalVariable01  *pLV) 
{
    SendMessage(pLV->g_hwndEdit, EM_SETSEL, -1, -1);
    SendMessage(pLV->g_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)str);
    SetFocus(pLV->g_hwndEdit);
}
