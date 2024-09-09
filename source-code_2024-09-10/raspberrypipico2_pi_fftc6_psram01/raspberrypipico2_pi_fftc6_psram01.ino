#include <Arduino.h>
#define Serial01 Serial

void main01(void);
extern "C" {
    int main_a (long x);
}

void setup() {
    Serial01.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    int in_char01;

    // シリアルポートより1文字読み込む
    in_char01 = Serial01.read();
 
    if(in_char01 != -1 ){
        // 読み込んだデータが -1 以外の場合　以下の処理を行う
 
        switch(in_char01){
            case 's':

                main01();
                break;
        }
    } else {
      // 読み込むデータが無い場合は何もしない
    }

    delay(100);
}

void main01(void) {
    Serial01.println("Calculating pi");
    main_a(100000);
    Serial01.println();
}
