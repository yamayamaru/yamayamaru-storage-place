//
//    FM TOWNSエミュレータの津軽でTCP/IPに飛ばしたRS232Cデータを受けてRS232Cに流すプログラム
//
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include <synchapi.h>

#pragma comment(lib,"ws2_32.lib")

int source_port;

HANDLE hComPort;                               // COMポートのハンドルを入れる変数
TCHAR *tchar01;                                // COMポートのポート名の文字列を入れる変数

int  dstSocket01, srcSocket01;

volatile int endflag01 = 0;
volatile int execflag01 = 0;

struct param01 {
    HANDLE  hComPort;
    volatile int *pendflag01;
    volatile int *pexecflag01;
    int dstSocket;
};

int serial01(){
    // 1.COMポートを開く
    hComPort = CreateFile(                     //ファイルとしてポートを開く
             tchar01,                          // ポート名を指すバッファへのポインタ:COM??を開く
             GENERIC_READ | GENERIC_WRITE,     // アクセスモード:読み書き両方する
             0,                                //ポートの共有方法を指定:オブジェクトは共有しない
             NULL,                             //セキュリティ属性:ハンドルを子プロセスへ継承しない
             OPEN_EXISTING,                    //ポートを開き方を指定:既存のポートを開く
             0,                                //ポートの属性を指定:同期  非同期にしたいときはFILE_FLAG_OVERLAPPED
             NULL                              // テンプレートファイルへのハンドル:NULLでいい
     );
    if (hComPort == INVALID_HANDLE_VALUE){               //ポートの取得に失敗
        fprintf(stderr, "指定COMポートが開けません.\n\r");
        CloseHandle(hComPort);                              //ポートを閉じる
        return 0;
    }
    else{
        fprintf(stderr, "COMポートは正常に開けました.\n\r");
    }
    // ポートをファイルとみなしCreateFile()関数を用いて開きます。
    // 失敗するとINVALID_HANDLE_VALUEを返します。
    // ポートの属性は非同期（FILE_FLAG_OVERLAPPED）にした方がいいらしいですが上手くいかなかったので同期通信にしてあります。
    // 2.送受信バッファの設定
    // SetupComm()関数を用いて送受信バッファの設定をします。
    int check;                                  //エラーチェック用の変数
    check = SetupComm(
          hComPort,                             //COMポートのハンドラ
          1024,                                 //受信バッファサイズ:1024byte
          1024                                  //送信バッファ:1024byte
     );
    if (check == FALSE){
        fprintf(stderr, "送受信バッファの設定ができません.\r\n");
        CloseHandle(hComPort);
        return 0;
    }
    else{
        fprintf(stderr, "送受信バッファの設定が完了しました.\r\n");
    }


    // 3.送受信バッファの初期化
    // PurgeComm()関数を用いて出入力すべてのバッファをクリアします。
    check = PurgeComm(
          hComPort,                                                       // COMポートのハンドラ
          PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR   // 出入力バッファをすべてクリア
    );
    if (check == FALSE){
        fprintf(stderr, "送受信バッファの初期化ができません.\r\n");
        CloseHandle(hComPort);
        return 0;
    } else{
         fprintf(stderr, "送受信バッファの初期化が完了しました.\r\n");
    }

    // 4.COMポート構成情報の初期化
    // DCB構造体で構成情報を設定します
    // ボーレートやデータサイズ、パリティビット、ストップビットなどはマイコン側とあわせてください。
    // 書き換えた後にSetCommState()関数で再設定を行います
    DCB dcb;                                   // 構成情報を記録する構造体の生成
    GetCommState(hComPort, &dcb);              // 現在の設定値を読み込み
    dcb.DCBlength = sizeof(DCB);               // DCBのサイズ
    dcb.BaudRate = 38400;                      // ボーレート:38400bps
    dcb.ByteSize = 8;                          // データサイズ:8bit
    dcb.fBinary = TRUE;                        // バイナリモード:通常TRUE
    dcb.fParity = NOPARITY;                    // パリティビット:パリティビットなし
    dcb.StopBits = ONESTOPBIT;                 // ストップビット:1bit
   
    dcb.fOutxCtsFlow = TRUE;                   // CTSフロー制御:フロー制御なし
    dcb.fOutxDsrFlow = FALSE;                  // DSRハードウェアフロー制御：使用しない
    dcb.fDtrControl = DTR_CONTROL_DISABLE;     // DTR有効/無効:DTR無効
    dcb.fRtsControl = RTS_CONTROL_ENABLE;      // RTSフロー制御:RTS制御なし
   // dcb.fOutxCtsFlow = TRUE;                 // CTSフロー制御:フロー制御あり
   // dcb.fOutxDsrFlow = TRUE;                 // DSRハードウェアフロー制御：使用する
   // dcb.fDtrControl = DTR_CONTROL_ENABLE;    // DTR有効/無効:DTR有効
   // dcb.fRtsControl = RTS_CONTROL_ENABLE;    // RTSフロー制御:RTS制御あり
   
    dcb.fOutX = FALSE;                         // 送信時XON/XOFF制御の有無:なし
    dcb.fInX = FALSE;                          // 受信時XON/XOFF制御の有無:なし
    dcb.fTXContinueOnXoff = TRUE;              // 受信バッファー満杯＆XOFF受信後の継続送信可否:送信可
    dcb.XonLim = 512;                          // XONが送られるまでに格納できる最小バイト数:512
    dcb.XoffLim = 512;                         // XOFFが送られるまでに格納できる最小バイト数:512
    dcb.XonChar = 0x11;                        // 送信時XON文字 ( 送信可：ビジィ解除 ) の指定:XON文字として11H ( デバイス制御１：DC1 )
    dcb.XoffChar = 0x13;                       // XOFF文字（送信不可：ビジー通告）の指定:XOFF文字として13H ( デバイス制御3：DC3 )
   
    dcb.fNull = FALSE;                         // NULLバイトの破棄:破棄しない
    dcb.fAbortOnError = TRUE;                  // エラー時の読み書き操作終了:終了する
    dcb.fErrorChar = FALSE;                    // パリティエラー発生時のキャラクタ（ErrorChar）置換:なし
    dcb.ErrorChar = 0x00;                      // パリティエラー発生時の置換キャラクタ
    dcb.EofChar = 0x03;                        // データ終了通知キャラクタ:一般に0x03(ETX)がよく使われます。
    dcb.EvtChar = 0x02;                        // イベント通知キャラクタ:一般に0x02(STX)がよく使われます
   
    check = SetCommState(hComPort, &dcb);      // 設定値の書き込み
    if (check == FALSE){//エラーチェック
        fprintf(stderr, "COMポート構成情報の変更に失敗しました.\r\n");
        CloseHandle(hComPort);
        return 0;
    } else{
         fprintf(stderr, "COMポート構成情報を変更しました.\r\n");
    }


    // 5.タイムアウト時間の設定
    // 構成情報の設定と似た手順です
    // COMMTIMEOUTS構造体を使い、GetCommTimeouts()関数で基本情報を取得し、
    // 変更したのちSetCommTimeouts()関数で書き換えています
    // 各値はよく知りませんがこんな感じが多いです。

    COMMTIMEOUTS TimeOut;                            // COMMTIMEOUTS構造体の変数を宣言
    GetCommTimeouts(hComPort, &TimeOut);             // タイムアウトの設定状態を取得
   
    //(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier × (受信予定バイト数) + ReadTotalTimeoutConstant
    TimeOut.ReadTotalTimeoutMultiplier = 0;          // 読込の１文字あたりの時間:タイムアウトなし
    TimeOut.ReadTotalTimeoutConstant = 1;         // 読込エラー検出用のタイムアウト時間

    //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier ×(送信予定バイト数) + WriteTotalTimeoutConstant
    TimeOut.WriteTotalTimeoutMultiplier = 0;         // 書き込み１文字あたりの待ち時間:タイムアウトなし
    TimeOut.WriteTotalTimeoutConstant = 1000;        // 書き込みエラー検出用のタイムアウト時間
   
    check = SetCommTimeouts(hComPort, &TimeOut);     // タイムアウト設定の書き換え

    if (check == FALSE){//エラーチェック
        fprintf(stderr, "タイムアウトの設定に失敗しました.\r\n");
        CloseHandle(hComPort);
        return 0;
    }
    else{
         fprintf(stderr, "タイムアウトの設定に成功しました.\r\n");
    }


    // 6.送信
    // WriteFile()関数を用いて送信
    // 送信するデータサイズはNULL文字の分+1するのを忘れない

    int  port;
    int  dstSocket, srcSocket;
    struct sockaddr_in dstAddr, srcAddr;
    int  dstAddrSize;

    // 各種パラメータ
    char buffer[1024];

    // Windows の場合
    WSADATA data;
    WSAStartup(MAKEWORD(2,0), &data);


    port = source_port;
    // sockaddr_in 構造体のセット
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    dstAddrSize = sizeof(dstAddr);


    // ソケットの生成
    srcSocket = socket(AF_INET, SOCK_STREAM, 0);
    srcSocket01 = srcSocket;
    // ソケットのバインド
    bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));

    // 接続の許可
    listen(srcSocket, 10);

    //接続
    endflag01 = 0;
    while(1){   //ループで回すことによって何度でもクライアントからつなぐことができる
        unsigned int _stdcall ConsoleToSerial(void *data);
        HANDLE   hThread01 = NULL;
        struct   param01 *p01;

        printf("ポート:%dで接続を待っています\nクライアントプログラムを動かしてください\n", port);
        dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
        dstSocket01 = dstSocket;
        if (dstSocket > 0) {
            printf("%s から接続を受けました socket = %d\n", inet_ntoa(dstAddr.sin_addr), dstSocket);

            execflag01 = 1;
            p01 = (struct param01 *) malloc(sizeof(struct param01));
            p01->hComPort = hComPort;
            p01->pendflag01 = &endflag01;
            p01->pexecflag01 = &execflag01;
            p01->dstSocket = dstSocket;
            hThread01 = (HANDLE)_beginthreadex(NULL, 0, &ConsoleToSerial, (void *)p01, 0, NULL);
        }

        while ((execflag01 != 0) && (endflag01 == 0)) {
            Sleep((DWORD)10);
        };

        if (dstSocket01 > 0) closesocket(dstSocket01);

        if (hThread01 != NULL) {
            // スレッド終了待ち
            WaitForSingleObject(hThread01, INFINITE);
            // スレッド削除
            CloseHandle(hThread01);
            hThread01 = NULL;
        }
        if (endflag01 != 0) break;
    }

    if (srcSocket01 > 0) closesocket(srcSocket01);

    WSACleanup();

    return 0;
}

unsigned int _stdcall ConsoleToSerial(void *data) {

    char buffer[512], buffer2[512];
    char crlfbuf[3] = { 0x0d, 0x0a, 0x00 };
    int SendSize;
    DWORD writeSize, readSize;
    HANDLE hComPort;
    int dstSocket;
    char *bufpos;
    struct param01 *p01;
    int numrcv, numrcv2;
    int rcv_data_size, rcv_data_size2;
    int count, count1, count2, count21;
    int i;

    struct timeval t_val = {0, 0};    // 1/1000秒
    int select_ret;
    fd_set fds_r, readfds;
    fd_set fds_w, writefds;

    count = 0;
    count1 = 0;
    count2 = 0;
    count21 = 0;
    rcv_data_size = 0;
    rcv_data_size2 = 0;
    p01 = (struct param01 *)data;
    hComPort = p01->hComPort;
    dstSocket = p01->dstSocket;
    *(p01->pexecflag01) = 1;

    FD_ZERO(&readfds);
    FD_SET(dstSocket, &readfds);

    while(1) {
        int write_num = 0;
        int i_num01;

        if (*(p01->pendflag01) != 0) {
            break;
        }
        // socketからの1文字読み込み(selectを使って1文字受信した時のみrecvする)
        memcpy(&fds_r, &readfds, sizeof(fd_set));
        select_ret = select(0, &fds_r, NULL, NULL, &t_val);
        if (select_ret == SOCKET_ERROR) break;
        if (select_ret != 0) {
            numrcv = recv(dstSocket, buffer, 6, 0);
            if (numrcv == 0 || numrcv == -1) {
                goto goto_label01;
            }
            if (numrcv > 0) {
                write_num = 0;
                rcv_data_size += numrcv;
                bufpos = buffer;
                SendSize = 1;
                for (i_num01 = 0; i_num01 < 10; i_num01++) {
                    WriteFile(hComPort, bufpos + write_num, numrcv - write_num, &writeSize, NULL);
                    write_num += writeSize;
                    if ((write_num - numrcv) >= 0) break;
                }
                FlushFileBuffers(hComPort);
                if (count >= 100) {
                    count = 0;
                    count1++;
                    printf("send data = %d\n", rcv_data_size);
                }
                count++;
            }
        }
        if (*(p01->pendflag01) != 0) {
            break;
        }

        // serialからの1文字読み込み
        ReadFile(hComPort, buffer2, 512, &readSize, 0); // シリアルポートに対する読み込み
        numrcv2 = readSize;
        if (numrcv2 > 0) {
            rcv_data_size2 += numrcv2;
            send(dstSocket, buffer2, numrcv2, 0);
            if (count2 >= 100) {
                count2 = 0;
                count21++;
                printf("serial recv data = %d\n", rcv_data_size2);
            }
            count2++;
        }
    }
    goto_label01:
    printf("接続を終了しました\n\n");
    *(p01->pexecflag01) = 0;
    free(p01);
    _endthreadex(0);
    return 0;
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

int main(int argc, char **argv)
{

    char *char01;
  
  
    if (argc != 3) {
        fprintf(stderr, "Usage: %s  com_port_number port_number\n", argv[0]);
        exit(0);
    }
    source_port = atoi(argv[2]);
  
    char01 = argv[1];
  
    if (!strcmp(char01, "1")) {
        tchar01 = _T("COM1");
    } else if (!strcmp(char01, "2")) {
        tchar01 = _T("COM2");
    } else if (!strcmp(char01, "3")) {
        tchar01 = _T("COM3");
    } else if (!strcmp(char01, "4")) {
        tchar01 = _T("COM4");
    } else if (!strcmp(char01, "5")) {
        tchar01 = _T("COM5");
    } else if (!strcmp(char01, "6")) {
        tchar01 = _T("COM6");
    } else if (!strcmp(char01, "7")) {
        tchar01 = _T("COM7");
    } else if (!strcmp(char01, "8")) {
        tchar01 = _T("COM8");
    } else if (!strcmp(char01, "9")) {
        tchar01 = _T("COM9");
    } else if (!strcmp(char01, "10")) {
        tchar01 = _T("\\\\.\\COM10");
    } else if (!strcmp(char01, "11")) {
        tchar01 = _T("\\\\.\\COM11");
    } else if (!strcmp(char01, "12")) {
        tchar01 = _T("\\\\.\\COM12");
    } else if (!strcmp(char01, "13")) {
        tchar01 = _T("\\\\.\\COM13");
    } else if (!strcmp(char01, "14")) {
        tchar01 = _T("\\\\.\\COM14");
    } else if (!strcmp(char01, "15")) {
        tchar01 = _T("\\\\.\\COM15");
    } else if (!strcmp(char01, "16")) {
        tchar01 = _T("\\\\.\\COM16");
    } else if (!strcmp(char01, "17")) {
        tchar01 = _T("\\\\.\\COM17");
    } else if (!strcmp(char01, "18")) {
        tchar01 = _T("\\\\.\\COM18");
    } else if (!strcmp(char01, "19")) {
        tchar01 = _T("\\\\.\\COM19");
    } else if (!strcmp(char01, "20")) {
        tchar01 = _T("\\\\.\\COM20");
    } else if (!strcmp(char01, "21")) {
        tchar01 = _T("\\\\.\\COM21");
    } else if (!strcmp(char01, "22")) {
        tchar01 = _T("\\\\.\\COM22");
    } else if (!strcmp(char01, "23")) {
        tchar01 = _T("\\\\.\\COM23");
    } else if (!strcmp(char01, "24")) {
        tchar01 = _T("\\\\.\\COM24");
    } else if (!strcmp(char01, "25")) {
        tchar01 = _T("\\\\.\\COM25");
    } else if (!strcmp(char01, "26")) {
        tchar01 = _T("\\\\.\\COM26");
    } else if (!strcmp(char01, "27")) {
        tchar01 = _T("\\\\.\\COM27");
    } else if (!strcmp(char01, "28")) {
        tchar01 = _T("\\\\.\\COM28");
    } else if (!strcmp(char01, "29")) {
        tchar01 = _T("\\\\.\\COM29");
    } else if (!strcmp(char01, "30")) {
        tchar01 = _T("\\\\.\\COM30");
    } else if (!strcmp(char01, "31")) {
        tchar01 = _T("\\\\.\\COM31");
    } else if (!strcmp(char01, "32")) {
        tchar01 = _T("\\\\.\\COM32");
    } else if (!strcmp(char01, "33")) {
        tchar01 = _T("\\\\.\\COM33");
    } else if (!strcmp(char01, "34")) {
        tchar01 = _T("\\\\.\\COM34");
    } else if (!strcmp(char01, "35")) {
        tchar01 = _T("\\\\.\\COM35");
    } else if (!strcmp(char01, "36")) {
        tchar01 = _T("\\\\.\\COM36");
    } else if (!strcmp(char01, "37")) {
        tchar01 = _T("\\\\.\\COM37");
    } else if (!strcmp(char01, "38")) {
        tchar01 = _T("\\\\.\\COM38");
    } else if (!strcmp(char01, "39")) {
        tchar01 = _T("\\\\.\\COM39");
    } else {
        fprintf(stderr, "com_port_numberが不正です\n");
        exit(0);
    }
  
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
  
    serial01();

    CloseHandle(hComPort); 
    exit(0);
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    case CTRL_LOGOFF_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    case CTRL_SHUTDOWN_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    default:
        return TRUE;
    }
}
