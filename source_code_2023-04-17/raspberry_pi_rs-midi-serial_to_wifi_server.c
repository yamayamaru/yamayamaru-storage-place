/*
  このプログラムはUSB-SerialでRS-MIDIのデータを受けてネットワークに飛ばすプログラムです
  別のクライアントプログラムで受けてクライアント側のUSB-RS232CにつないだRS-MIDI対応のMIDI音源を鳴らせます

  WindowsのPCではHairless MIDI<->Serial Bridgeというフリーウェアを使うことで
  MIDIからSerialのデータに変換できます
  サーバプログラムとクライアントプログラムを両方同じLinuxマシンで実行することでUSB-Serialで受けて
  USB-RS232CにつながっているRS-MIDI対応のMIDI音源に1台で鳴らすことができます。

コンパイル方法
      gcc -O2 -o raspberry_pi_rs-midi-serial_to_wifi_server raspberry_pi_rs-midi-serial_to_wifi_server.c -lpthread

  41行目の "/dev/ttyUSB0" の部分はご自分のUSB-Serialアダプタのデバイスを指定してください。 

このプログラムで生じた損害などは一切保障しません
  自己責任で行ってください
*/


#include<stdio.h>
#include<stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include <signal.h>


#define SERIAL_PORT0 "/dev/ttyUSB0"
#define BAUDRATE0 B38400                    /* ボーレートの設定 */

#define PORT 4242

int fd0; /* シリアル通信ファイルディスクリプタ */

struct termios newtio0, oldtio0;     /* シリアル通信設定 */
void sigint_handler(int sig);



void *ClientSession(void *data);

struct param01 {
    int      socket;
    char     *buffer;
};

volatile int endflag = 0;

int srcSocket;                   /* 自分 */
pthread_t pthread01;


int main() {

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if(!(fd0 = open(SERIAL_PORT0, O_RDWR))) return -1; /* デバイスをオープンする */
    ioctl(fd0, TCGETS, &oldtio0);       /* 現在のシリアルポートの設定を待避 */

    bzero(&newtio0, sizeof(newtio0));
    newtio0 = oldtio0;                  /* ポートの設定をコピー */


    newtio0.c_cflag += CREAD;               /* 受信有効 */
    newtio0.c_cflag += CLOCAL;              /* ローカルライン（モデム制御なし）*/
    newtio0.c_cflag += CS8;                 /* データビット:8bit */
    newtio0.c_cflag += 0;                   /* ストップビット:1bit */
    newtio0.c_cflag += 0;                   /* パリティ:None */

    cfsetispeed(&newtio0, BAUDRATE0);
    cfsetospeed(&newtio0, BAUDRATE0);

    cfmakeraw(&newtio0);                    /* RAWモード */

    tcsetattr(fd0, TCSANOW, &newtio0);     /* デバイスに設定を行う */

    ioctl(fd0, TCSETS, &newtio0);       /* ポートの設定を有効にする */




    /* ポート番号，ソケット */
    int dstSocket;  /* 相手 */

    /* sockaddr_in 構造体 */
    struct sockaddr_in srcAddr;
    struct sockaddr_in dstAddr;
    int dstAddrSize = sizeof(dstAddr);
    int status;
    struct param01 *p01;

    char buffer[1024];

    /* sockaddr_in 構造体のセット */
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(PORT);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* ソケットの生成（ストリーム型） */
    srcSocket = socket(AF_INET, SOCK_STREAM, 0);
    /* ソケットのバインド */
    bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
    /* 接続の許可 */
    listen(srcSocket, 1);

    while(1){
        /* 接続の受付け */
        printf("ポート:%dで接続を待っています\n", PORT);
        dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
        printf("%s から接続を受けました socket = %d\n", inet_ntoa(dstAddr.sin_addr), dstSocket);

        p01 = (struct param01 *)malloc(sizeof(struct param01));
        p01->buffer = (char *)malloc(1);
        p01->socket = dstSocket;

        endflag = 0;
        status = pthread_create(&pthread01, NULL, ClientSession, (void *)p01);    /* スレッド作成 */


        while (endflag == 0);
	break;
    }

    void *pthread_ret;
    status = pthread_join(pthread01, (void *)pthread_ret);     /* 指定したスレッドが終了するまで 待機 */
    close(srcSocket);

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                         /* デバイスのクローズ */

    return(0);
}

void *ClientSession(void *data)
{
    struct param01 *p01 = (struct param01 *)data;
    int dstSocket = p01->socket;
    char   *buffer = p01->buffer;
    int    i;
    int status;

    while (endflag == 0) {
        ssize_t numrcv = read(fd0, buffer, 1);
        /* パケットの送信 */
        send(dstSocket, buffer, numrcv, 0);
    }
    printf("接続終了 socket=%d\n", p01->socket);
    free(p01->buffer);
    close(p01->socket);
    free(p01);

    return NULL;
}

void sigint_handler(int sig) {
    endflag == 1;
    
    void *pthread_ret;
    int status = pthread_join(pthread01, (void *)pthread_ret);     /* 指定したスレッドが終了するまで 待機 */
    close(srcSocket);

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                         /* デバイスのクローズ */
    exit(1);
}
