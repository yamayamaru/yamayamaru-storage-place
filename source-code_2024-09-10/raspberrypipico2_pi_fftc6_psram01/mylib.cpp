#include <Arduino.h>
#define Serial01 Serial

extern "C" {
    void printf1(const char* format, ...);
    void putchar1(char a);
    double second();
    void *pmalloc1(size_t size);
}
char str_buff01[1024];
void printf1(const char* format, ...)
{

    va_list ap;
    va_start(ap, format);

//    vsprintf(str_buff01, format, ap);
    vsnprintf(str_buff01, 1024, format, ap);

    va_end(ap);

    Serial01.print(str_buff01);

}

int putchar1_count01 = 0;
char putchar1_buf[2];
void putchar1(char a) {
    putchar1_buf[0] = a;
    putchar1_buf[1] = '\0';
    Serial.print(putchar1_buf);
    putchar1_count01++;
    if (putchar1_count01 > 80) {
        putchar1_count01 = 0;
        Serial.print("\r\n");      
    }
}

double second() {
  return ((double)millis() / 1000.0) ;
}

void *pmalloc1(size_t size) {
  return pmalloc(size);
}