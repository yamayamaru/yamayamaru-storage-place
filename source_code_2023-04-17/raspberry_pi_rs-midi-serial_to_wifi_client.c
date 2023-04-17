/*
  このプログラムはサーバプログラムでネットワークに飛ばしたRS-MIDIのデータを受け取って
  USB-RS232CにつないだRS-MIDI対応MIDI音源を鳴らします
  起動するとサーバのIPアドレスを聞いてくるのでIPアドレスを入力してください

  コンパイル方法
      gcc -O2 -o raspberry_pi_rs-midi-serial_to_wifi_client raspberry_pi_rs-midi-serial_to_wifi_client.c

  36行目の "/dev/ttyUSB0" の部分はご自分のUSB-RS232Cアダプタのデバイスを指定してください。

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
#include <string.h>

#include <signal.h>



#define SERIAL_PORT0 "/dev/ttyUSB0"
#define BAUDRATE0 B38400              /* ボーレートの設定 */

#define PORT 4242


int fd0; /* シリアル通信ファイルディスクリプタ */

struct termios newtio0, oldtio0;     /* シリアル通信設定 */
void sigint_handler(int sig);

char buffer[1];
char destination[32];
int dstSocket;
struct sockaddr_in dstAddr;
int numrcv, rcv_data_size;



int main(int argc, char *argv[]){

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if(!(fd0 = open(SERIAL_PORT0, O_RDWR))) return -1; /* デバイスをオープンする */
    ioctl(fd0, TCGETS, &oldtio0);       /* 現在のシリアルポートの設定を待避 */

    bzero(&newtio0, sizeof(newtio0));
    newtio0 = oldtio0;                  /* ポートの設定をコピー */


    newtio0.c_cflag += CREAD;               /* 受信有効 */
    newtio0.c_cflag += CLOCAL;              /* ローカルライン（モデム制御なし） */
    newtio0.c_cflag += CS8;                 /* データビット:8bit */
    newtio0.c_cflag += 0;                   /* ストップビット:1bit */
    newtio0.c_cflag += 0;                   /* パリティ:None */

    cfsetispeed(&newtio0, BAUDRATE0);
    cfsetospeed(&newtio0, BAUDRATE0);

    cfmakeraw(&newtio0);                    /* RAWモード */

    tcsetattr(fd0, TCSANOW, &newtio0);     /* デバイスに設定を行う */

    ioctl(fd0, TCSETS, &newtio0);       /* ポートの設定を有効にする */




    /* 相手先アドレスの入力と送る文字の入力 */
    printf("サーバーマシンのIPは？:");
    scanf("%s", destination);

    /* sockaddr_in 構造体のセット */
    memset(&dstAddr, 0, sizeof(dstAddr));
    dstAddr.sin_port = htons(PORT);
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_addr.s_addr = inet_addr(destination);

    /* ソケットの生成 */
    dstSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* 接続 */
    if(connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr))){
        printf("%s　に接続できませんでした\n",destination);
        ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
        close(fd0);                         /* デバイスのクローズ */
        return(-1);
    }
    printf("%s に接続しました\n", destination);


    rcv_data_size = 0;
    for (;;) {
        numrcv = recv(dstSocket, buffer, sizeof(buffer), 0);
        rcv_data_size += numrcv;
        if (numrcv == 0 || numrcv == -1) {
            printf("中断しました\n");
            break;
        }
        write(fd0, buffer, numrcv);
    }

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                         /* デバイスのクローズ */

    close(dstSocket);


    return 0;
}

void sigint_handler(int sig) {
    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                         /* デバイスのクローズ */
    close(dstSocket);

    exit(1);
}
