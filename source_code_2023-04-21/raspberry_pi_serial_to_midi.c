/*
  このプログラムはシリアルポートからRS-MIDIのデータを受け取って
  そのデータをそのままMIDIインターフェースに出力してMIDI音源を鳴らします

  コンパイル方法
      gcc -O2 -o raspberry_pi_serial_to_midi raspberry_pi_serial_to_midi.c

  実行方法
      ./raspberry_pi_serial_to_midi  serial_device  midi_device
  実行例
      ./raspberry_pi_serial_to_midi  /dev/ttyUSB0  /dev/snd/midiC2D0


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

#define SERIAL_PORT0  (argv[1])
#define BAUDRATE0     B38400         //ボーレートの設定

#define MIDI_PORT0    (argv[2])


int fd0; /* シリアル通信ファイルディスクリプタ */
int fd1; /* MIDIポートファイルディスクリプタ */

struct termios newtio0, oldtio0;    /* シリアル通信設定 */
void sigint_handler(int sig);

char buf01[1];
int main(int argc, char *argv[]){

    if (argc != 3) {
        printf("Usage : %s  serial_device  midi_device\n", argv[0]);
        printf("例) %s  /dev/ttyUSB0  /dev/snd/midiC2D0\n", argv[0]);
        return -1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if(!(fd0 = open(SERIAL_PORT0, O_RDWR))) {    /* デバイスをオープンする */
        fprintf(stderr, "シリアルポートが開けません : %s\n", SERIAL_PORT0);
        return -1;
    }
    fprintf(stderr, "シリアルポート %s を開きました\n", SERIAL_PORT0);
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


    if(!(fd1 = open(MIDI_PORT0, O_WRONLY))) {
        fprintf(stderr, "MIDIデバイスがオープンできません : %s\n", MIDI_PORT0);
        close(fd0);
        return -1; /* デバイスをオープンする */
    }
    fprintf(stderr, "MIDIポート %s を開きました\n", MIDI_PORT0);

    ssize_t cnt = 0;
    ssize_t cnt01 = 0;
    for (;;) {
        cnt = read(fd0, buf01, sizeof(buf01));
        if (cnt == 0) break;
        write(fd1, buf01, cnt);
        cnt01++;
    }

    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                        /* デバイスのクローズ */
    close(fd1);                        /* デバイスのクローズ */

    return 0;
}

void sigint_handler(int sig) {
    ioctl(fd0, TCSETS, &oldtio0);       /* ポートの設定を元に戻す */
    close(fd0);                        /* デバイスのクローズ */
    close(fd1);                        /* デバイスのクローズ */

    exit(1);
}
