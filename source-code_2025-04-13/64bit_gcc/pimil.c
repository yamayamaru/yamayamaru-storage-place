#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define DCML (0x65640/sizeof(int))              //100万桁         int(1000000 / log10(256) + 55)     (必ず4の倍数にすること)
//#define DCML (0xa240/sizeof(int))               //10万桁
#define DCML (0x1040/sizeof(int))               //1万桁
//#define DCML  (0x1a8/sizeof(int))               //1000桁

struct tm tim1;
struct tm tim2;
struct tm tim3;

unsigned int pi0001[0xd0004/sizeof(int)+1];
unsigned int *pi01 = pi0001 + 0xd0004/sizeof(int);

int r_esi;
unsigned int r_edx, r_ebx, r_ecx;

int carry32(unsigned int a, unsigned int b);
int borrow32(unsigned int a, unsigned int b);
unsigned int mul32_mul_32_to_64(unsigned int a, unsigned int b, unsigned int *r);
unsigned int div64_div_32(unsigned int a, unsigned int b, unsigned int rr, unsigned int *r);

void frst();
void scnd();
void thrd();
int div8();
int div1();
int diva();
int divs();
void trns();
void prnt();
unsigned int cvrt(int i);
void tmdp();

int main(int argc, char *argv[]) {
    time_t tim01;
    *(pi01 + 0) = 0;

    tim01 = time(NULL);
    tim1 = *(localtime((const time_t *)&tim01));

    frst();
    scnd();
    thrd();
    tim01 = time(NULL);
    tim2 = *(localtime((const time_t *)&tim01));

    prnt();
    tim01 = time(NULL);
    tim3 = *(localtime((const time_t *)&tim01));

    tmdp();

    return 0;
}

void frst() {
    r_esi = (int)(0 - DCML);
    pi01[r_esi] = 0xc0;

    r_ecx = 1;
    while (1) {
        if (div8() == 0) break;
        diva();
        r_ecx += 2;
        if (div8() == 0) break;
        divs();
        r_ecx += 2;
    }
}

void scnd() {
    r_esi = (int)(0 - DCML);
    pi01[r_esi] = 0x1c8;

    r_ebx = 0x0cb1;
    r_ecx = 1;
    while (1) {
        if (div1() == 0) break;
        diva();
        r_ecx += 2;
        if (div1() == 0) break;
        divs();
        r_ecx += 2;
    }
}

void thrd() {
    r_esi = (int)(0 - DCML);
    pi01[r_esi] = 0x3bc;

    r_ebx = 0xdf21;
    r_ecx = 1;
    while (1) {
        if (div1() == 0) break;
        diva();
        r_ecx += 2;
        if (div1() == 0) break;
        divs();
        r_ecx += 2;
    }
}

int div8() {
    int ret01;
    unsigned int a;

    ret01 = 0;

    a = pi01[r_esi];
    r_ebx = a << (32-6);
    a = a >> 6;

    pi01[r_esi] = a;
    if (r_esi == -1) {
        if (a == 0) {
            r_esi++;
            if (r_esi == 0) ret01 = 0;
            else            ret01 = 1;
        } else {
            ret01 = 1;
        }
    } else {
        pi01[r_esi+1] = r_ebx;
        if (a == 0) {
            r_esi++;
            if (r_esi == 0) ret01 = 0;
            else            ret01 = 1;
        } else {
            ret01 = 1;
        }
    }

    return ret01;
}

int div1() {
    int i, ret01;

    ret01 = 0;

    i = r_esi;
    r_edx = 0;
    while (1) {
        pi01[i] = div64_div_32(pi01[i], r_ebx, r_edx, &r_edx);
        i++;
        if (i == 0) break;
    }
    if (pi01[r_esi] == 0) {
        r_esi++;
        if (r_esi == 0) ret01 = 0;
        else            ret01 = 1;
    } else {
        ret01 = 1;
    }

    return ret01;
}

int diva() {
    int i, j, ret01;
    unsigned int a, b;

    i = r_esi;

    r_edx = 0;
    while (1) {
        a = pi01[i-DCML];
        b = div64_div_32(pi01[i], r_ecx, r_edx, &r_edx);
        pi01[i-DCML] = a + b;
        if (carry32(a, b) != 0) {
            j = i;
            while (1) {
                j--;
                pi01[j - DCML]++;
                if (pi01[j - DCML] != 0) break;             // pi01[j - DCML]が0以外になるまで続ける(0になるときはキャリーが出てる)
            }
        }
        i++;
        if (i == 0) break;
    }
    return 0;
}

int divs() {
    int i, j;
    unsigned int a, b, c, d;

    i = r_esi;
    r_edx = 0;

    while (1) {
        b = div64_div_32(pi01[i], r_ecx, r_edx, &r_edx);
        a = pi01[i-DCML];
        pi01[i-DCML] = a - b;
        if (borrow32(a, b) != 0) {
            j = i;
            while (1) {
                j--;
                d = 1;
                c = pi01[j - DCML];
                pi01[j - DCML] = c - d;
                if (borrow32(c, d) == 0) break;                     // ボローが出なくなるまで続ける
            }
        }
        i++;
        if (i == 0) break;
    }
    return 0;
}

void trns() {
    int i, j;

    i = -1;
    j = 1;
    j = j - DCML;
    while (1) {
        pi01[j] = pi01[i - DCML];
        i--;
        j++;
        if (j == 0) break;
    }

    putchar((char)((pi01[i - DCML] & 0xff) + 0x30));
    putchar('.');
    printf("\n");
}

void prnt() {
    int j, k, a, c;
    unsigned int i;

    i = 0;
    j = (int)(1 - DCML);

    k = 0;
    trns();
    while (1) {
        a = cvrt(j);
        putchar(a + 0x30);
        c = carry32(i, 0x1a934f09);
        i = i + 0x1a934f09;
        if (c != 0) {
            j++;
            if (j == 0) break;
        }
        if (k % 10 == 9) {
            putchar(' ');
            if (k % 50 == 49) {
                printf("\n");
                if (k % 1000 == 999) {
                    printf("\n");
                }
            }
        }
        k++;
    }
    printf("\n");
}

unsigned int cvrt(int ii) {
    int i = ii;
    unsigned int rr01, r01, a01, a02;

    rr01 = 0;
    while (1) {
        a01 = mul32_mul_32_to_64(pi01[i], 10, &r01);
        a02 = a01 + rr01;
        if (carry32(a01, rr01)) r01++;
        rr01 = r01;
        pi01[i] = a02;
        i++;
        if (i == 0) break;
    }

    return r01;
}

void tmdp() {
    printf("start  %4d/%2d/%2d %2d:%2d:%2d\n", tim1.tm_year + 1900, tim1.tm_mon + 1, tim1.tm_mday,
                      tim1.tm_hour, tim1.tm_min, tim1.tm_sec);
    printf("end_1  %4d/%2d/%2d %2d:%2d:%2d\n", tim2.tm_year + 1900, tim2.tm_mon + 1, tim2.tm_mday,
                      tim2.tm_hour, tim2.tm_min, tim2.tm_sec);
    printf("end_2  %4d/%2d/%2d %2d:%2d:%2d\n", tim3.tm_year + 1900, tim3.tm_mon + 1, tim3.tm_mday,
                      tim3.tm_hour, tim3.tm_min, tim3.tm_sec);
}

int carry32(unsigned int a, unsigned int b) {
    int x, carry;

    carry = 0;
    x = a + b;
    if (x < b) carry = 1;

    return carry;
}


int borrow32(unsigned int a, unsigned int b) {
    int borrow = 0;
    if (a < b) borrow = 1;

    return borrow;
}

// 64bit * 32bit = 32bit    ret = rr:a / b   *r = rr:a % b
unsigned int div64_div_32(unsigned int a, unsigned int b, unsigned int rr, unsigned int *r) {
     unsigned int ret01;
     unsigned long long a64, b64, c64, r01;

     r01 = (unsigned long long)rr;
     a64 = (r01 << 32) | a;
     b64 = b;
     c64 = a64 / b64;
     ret01 = (unsigned int)c64;
     *r = (unsigned int)(a64 - b64 * c64);

     return ret01;
}

// 32bit * 32bit = 64bit    ret = low(a * b)   *r = high(a * b)
unsigned int mul32_mul_32_to_64(unsigned int a, unsigned int b, unsigned int *r) {
     unsigned int ret01;
     unsigned long long a64, b64, c64;

     a64 = a;
     b64 = b;
     c64 = a64 * b64;
     ret01 = (unsigned int)c64;
     *r = (unsigned int)(c64 >> 32);

     return ret01;
}
