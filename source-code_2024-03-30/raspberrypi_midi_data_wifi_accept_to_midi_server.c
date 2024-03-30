/*
  このプログラムはネットワークに飛ばしたSerialのデータを受けて
  そのデータをMIDIインターフェースに出力するプログラムです

  コンパイル方法
      gcc -O2 -o raspberrypi_midi_data_wifi_accept_to_midi_server raspberrypi_midi_data_wifi_accept_to_midi_server.c  -lpthread

  実行方法
      ./raspberrypi_midi_data_wifi_accept_to_midi_server  midi_device
  実行例
      ./raspberrypi_midi_data_wifi_accept_to_midi_server  /dev/snd/midiC2D0

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

#include <pthread.h>


#define MIDI_PORT0   (argv[1])

#define PORT 4242

void *ClientSession(void *data);

struct param01 {
    int      socket;
    char     *buffer;
    volatile int  execflag;
};

volatile int endflag = 0;

int fd0; /* midiデバイスのファイルディスクリプタ */

void sigint_handler(int sig);

char buffer[1024];
char destination[32];
int dstSocket;
int srcSocket;
struct sockaddr_in dstAddr;
struct sockaddr_in srcAddr;
int dstAddrSize;
int numrcv, rcv_data_size;
int status;
pthread_t pthread01;
struct param01 *p01;

void micro_delay(long micro_sec); 

int main(int argc, char *argv[]){

    if (argc != 2) {
       printf("Usage : %s  midi_device\n", argv[0]); 
       printf("例)     %s  /dev/snd/midiC2D0\n", argv[0]);
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

    dstAddrSize = sizeof(dstAddr);

    /* sockaddr_in 構造体のセット */
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(PORT);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* ソケットの生成 */
    srcSocket = socket(AF_INET, SOCK_STREAM, 0);

    // ソケットのバインド
    bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
    // 接続の許可
    listen(srcSocket, 10);

    while(1){ //ループで回すことによって何度でもクライアントからつなぐことができる

        // 接続の受付け
	printf("ポート:%dで接続を待っています\nクライアントプログラムを動かしてください\n", PORT);
	dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
	printf("%s から接続を受けました socket = %d\n", inet_ntoa(dstAddr.sin_addr), dstSocket);


        p01 = (struct param01 *)malloc(sizeof(struct param01));
        p01->buffer = (char *)malloc(1024);
        p01->socket = dstSocket;
	p01->execflag = 1;

        endflag = 0;
        status = pthread_create(&pthread01, NULL, ClientSession, (void *)p01);    //スレッド作成

        while ((p01->execflag != 0) && (endflag == 0));

        close(dstSocket);

        void *pthread_ret;
        status = pthread_join(pthread01, (void *)pthread_ret);     //指定したスレッドが終了するまで待機
        if (endflag == 1) break;
    }

    close(srcSocket);

    close(fd0);                         /* デバイスのクローズ */

    return(0);
}

void *ClientSession(void *data) {

    struct param01 *p01 = (struct param01 *)data;
    int dstSocket = p01->socket;
    char   *buffer = p01->buffer;
    int    i;
    int status;
    int count, count1;

    count = 0;
    count1 = 0;
    rcv_data_size = 0;
    for (;;) {
        if (endflag != 0) break;
        numrcv = recv(dstSocket, buffer, 1, 0);
        rcv_data_size += numrcv;
        if (numrcv == 0 || numrcv == -1) {
            printf("中断しました\n");
            goto goto_label01;
        }
	count++;
        if (numrcv > 0) {
	    write(fd0, buffer, numrcv);
            if (count >= 100) {
	        printf("%4d : receive 100bytes\n", count1, buffer[0]);
		count = 0;
		count1++;
            }	
	    fflush(stdout);
	    micro_delay(320);
        }
    }
    goto_label01:

    p01->execflag = 0;

    close(dstSocket);


    return 0;
}

void sigint_handler(int sig) {
    endflag = 1;
    close(srcSocket);
    close(dstSocket);
    close(fd0);
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
