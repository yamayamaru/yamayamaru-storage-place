#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>



// リンカスクリプトから取得する「プログラム本体（変数等）が終わった直後のアドレス」
extern char _end;

// 外部の main 関数と、newlib 内のクリーンアップ関数を宣言
extern int main(int argc, char **argv);
extern void _init_signal(void); // 必要に応じて（空でも可）
                                // j
// リンカスクリプトで定義される BSS 領域（未初期化変数）の範囲
extern char __bss_start;
extern char _end;

// プログラムの真の入り口
void _exit(int a);
void ___startup() {

    // 1. BSS領域（未初期化のグローバル変数）をゼロでクリアする（C言語の基本仕様）
    // これをしないと、初期値なしのグローバル変数がゴミデータだらけになります
    char *bss = (char *)&__bss_start;
    while (bss < &_end) {
        *bss++ = 0;
    }
    // 2. newlib のハードウェア非依存の初期化（必要であれば呼び出す）
    // ※今回は極限までシンプルな環境なので、省略しても malloc 等は動きます
    // 3. あなたの main 関数を呼び出す
    // RUN386から引数（argc, argv）を正しく受け取る処理は複雑なため、
    // まずは引数なし（0, 0）として呼び出します
    int result = main(0, 0);

    // 4. main が終了したら無限ループで安全に停止させる
    //（RUN386環境なら、ここにDOS終了のシステムコール等を書くのが理想です）
    /*
        while (1) {
            __asm__ __volatile__("hlt"); // CPUを停止状態にする
    }
    */

    _exit(result);
}


// 【重要】環境から分かっているヒープの限界アドレス
// 例：RUN386の環境や起動オプションでヒープを「固定10MB」等にしている場合
// ※リンカスクリプトで最大サイズを決めるか、またはマクロでハードコードします。

#define HEAP_MAX_ADDRESS01  ( 10485760 ) // _endから10MBまで

static char *current_heap_end = 0;
char *previous_heap_end;
caddr_t _sbrk_r(struct _reent *reent01, int incr) {

    // 初回呼び出し時に、ヒープの開始点をセット
    if (current_heap_end == 0) {
        current_heap_end = &_end;
    }

    previous_heap_end = current_heap_end;

    // 【ここが重要：固定ヒープの境界チェック】
    // 今回の要求（incr）を満たしたとき、最初から決められた限界を超えないかチェック
    if ((long)(current_heap_end + incr) > (long)(&_end + HEAP_MAX_ADDRESS01)) {
        // もし最初から確保されている枠を超えてしまったら、
        // 「メモリ不足エラー（ENOMEM）」として -1 を返す
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    // 枠内であれば、ポインタを要求サイズ分だけ進める
    current_heap_end += incr;

    // 確保した領域の先頭アドレスを返す
    return (caddr_t) previous_heap_end;
}

caddr_t _sbrk(int incr) {
    volatile unsigned long end01;

    end01 = (unsigned long)&_end;
    end01 = end01 + HEAP_MAX_ADDRESS01;

    // 初回呼び出し時に、ヒープの開始点をセット
    if (current_heap_end == 0) {
        current_heap_end = &_end;
    }

    previous_heap_end = current_heap_end;

    // 【ここが重要：固定ヒープの境界チェック】
    // 今回の要求（incr）を満たしたとき、最初から決められた限界を超えないかチェック
    if ((long)(current_heap_end + incr) > (long)(&_end + HEAP_MAX_ADDRESS01)) {
        // もし最初から確保されている枠を超えてしまったら、
        // 「メモリ不足エラー（ENOMEM）」として -1 を返す
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    // 枠内であれば、ポインタを要求サイズ分だけ進める
    current_heap_end += incr;

    // 確保した領域の先頭アドレスを返す
    return (caddr_t) previous_heap_end;
}

static inline int handle_dos_error(unsigned char cflag, unsigned int eax_val) {
    if (cflag) {
        errno = eax_val; // DOSエラーコードをPOSIX errnoに
        return -1;
    }
    return eax_val;
}


int _open_r(struct _reent *reent, const char *name, int flags, int mode) {
    unsigned int eax_val;
    unsigned char cflag;

    // 1. 新規作成フラグ (O_CREAT) または 上書き切り詰めフラグ (O_TRUNC) がある場合
    if ((flags & O_CREAT) || (flags & O_TRUNC)) {
        // MS-DOS INT 21h / AH=3Ch (Create File) を実行
        // CX = ファイル属性 (0: 通常ファイル)
        unsigned int attr = 0;

        __asm__ __volatile__ (
            "mov $0x3C, %%ah\n\t"  // 機能番号 3Ch (ファイル作成)
            "int $0x21\n\t"
            "setc %1\n\t"
            : "=a" (eax_val), "=qm" (cflag)
            : "d" (name), "c" (attr)
            : "cc", "memory"
        );
    }
    // 2. 既存のファイルを開く場合
    else {
        // DOSのアクセスモードを決定 (0:読込専用, 1:書込専用, 2:読書可能)
        unsigned char dos_mode = 0;
        if ((flags & O_ACCMODE) == O_WRONLY) dos_mode = 1;
        else if ((flags & O_ACCMODE) == O_RDWR)   dos_mode = 2;

        // MS-DOS INT 21h / AH=3Dh (Open File) を実行
        __asm__ __volatile__ (
            "mov $0x3D, %%ah\n\t"  // 機能番号 3Dh (ファイルオープン)
            "int $0x21\n\t"
            "setc %1\n\t"
            : "=a" (eax_val), "=qm" (cflag)
            : "d" (name), "a" (dos_mode)
            : "cc", "memory"
        );
    }

    return handle_dos_error(cflag, eax_val);
}
_ssize_t _write_r(struct _reent *reent, int file, char *ptr, int len) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x40, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file), "c" (len), "d" (ptr)
        : "cc", "memory"
    );

    return handle_dos_error(cflag, eax_val);
}

_ssize_t _read_r(struct _reent *_reent, int file, char *ptr, int len) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x3F, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file), "c" (len), "d" (ptr)
        : "cc", "memory"
    );

    return handle_dos_error(cflag, eax_val);
}

int _close_r(struct _reent *_reent, int file) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x3E, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file)
        : "cc", "memory"
    );

    if (cflag) {
        errno = eax_val;
        return -1;
    }
    return 0;
}

_off_t _lseek_r(struct _reent *_reent, int file, int ptr, int dir) {
    unsigned int eax_val;
    unsigned int edx_val;
    unsigned char cflag;

    // 32bitのptrを 上位16bit(CX) と 下位16bit(DX) に分割
    unsigned int offset_high = (unsigned int)((unsigned long)ptr >> 16);
    unsigned int offset_low  = (unsigned int)(ptr & 0xFFFF);

    __asm__ __volatile__ (
        "mov $0x42, %%ah\n\t"
        "int $0x21\n\t"
        "setc %2\n\t"
        : "=a" (eax_val), "=d" (edx_val), "=qm" (cflag)
        : "b" (file), "a" (dir), "c" (offset_high), "d" (offset_low)
        : "cc", "memory"
    );

    if (cflag) {
        errno = eax_val;
        return -1;
    }

    // 戻り値 DX:AX を結合して32bit値を復元
    return (int)(((edx_val & 0xFFFF) << 16) | (eax_val & 0xFFFF));
}

int _kill_r(struct _reent *_reent, int a, int b) {
    return 0;
}

int _getpid_r(struct _reent *_reent) {
    return 0;
}

int _fstat_r(struct _reent *_reent, int a, struct stat *stat01) {
    return 0;
}

int _isatty_r(struct _reent *_reent, int a) {
    return 1;
}

int _open(const char *name, int flags, int mode) {
    unsigned int eax_val;
    unsigned char cflag;

    // 1. 新規作成フラグ (O_CREAT) または 上書き切り詰めフラグ (O_TRUNC) がある場合
    if ((flags & O_CREAT) || (flags & O_TRUNC)) {
        // MS-DOS INT 21h / AH=3Ch (Create File) を実行
        // CX = ファイル属性 (0: 通常ファイル)
        unsigned int attr = 0;

        __asm__ __volatile__ (
            "mov $0x3C, %%ah\n\t"  // 機能番号 3Ch (ファイル作成)
            "int $0x21\n\t"
            "setc %1\n\t"
            : "=a" (eax_val), "=qm" (cflag)
            : "d" (name), "c" (attr)
            : "cc", "memory"
        );
    }
    // 2. 既存のファイルを開く場合
    else {
        // DOSのアクセスモードを決定 (0:読込専用, 1:書込専用, 2:読書可能)
        unsigned char dos_mode = 0;
        if ((flags & O_ACCMODE) == O_WRONLY) dos_mode = 1;
        else if ((flags & O_ACCMODE) == O_RDWR)   dos_mode = 2;

        // MS-DOS INT 21h / AH=3Dh (Open File) を実行
        __asm__ __volatile__ (
            "mov $0x3D, %%ah\n\t"  // 機能番号 3Dh (ファイルオープン)
            "int $0x21\n\t"
            "setc %1\n\t"
            : "=a" (eax_val), "=qm" (cflag)
            : "d" (name), "a" (dos_mode)
            : "cc", "memory"
        );
    }

    return handle_dos_error(cflag, eax_val);
}
int open (const char *name, int flags, ...) {
    return _open(name, flags, 0);
}

_ssize_t _write(int file, char *ptr, int len) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x40, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file), "c" (len), "d" (ptr)
        : "cc", "memory"
    );

    return handle_dos_error(cflag, eax_val);
}

_ssize_t write(int file, char *ptr, int len) {
    return _write(file, ptr, len);
}

static char dummy01[16];
_ssize_t _flush(int file) {
    unsigned int eax_val;
    unsigned char cflag;
    int len = 0;

    __asm__ __volatile__ (
        "mov $0x40, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file), "c" (len), "d" (dummy01)
        : "cc", "memory"
    );

    return handle_dos_error(cflag, eax_val);
}

_ssize_t _read(int file, char *ptr, int len) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x3F, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file), "c" (len), "d" (ptr)
        : "cc", "memory"
    );

    return handle_dos_error(cflag, eax_val);
}

_ssize_t read(int file, char *ptr, int len) {
    return _read(file, ptr, len);
}

int _close(int file) {
    unsigned int eax_val;
    unsigned char cflag;

    __asm__ __volatile__ (
        "mov $0x3E, %%ah\n\t"
        "int $0x21\n\t"
        "setc %1\n\t"
        : "=a" (eax_val), "=qm" (cflag)
        : "b" (file)
        : "cc", "memory"
    );

    if (cflag) {
        errno = eax_val;
        return -1;
    }
    return 0;
}

int close(int file) {
    return _close(file);
}

_off_t _lseek(int file, int ptr, int dir) {
    unsigned int eax_val;
    unsigned int edx_val;
    unsigned char cflag;

    // 32bitのptrを 上位16bit(CX) と 下位16bit(DX) に分割
    unsigned int offset_high = (unsigned int)((unsigned long)ptr >> 16);
    unsigned int offset_low  = (unsigned int)(ptr & 0xFFFF);

    __asm__ __volatile__ (
        "mov $0x42, %%ah\n\t"
        "int $0x21\n\t"
        "setc %2\n\t"
        : "=a" (eax_val), "=d" (edx_val), "=qm" (cflag)
        : "b" (file), "a" (dir), "c" (offset_high), "d" (offset_low)
        : "cc", "memory"
    );

    if (cflag) {
        errno = eax_val;
        return -1;
    }

    // 戻り値 DX:AX を結合して32bit値を復元
    return (int)(((edx_val & 0xFFFF) << 16) | (eax_val & 0xFFFF));
}

_off_t lseek(int file, int ptr, int dir) {
    return _lseek(file, ptr, dir);
}

int _kill(int a, int b) {
    return 0;
}

int _getpid() {
    return 0;
}

int _fstat(int fd, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd) {
    return 1;
}

void _exit(int a) {
   asm volatile (
        "movb   $0x4c, %%ah\n\r"
        "int    $0x21\r\n"
        :
        :"a"(a)
        :
    );
}


// 簡易的な閏年判定と月ごとの累積日数データ
static const int days_before_month[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

// 年月日から1970年1月1日からの経過日数を計算する関数
static long long date_to_epoch_days(int year, int month, int day) {
    long long days;
    int y = year - 1970;

    // 年による日数の加算（1年=365日）と、1970年以降の閏年の回数を加算
    days = y * 365 + (y + 2) / 4;

    // 今年の指定月より前の日数を加算
    days += days_before_month[month - 1];

    // 当月が閏年かつ3月以降なら1日足す
    if (month > 2 && (year % 4 == 0)) {
        days++;
    }

    // 日数を加算（MS-DOSの日は1始まりなので -1）
    days += (day - 1);

    return days;
}

int _gettimeofday(struct timeval *tv, void *tz);
int gettimeofday(struct timeval *tv, void *tz) {
    return _gettimeofday(tv, tz);
}

int _gettimeofday(struct timeval *tv, void *tz) {
    if (!tv) return 0;

    uint8_t hour, min, sec, hundredth;
    uint16_t year;
    uint8_t month, day;

    unsigned int eax_val;
    unsigned int ecx_val;
    unsigned int edx_val;

    // 1. MS-DOS システムコール: 時刻の取得 (INT 21h, AH=2Ch)
    // 出力: CH=時, CL=分, DH=秒, DL=1/100秒
    eax_val = 0x2c00;
    asm volatile (
        "int     $0x21\n\t"
        :"=c"(ecx_val), "=d"(edx_val)
        :"a"(eax_val)
        : "cc", "memory"
    );
    hour = ecx_val >> 8;
    min  = ecx_val & 0xff;
    sec  = edx_val >> 8;
    hundredth = edx_val & 0xff;

    // 2. MS-DOS システムコール: 日付の取得 (INT 21h, AH=2Ah)
    // 出力: CX=年(1980-2099), DH=月(1-12), DL=日(1-31)
    eax_val = 0x2a00;
    asm volatile (
        "int    $0x21\n\t"
        :"=c"(ecx_val), "=d"(edx_val)
        :"a"(eax_val)
        :"cc", "memory"
    );
    year  = ecx_val;
    month = edx_val >> 8;
    day   = edx_val & 0xff;

    // 3. 取得した日時をUnixエポック秒に変換
    long long epoch_days = date_to_epoch_days((int)year, (int)month, (int)day);
    long long epoch_seconds = (epoch_days * 86400) +
                                  ((long long)hour * 3600) +
                                  ((long long)min * 60) +
                                  (long long)sec;

    // 4. timeval 構造体に格納
    tv->tv_sec = (time_t)epoch_seconds;
    tv->tv_usec = (suseconds_t)hundredth * 10000; // 1/100秒をマイクロ秒に変換

    return 0;
}

__attribute__((noinline)) double second2() {
    uint8_t hour, min, sec, hundredth;
    unsigned int eax_val;
    unsigned int ecx_val;
    unsigned int edx_val;


    eax_val = 0x2c00;
    __asm__ __volatile__ (
        "int     $0x21\n\t"
        :"=c"(ecx_val), "=d"(edx_val)
        :"a"(eax_val)
        : "cc", "memory"
    );
    hour = ecx_val >> 8;
    min  = ecx_val & 0xff;
    sec  = edx_val >> 8;
    hundredth = edx_val & 0xff;


    return ((double)hour * 3600.0 + (double)min * 60.0 + (double)sec + (double)hundredth / 100.0);
}

__attribute__((noinline)) int kbhit01(void) {
    unsigned int data;
    int ret01;
    unsigned int eax_val;

    eax_val = 0x0700;
    asm volatile (
        "int    $0x90\n\t"
        : "=d"(data)
        : "a"(eax_val)
        : "cc", "memory"
    );
    if ((data & 0xff) == 0xff) ret01 = 0;
    else                       ret01 = 1;

    return ret01;
}

__attribute__((noinline)) int kbclr01(void) {
    unsigned int eax_val;

    eax_val = 0x0600;
    asm volatile (
        "int    $0x90\n\t"
        : "=a"(eax_val)
        :
        : "cc", "memory"
    );

    return (eax_val >> 8) & 0xff;
}

__attribute__((noinline)) int getch01(void) {
    int ret01;
    unsigned int eax_value;
    unsigned int edx_value;
    unsigned int ebx_value;

    eax_value = 0x0900;
    __asm__ __volatile__ (
        "int    $0x90\n\t"
        :"=a"(eax_value),"=d"(edx_value),"=b"(ebx_value)
        :
        :
    );

    if ((eax_value & 0xff) == 0) ret01 = edx_value & 0xff;
    else                         ret01 = -1;
    if ((edx_value & 0xff00) == 0xff00) ret01 = -1;

    return ret01;
}

__attribute__((noinline)) double sin01(double x) {
    double __res;
    __asm__ volatile ("fsin" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

__attribute__((noinline)) float sinf01(float x) {
    float __res;
    __asm__ volatile ("fsin" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

__attribute__((noinline)) double cos01(double x) {
    double __res;
    __asm__ volatile ("fcos" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

__attribute__((noinline)) float cosf01(float x) {
    float __res;
    __asm__ volatile ("fcos" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

__attribute__((noinline)) double sqrt01(double x) {
    double __res;
    __asm__ volatile ("fsqrt" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

__attribute__((noinline)) float sqrtf01(float x) {
    float __res;
    __asm__ volatile ("fsqrt" : "=t" (__res) : "0" (x):"cc", "memory");
    return __res;
}

double second(void) {
    uint8_t  hour, min, sec, hundredth;
    uint16_t year;
    uint8_t  month, day;

    // 1. MS-DOS システムコール: 時刻の取得 (INT 21h, AH=2Ch)
    // 出力: CH=時, CL=分, DH=秒, DL=1/100秒

    unsigned int eax_val;
    unsigned int ecx_val;
    unsigned int edx_val;

    eax_val = 0x2c00;
    __asm__ __volatile__ (
        "int     $0x21\n\t"
        :"=c"(ecx_val), "=d"(edx_val)
        :"a"(eax_val)
        : "cc", "memory"
    );
    hour = ecx_val >> 8;
    min  = ecx_val & 0xff;
    sec  = edx_val >> 8;
    hundredth = edx_val & 0xff;


    // 2. MS-DOS システムコール: 日付の取得 (INT 21h, AH=2Ah)
    // 出力: CX=年(1980-2099), DH=月(1-12), DL=日(1-31)

    eax_val = 0x2a00;
    asm volatile (
        "int    $0x21\n\t"
        :"=c"(ecx_val), "=d"(edx_val)
        :"a"(eax_val)
        :"cc", "memory"
    );
    year  = ecx_val;
    month = edx_val >> 8;
    day   = edx_val & 0xff;


    // 3. 取得した日時をUnixエポック秒に変換
    long long epoch_days = date_to_epoch_days((int)year, (int)month, (int)day);
    double        epoch_seconds = ((double)epoch_days * 86400.0) +
                                  ((double)hour * 3600.0) +
                                  ((double)min * 60.0) +
                                  (double)sec + ((double)hundredth / 100.0);
    return epoch_seconds;
}

void _outb(int addr, int data) {
    __asm__ __volatile__ (
        "outb   %%al, %%dx\n\t"
        :
        :"d"(addr), "a"(data)
        :
    );
}

void _outw(int addr, int data) {
    __asm__ __volatile__ (
        "outw   %%ax, %%dx\n\t"
        :
        :"d"(addr), "a"(data)
        :
    );
}

char printf_serial_str01[1024];
void printf_serial(const char* format, ...)
{
    int i;
    va_list ap;
    va_start(ap, format);

    vsnprintf(printf_serial_str01, 1022, format, ap);

//    _write(1, printf_serial_str01, strnlen(printf_serial_str01, 1022));
    for (i = 0; i < strnlen(printf_serial_str01, 1022); i++) {
        _outb(0x0a00, printf_serial_str01[i]);
    }

    va_end(ap);
}
