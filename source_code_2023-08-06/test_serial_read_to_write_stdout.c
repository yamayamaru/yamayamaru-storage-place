/*
    このプログラムはlicheePi4Aのシリアルから入力されたものをループバックしてシリアルに出力するのと
    同時にstdoutに出力するものです
    コンパイル方法
      gcc -O2 -o test_serial_read_to_write_stdout test_serial_read_to_write_stdout.c

    実行方法
          ./test_serial_read_to_write_stdout  serial_device
    実行例
          ./test_serial_read_to_write_stdout  /dev/ttyS1


    このプログラムで生じた損害などは一切保障しません
    自己責任で行ってください
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <signal.h>

#define SERIAL_PORT0 (argv[1])
#define BAUDRATE0 B115200         //ボーレートの設定

int fd0; /* シリアル通信ファイルディスクリプタ */

struct termios newtio0, newtio1, oldtio0, oldtio1;    /* シリアル通信設定 */
void sigint_handler(int sig);

char buf01[1];
int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "Usage : %s  serial_device\n", argv[0]);
        fprintf(stderr, "例) %s  /dev/ttyS1\n", argv[0]);
        return -1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if(!(fd0 = open(SERIAL_PORT0, O_RDWR))) return -1; /* デバイスをオープンする */
    fprintf(stderr, "入力側シリアルポート %s を開きました\n", SERIAL_PORT0);

    ioctl(fd0, TCGETS, &oldtio0);       /* 現在のシリアルポートの設定を待避 */

    bzero(&newtio0, sizeof(newtio0));
    newtio0 = oldtio0;                  /* ポートの設定をコピー */


    newtio0.c_cflag |= CREAD;               // 受信有効
    newtio0.c_cflag |= CLOCAL;              // ローカルライン（モデム制御なし）
    newtio0.c_cflag |= CS8;                 // データビット:8bit
    newtio0.c_cflag |= 0;                   // ストップビット:1bit
    newtio0.c_cflag |= 0;                   // パリティ:None

    cfsetispeed(&newtio0, BAUDRATE0);
    cfsetospeed(&newtio0, BAUDRATE0);

    cfmakeraw(&newtio0);                    // RAWモード

    tcsetattr(fd0, TCSANOW, &newtio0);     // デバイスに設定を行う

    ioctl(fd0, TCSETS, &newtio0);       /* ポートの設定を有効にする */

    ssize_t cnt;
    ssize_t cnt01 = 0;
    for (;;) {
        cnt = read(fd0, buf01, sizeof(buf01));
        if (cnt == 0) break;
        write(fd0, buf01, cnt);
        write(1, buf01, cnt);
        cnt01++;
    }

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                        /* デバイスのクローズ */

    return 0;
}

void sigint_handler(int sig) {
    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                        /* デバイスのクローズ */

    exit(1);
}
