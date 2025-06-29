#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct tm time_a01;
struct tm time_a02;
struct tm time_a03;

extern void main01(void);

void putchar1(int c);
void gtm11(void);
void gtm21(void);
void gtm31(void);
void tmdp1(void);

int main(int argc, char *argv[]) {

    main01();

    return 0;
}

void gtm(struct tm *ltime01) {
    time_t time01;
    time01 = time(NULL);
    localtime_s(ltime01, &time01);
}

void gtm11(void) {
    gtm(&time_a01);
}

void gtm21(void) {
    gtm(&time_a02);
}

void gtm31(void) {
    gtm(&time_a03);
}

void tmdp11(struct tm *time01) {
    printf("%d/%02d/%02d %02d:%02d:%02d\n", 
          time01->tm_year+1900, time01->tm_mon+1, time01->tm_mday, 
          time01->tm_hour, time01->tm_min, time01->tm_sec);
}

void tmdp1(void) {
    printf("start  ");
    tmdp11(&time_a01);
    printf("end_1  ");
    tmdp11(&time_a02);
    printf("end_2  ");
    tmdp11(&time_a03);
}

void putchar1(int c) {
    putchar(c);
}
