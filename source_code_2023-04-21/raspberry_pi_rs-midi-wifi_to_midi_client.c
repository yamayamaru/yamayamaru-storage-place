/*
  このプログラムはサーバプログラムでネットワークに飛ばしたRS-MIDIのデータを受け取って
  MIDIインターフェースにつないだMIDI音源を鳴らします
  起動するとサーバのIPアドレスを聞いてくるのでIPアドレスを入力してください
  サーバ側のプログラムはこちらにあります
      https://github.com/yamayamaru/yamayamaru-storage-place/tree/main/source_code_2023-04-17

      raspberry_pi_rs-midi-serial_to_wifi_server.cがサーバプログラムです


  コンパイル方法
      gcc -O2 -o raspberry_pi_rs-midi-wifi_to_midi_client raspberry_pi_rs-midi-wifi_to_midi_client.c

  実行方法
      ./raspberry_pi_rs-midi-wifi_to_midi_client  midi_device
  実行例
      ./raspberry_pi_rs-midi-wifi_to_midi_client  /dev/snd/midiC2D0

      注意: /dev/snd/midiC2D0の部分はご自分のMIDIインターフェースのデバイスを指定してください
  
  このプログラムで生じた損害などは一切保障しません
  自己責任で行ってください
 */

#include<stdio.h>
#include<stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <signal.h>
#include <time.h>



#define MIDI_PORT0   (argv[1])

#define PORT 4242


int fd0; /* シリアル通信ファイルディスクリプタ */

void sigint_handler(int sig);

char buffer[1];
char destination[32];
int dstSocket;
struct sockaddr_in dstAddr;
int numrcv, rcv_data_size;

void micro_delay(long micro_sec); 

int main(int argc, char *argv[]){

    if (argc != 2) {
       printf("Usage : %s  midi_device\n"); 
       printf("例)     %s  /dev/snd/midiC2D0\n");
       return -1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    if(!(fd0 = open(MIDI_PORT0, O_WRONLY))) { /* デバイスをオープンする */
        printf("MIDIデバイスが開けません : %s\n", MIDI_PORT0);
        return -1; /* デバイスをオープンする */
    }

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
	micro_delay(320);
    }

    close(fd0);                         /* デバイスのクローズ */

    close(dstSocket);


    return 0;
}

void sigint_handler(int sig) {
    close(fd0);                         /* デバイスのクローズ */
    close(dstSocket);

    exit(1);
} 

struct timespec delay_nano_sleep_param01_req, delay_nano_sleep_param01_rem;

void micro_delay(long micro_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req.tv_sec = micro_sec / 1000000;
    delay_nano_sleep_param01_req.tv_nsec = (micro_sec % 1000000) * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req.tv_sec = delay_nano_sleep_param01_rem.tv_sec;
        delay_nano_sleep_param01_req.tv_nsec = delay_nano_sleep_param01_rem.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    }
}
