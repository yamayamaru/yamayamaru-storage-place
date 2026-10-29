#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 定数
#define WINDOW_WIDTH    (500)        // ウィンドウの幅
#define WINDOW_HEIGHT   (480)        // ウィンドウの高さ
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)
#define HAT05_WIDTH     (500)
#define HAT05_HEIGHT    (380)
#define BITMAP_WIDTH    (500)
#define BITMAP_HEIGHT   (380)

#define TIMER_ID        (100)     // 作成するタイマの識別ID
#define TIMER_ELAPSE    (100)     // WM_TIMERの発生間隔
#define START_BUTTON_ID (100)     // 「開始」ボタンのID
#define BUTTON_WIDTH     (80)     // 「等倍」ボタンの幅
#define BUTTON_HEIGHT    (30)     // 「等倍」ボタンの高さ
#define MARGIN            (1)     // 上下左右の余白

#define PALETTE_ENTRY   161


HINSTANCE hInst01;

struct LocalVariable01 {
    HBITMAP   hBitmap;
    HBITMAP   hOldBitmap;
    HDC       hBuffer;
    HDC       *gra1;
    HWND      g_hwndEdit;
    HANDLE    g_hat05Thread;

    int       flag01;

    long      xdot, ydot;
    int       flag1;
    int       endFlag1;
    int       startFlag1;

    HWND      hWnd01;

    DWORD     startTick, endTick;

    struct LocalVariable02  *pLV0002;
};

struct LocalVariable01  *global_pLV;

struct LocalVariable02 {
    long     i, x1, y1;
    double   x, y;
    double   dr, r, z, sx, sy;
    long     xmax, ymax;
    double   d[160];

    long     loop_count;

    int      execflag1;
    int      setparamflag1;
    HDC      *gra1;

    long     inum;
};

LOGPALETTE   *lpPalette;
HPALETTE     hPalette;
COLORREF     color[PALETTE_ENTRY];
int          col2r[PALETTE_ENTRY];
int          col2g[PALETTE_ENTRY];
int          col2b[PALETTE_ENTRY];

// プロトタイプ宣言
LRESULT CALLBACK  MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void Hat05_INIT(LocalVariable01  *pLV);                 //初期化ルーチン
void hat05_start(LocalVariable01  *pLV);                //初期化ルーチンその２
void Hat05SetParam(LocalVariable02  *pLV2, long xdot1, long ydot1, HDC  *gra2);
void hat05_run(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV); //タイマーイベント処理ルーチン
void hat05(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV);
void hat05End(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV);  //終了ルーチン

char strbuffer01[1024];
void AppendText(LPSTR str, LocalVariable01  *pLV) ;     // EditBox文字出力ルーチン

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

// 開始位置
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
{
    MSG msg;

    hInst01 = hInst;

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
    wc.style         = 0;
    wc.lpfnWndProc   = MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  =  NULL;
    wc.lpszClassName = "WHat05WClass";
    return RegisterClass(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
    HWND hWnd;
    hWnd = CreateWindow(
        "WHat05WClass",
        "WHAT05",
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
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hDC;
    PAINTSTRUCT ps;
    int xpos01, ypos01, color01r, color01g, color01b;
    int wmId, wmEvent;
    HPEN hpen03;
    HBRUSH hbrush03;
    struct LocalVariable01  *pLV;
    int i;

    pLV = global_pLV;


    switch( msg )
    {
        case WM_DESTROY:          // ウィンドウが破棄されるとき
            SelectObject(pLV->hBuffer, pLV->hOldBitmap);
            DeleteDC(pLV->hBuffer);
            DeleteObject(pLV->hBitmap);
            pLV->gra1 = NULL;

            if (pLV != NULL)   free((void *)pLV);
            global_pLV = NULL;

            DeleteObject(hPalette);

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
            int yPos = HAT05_HEIGHT - BUTTON_HEIGHT - MARGIN;
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
                    0, HAT05_HEIGHT + MARGIN, rect.right, rect.bottom - (HAT05_HEIGHT + MARGIN), hWnd,
                    NULL, hInst01, NULL);
                if(!pLV->g_hwndEdit)  return -1;
            }

            // パレットの設定
            for (i = 0; i <= 160; i++) {
                col2r[i] = 0;
                col2g[i] = 0;
                col2b[i] = 0;
                if (i <= 31) {
                    col2b[i] = i;
                }
                if ((i > 32) && (i <= 63)) {
                    col2b[i] = (63 - i);
                    col2r[i] = (i -33);
                }
                if (i == 64) {
                    col2r[i] = (i - 33);
                }
                if ((i > 64) && (i <= 95)) {
                    col2r[i] = (95 - i);
                    col2g[i] = (i - 65);
                }
                if (i == 96) {
                    col2g[i] = (i - 65);
                }
                if ((i > 96) && (i <= 127)) {
                    col2g[i] = ((127 - i) + (i - 97));
                    col2b[i] = (i - 97);
                }
                if (i == 128) {
                    col2g[i] = (i - 97);
                    col2b[i] = (i - 97);
                }
                if ((i > 128) && (i <= 159)) {
                    col2g[i] = ((159 - i) + (i - 129));
                    col2b[i] = ((159 - i) + (i - 129));
                    col2r[i] = (i - 129);
                }
                if (i >= 160) {
                    col2g[i] = 31;
                    col2b[i] = 31;
                    col2r[i] = 31;
                }
            }

            lpPalette = (LOGPALETTE *)malloc(sizeof (LOGPALETTE) + (PALETTE_ENTRY + 1) * sizeof (PALETTEENTRY));
            lpPalette->palVersion = 0x0300;
            lpPalette->palNumEntries = PALETTE_ENTRY;

            for (i = 0 ; i < PALETTE_ENTRY ; i++) {
                // color[]にパレットの色を設定(PALETTERGB()で直接パレット番号を指定できる(同じ色のパレットが適用される))
                color[i] = PALETTERGB(col2r[i] * 8, col2g[i] * 8, col2b[i] * 8);
                // パレットデータを作成
                lpPalette->palPalEntry[i].peRed   = (BYTE)col2r[i] * 8;
                lpPalette->palPalEntry[i].peGreen = (BYTE)col2g[i] * 8;
                lpPalette->palPalEntry[i].peBlue  = (BYTE)col2b[i] * 8;
                lpPalette->palPalEntry[i].peFlags = NULL;
            }

            color[PALETTE_ENTRY] = PALETTERGB(255 , 255 , 255);
            lpPalette->palPalEntry[PALETTE_ENTRY].peRed   = 255;
            lpPalette->palPalEntry[PALETTE_ENTRY].peGreen = 255;
            lpPalette->palPalEntry[PALETTE_ENTRY].peBlue  = 255;

            hPalette = CreatePalette(lpPalette);
            free(lpPalette);

            // ダブルバッファの作成
            hDC = GetDC(hWnd);
            pLV->hBitmap = CreateCompatibleBitmap(hDC, BITMAP_WIDTH, BITMAP_HEIGHT);
            pLV->hBuffer = CreateCompatibleDC(hDC);

            pLV->gra1 = &(pLV->hBuffer);
            pLV->hOldBitmap = (HBITMAP)SelectObject(*(pLV->gra1), pLV->hBitmap);


            // ダブルバッファにパレット設定  *(pLV->gra1) == pLV->hBuffer
            SelectPalette(*(pLV->gra1) , hPalette , FALSE);
            RealizePalette(*(pLV->gra1));

            ReleaseDC(hWnd, hDC);

            HPEN hpen05, holdpen05;
            HBRUSH hbrush05, holdbrush05;

            //SelectObject()で変更したら、必ず元に戻すこと(そうしないとオブジェクトが消えない模様なのでゴミが残る)
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

            Hat05_INIT(pLV);

            return 0;

        case WM_TIMER:          // タイマメッセージ

            if( wp != TIMER_ID ) {
                break;  // 識別IDが一致しないタイマメッセージはDefWindowProc()に任せる
            }

            if (pLV != NULL) {
                hat05_run(pLV->pLV0002, pLV);
                if (pLV->endFlag1 == 1) {
                    KillTimer(hWnd, TIMER_ID);
                }
            }

            InvalidateRect( hWnd, NULL, FALSE );
            return 0;

        case WM_COMMAND:
            wmId = LOWORD(wp);
            wmEvent = HIWORD(wp);
            switch( wmId )
            {
                case START_BUTTON_ID:

                    if (pLV->flag1 == 0) {
                        hat05_start(pLV);
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
            // マウスボタンを離す場合
            return 0;

        case WM_PAINT: 
            // 無効領域があるとき
            // バックバッファの内容を描画
            hDC = BeginPaint( hWnd, &ps );

            SelectPalette(hDC , hPalette , FALSE);
            RealizePalette(hDC);

            BitBlt(hDC, 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT, *(pLV->gra1), 0, 0, SRCCOPY);

            EndPaint( hWnd, &ps );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wp, lp );
}

void Hat05_INIT(LocalVariable01  *pLV){
    pLV->xdot = BITMAP_WIDTH;
    pLV->ydot = BITMAP_HEIGHT;

    pLV->flag1 = 0;
    pLV->endFlag1 = 0;
    pLV->startFlag1 = 0;
}

void  hat05_start(LocalVariable01  *pLV) {
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

        Hat05SetParam(pLV2, pLV->xdot, pLV->ydot, pLV->gra1);
        pLV->startFlag1 = 1;
    }
}

void Hat05SetParam(LocalVariable02  *pLV2, long xdot1, long ydot1, HDC  *gra2){
    int i;

    pLV2->xmax = xdot1;
    pLV2->ymax = ydot1;
    pLV2->gra1 = gra2;

    for (i = 0; i < 160; i++) {
        pLV2->d[i] = 100.0;
    }

    pLV2->setparamflag1 = 1;
}

void hat05_run(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV){
    if (pLV->flag1 == 1) {
        if (pLV2->setparamflag1 == 1) {
            hat05(pLV2, pLV);
        }
    }
}

void hat05(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV) {
    int     zz, px, py, f;
    int     xgap1, ygap1, sxi;
    double  sx, sy;
    DWORD   tick_loop_start;
    int     loop_max = 10000;
    int     fastcallflag01;
    long    i01;

    xgap1 = 2;
    ygap1 = 10;
    HPEN hpen05, holdpen05;
    HBRUSH hbrush05, holdbrush05;
    fastcallflag01 = 0;

    if (pLV2->inum == 0) {
        pLV2->execflag1 = 1;
        pLV2->dr = 3.141592653589793 / 180.0;
        pLV2->y1 = -1800;
        pLV2->x1 = -1800;
        pLV2->y = pLV2->y1 / 10.0;

        //SelectObject()で変更したら、必ず元に戻すこと(そうしないとオブジェクトが消えない模様なのでゴミが残る)
        hpen05 = CreatePen( PS_SOLID, 1, RGB( 0,0,0));
        hbrush05 = CreateSolidBrush( RGB( 0,0,0));
        holdpen05 = SelectObject(*(pLV2->gra1), hpen05);
        holdbrush05 = SelectObject(*(pLV2->gra1), hbrush05);
        Rectangle(*(pLV2->gra1), 0, 0, (int)pLV2->xmax, (int)pLV2->ymax);
        SelectObject(*(pLV2->gra1), holdpen05);
        SelectObject(*(pLV2->gra1), holdbrush05);
        DeleteObject(hpen05);
        DeleteObject(hbrush05);

        tick_loop_start = (DWORD)GetTickCount();
        pLV2->loop_count = loop_max;
        fastcallflag01 = 1;
    }

    for (i01 = 0; i01 < pLV2->loop_count; i01++){
        pLV2->x = pLV2->x1 / 10.0;
        pLV2->r = pLV2->dr * sqrt(pLV2->x * pLV2->x + pLV2->y * pLV2->y);
        pLV2->z = 100 * cos(pLV2->r) - 30 * cos(3*pLV2->r);
        sx = 80 + pLV2->x / 3 - pLV2->y / 6;
        sy = 40 - pLV2->y / 6 - pLV2->z / 4;
        sxi = (int)sx;

        if (!((sxi < 0) || (sxi >= 160))) {
            if (!(pLV2->d[sxi] <= sy)) {

                zz = (int)((pLV2->z + 100) * 0.035 / 7.0 * 160.0) + 1;
                f = 1;

                if (zz > 160) zz = 160;
                px = (int)(sx * 2.0 * pLV2->xmax / 320.0);
                py = (int)(sy * 2.0 * pLV2->ymax / 200.0);

                if (f == 1)
                {
                    SetPixel( *(pLV2->gra1), px, py, color[zz]); 
                }
                pLV2->d[sxi] = sy;
            }
        }
        pLV2->x1 = pLV2->x1 + xgap1;

        if(pLV2->x1 >= 1800) {
            pLV2->x1 = -1800;
            pLV2->y1 = pLV2->y1 + ygap1;
            pLV2->y = pLV2->y1 / 10.0;
//              if (i % 16 == 0) InvalidateRect( pLV2->hWnd, NULL, FALSE );
        }

        if ((pLV2->y1 >= 1800)) {
            pLV2->execflag1 = 0;
            hat05End(pLV2, pLV);
            break;
        } else {
           pLV2->inum++;
        }

        if (fastcallflag01 == 1) {
            if (((DWORD)GetTickCount() - tick_loop_start) >= 100) break;
        }
    }
    if (fastcallflag01 == 1) {
        pLV2->loop_count = (long)i01 * 10 / 7;
        sprintf(strbuffer01, "one_call_loop_count = %ld\r\n", pLV2->loop_count);
        AppendText(strbuffer01, pLV);
    }
}


void hat05End(struct LocalVariable02  *pLV2, struct LocalVariable01  *pLV){
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
    sprintf(strbuffer01, "sizeof(int)      = %d bytes\r\n", sizeof(int));
    AppendText(strbuffer01, pLV);
    sprintf(strbuffer01, "sizeof(void *) = %d bytes\r\n", sizeof(void *));
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
