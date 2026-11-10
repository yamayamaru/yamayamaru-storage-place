#ifndef NEWLIB_STUB_H
#define NEWLIB_STUB_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

caddr_t _sbrk_r(struct _reent *reent01, int incr);
caddr_t _sbrk(int incr);

__attribute__((noinline)) int _open(const char *name, int flags, int mode);
__attribute__((noinline)) _ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t size);
__attribute__((noinline)) _ssize_t _read_r(struct _reent *_reent, int fd, void *buf, size_t size);
__attribute__((noinline)) int _close_r(struct _reent *_reent, int fd);
__attribute__((noinline)) _off_t _lseek_r(struct _reent *_reent, int fd, _off_t offset, int size);
__attribute__((noinline)) int _kill_r(struct _reent *_reent, int a, int b);
__attribute__((noinline)) int _getpid_r(struct _reent *_reent);
__attribute__((noinline)) int _fstat_r(struct _reent *_reent, int a, struct stat *stat01);
__attribute__((noinline)) int _isatty_r(struct _reent *_reent, int a);

__attribute__((noinline)) int _open(const char *name, int flags, int mode);
__attribute__((noinline)) _ssize_t _write(int fd, const void *buf, size_t size);
__attribute__((noinline)) _ssize_t _read(int fd, void *buf, size_t size);
__attribute__((noinline)) _off_t _lseek(int fd, _off_t offset, int dir);
__attribute__((noinline)) int _kill(int a, int b);
__attribute__((noinline)) int _getpid();
__attribute__((noinline)) int _fstat(int fd, struct stat *st);
__attribute__((noinline)) int _isatty(int fd);

__attribute__((noinline)) void _exit(int a);

static unsigned long date_to_epoch_days(int year, int month, int day);
__attribute__((noinline)) double second();
__attribute__((noinline)) double second2();

__attribute__((noinline)) int kbhit01(void);
__attribute__((noinline)) int kbclr01(void); 
__attribute__((noinline)) int getch01(void);

__attribute__((noinline)) void _outb(int addr, int data);
__attribute__((noinline)) void _outw(int addr, int data);
__attribute__((noinline)) void printf_serial(const char* format, ...);

__attribute__((noinline)) double sin01(double x); 
__attribute__((noinline)) float sinf01(float x); 
__attribute__((noinline)) double cos01(double x); 
__attribute__((noinline)) float cosf01(float x); 
__attribute__((noinline)) double sqrt01(double x); 
__attribute__((noinline)) float sqrtf01(float x); 

#endif    //NEWLIB_STUB_H

