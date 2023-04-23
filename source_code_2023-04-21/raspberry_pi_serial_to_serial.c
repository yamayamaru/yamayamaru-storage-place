/*
    このプログラムはシリアルからRS-MIDIのデータを受け取って
    そのデータをそのままUSB-RS232Cインターフェースに出力してRS-MIDI対応MIDI音源を鳴らします
    USB-RS232CアダプタをRS-MIDI用ケーブルを使ってRS-MIDI対応のMIDI音源のRS232Cポートに接続します
    MIDI音源のRS232CポートのスイッチはRS232C-2もしくはPC2に設定してください

    コンパイル方法
      gcc -O2 -o raspberry_pi_serial_to_serial raspberry_pi_serial_to_serial.c

    実行方法
          ./raspberry_pi_serial_to_serial  serial_device_in  serial_device_out
    実行例
          ./raspberry_pi_serial_to_serial  /dev/ttyUSB1  /dev/ttyUSB0


    このプログラムで生じた損害などは一切保障しません
    自己責任で行ってください
*/

#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <signal.h>

#define SERIAL_PORT0 (argv[1])
#define BAUDRATE0 B38400         //ボーレートの設定(入力側)
#define SERIAL_PORT1 (argv[2])
#define BAUDRATE1 B38400         //ボーレートの設定(音源側)

int fd0 = -1; /* シリアル通信ファイルディスクリプタ */
int fd1 = -1; /* シリアル通信ファイルディスクリプタ */

struct termios newtio0, newtio1, oldtio0, oldtio1;    /* シリアル通信設定 */
void sigint_handler(int sig);

char buf01[1];
int main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage : %s  serial_device_in  serial_device_out\n", argv[0]);
	fprintf(stderr, "例) %s  /dev/ttyUSB1  /dev/ttyUSB0\n", argv[0]);
	return -1;
    }
         
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if((fd0 = open(SERIAL_PORT0, O_RDONLY)) == -1) {
        fprintf(stderr, "シリアルポートがオープンできません : %s\n", SERIAL_PORT0);
        return -1; /* デバイスをオープンする */
    }
    fprintf(stderr, "入力側シリアルポート %s を開きました\n", SERIAL_PORT0);

    ioctl(fd0, TCGETS, &oldtio0);       /* 現在のシリアルポートの設定を待避 */
 
    bzero(&newtio0, sizeof(newtio0));
    newtio0 = oldtio0;                  /* ポートの設定をコピー */


    newtio0.c_cflag += CREAD;               // 受信有効
    newtio0.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    newtio0.c_cflag += CS8;                 // データビット:8bit
    newtio0.c_cflag += 0;                   // ストップビット:1bit
    newtio0.c_cflag += 0;                   // パリティ:None

    cfsetispeed(&newtio0, BAUDRATE0);
    cfsetospeed(&newtio0, BAUDRATE0);

    cfmakeraw(&newtio0);                    // RAWモード

    tcsetattr(fd0, TCSANOW, &newtio0);     // デバイスに設定を行う

    ioctl(fd0, TCSETS, &newtio0);       /* ポートの設定を有効にする */


    if((fd1 = open(SERIAL_PORT1, O_WRONLY)) == -1) {
        close(fd0);
        fprintf(stderr, "シリアルポートがオープンできません : %s\n", SERIAL_PORT1);
        return -1; /* デバイスをオープンする */
    }
    fprintf(stderr, "音源側シリアルポート %s を開きました\n", SERIAL_PORT1);
    
    ioctl(fd1, TCGETS, &oldtio1);       /* 現在のシリアルポートの設定を待避 */
 
    bzero(&newtio1, sizeof(newtio1));
    newtio1 = oldtio1;                  /* ポートの設定をコピー */

    newtio1.c_cflag += CREAD;               // 受信有効
    newtio1.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    newtio1.c_cflag += CS8;                 // データビット:8bit
    newtio1.c_cflag += 0;                   // ストップビット:1bit
    newtio1.c_cflag += 0;                   // パリティ:None

    cfsetispeed(&newtio1, BAUDRATE1);
    cfsetospeed(&newtio1, BAUDRATE1);

    cfmakeraw(&newtio1);                    // RAWモード

    tcsetattr(fd1, TCSANOW, &newtio1);     // デバイスに設定を行う
 
    ioctl(fd1, TCSETS, &newtio1);       /* ポートの設定を有効にする */

    ssize_t cnt;
    ssize_t cnt01 = 0;
    for (;;) {
        cnt = read(fd0, buf01, sizeof(buf01));
	if (cnt == 0) break;
        write(fd1, buf01, cnt);
	cnt01++;
    }

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                        /* デバイスのクローズ */
    ioctl(fd1, TCSETS, &oldtio1);       /* ポートの設定を元に戻す */
    close(fd1);                        /* デバイスのクローズ */

    return 0;
}

void sigint_handler(int sig) {
    if (fd0 != -1) {
        ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
        close(fd0);                        /* デバイスのクローズ */
    }
    if (fd1 != -1) {
        ioctl(fd1, TCSETS, &oldtio1);       /* ポートの設定を元に戻す */
        close(fd1);                        /* デバイスのクローズ */
    }

    exit(1);
}
