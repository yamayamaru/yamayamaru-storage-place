/* Please check the following macros before compiling */

#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifndef FFT_ERROR_MARGIN
#define FFT_ERROR_MARGIN 0.4  /* must be < 0.5 */
#endif


#ifndef dgt_int
#ifdef USE_DGT_LONG_INT
#define dgt_int long long int /* 64 bit int */
#define DGT_INT_MAX LLONG_MAX /* 64 bit int max */
#else
#ifdef USE_DGT_NORMAL_INT
#define dgt_int int           /* 32 bit int */
#define DGT_INT_MAX INT_MAX   /* 32 bit int max */
#else
#define dgt_int short int     /* 16 bit int */
#define DGT_INT_MAX SHRT_MAX  /* 16 bit int max */
#endif
#endif
#endif

#ifndef fft_int
#ifdef USE_FFT_LONG_INT
#define fft_int long long int /* 64 bit int */
#define FFT_INT_MAX LLONG_MAX /* 64 bit int max */
#else
#define fft_int int           /* 32 bit int */
#define FFT_INT_MAX INT_MAX   /* 32 bit int max */
#endif
#endif

#ifndef fft_float
#ifdef USE_FFT_LONG_DOUBLE
#define fft_float long double
#define FFT_FLOAT_EPSILON LDBL_EPSILON
#define xlint long int
#define XLINT_MIN LONG_MIN
#define XLINT_MAX LONG_MAX
#define xlfloat long double
#define XLFLOAT_EPSILON LDBL_EPSILON
#define XLFLOAT_NAN (-LDBL_MAX)
fft_float sqrtxl(fft_float);
fft_float atanxl(fft_float);
fft_float cosxl(fft_float);
fft_float sinxl(fft_float);
#define fft_sqrt(x) sqrtxl(x)
#define fft_atan(x) atanxl(x)
#define fft_cos(x) cosxl(x)
#define fft_sin(x) sinxl(x)
#define FC_HALF ((fft_float) 0.5L)
#define FC_PI_2 ((fft_float) 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314L)
/* -------- WRN=cos(N*pi/20000), WIN=sin(N*pi/20000) -------- */
#define WR5000 ((fft_float) 0.707106781186547524400844362104849039284835937688474036588339868995366239231053519425L)
#define WR2500 ((fft_float) 0.923879532511286756128183189396788286822416625863642486115097731280535007501102358714L)
#define WI2500 ((fft_float) 0.382683432365089771728459984030398866761344562485627041433800635627546033960089692237L)
#define WR1250 ((fft_float) 0.980785280403230449126182236134239036973933730893336095002916088545306513549605063915L)
#define WI1250 ((fft_float) 0.195090322016128267848284868477022240927691617751954807754502089494763318785924580225L)
#define WR3750 ((fft_float) 0.831469612302545237078788377617905756738560811987249963446124590227637920144642343981L)
#define WI3750 ((fft_float) 0.555570233019602224742830813948532874374937190754804045924153528202949247577480068383L)
#else
#define fft_float double
#define FFT_FLOAT_EPSILON DBL_EPSILON
#define fft_sqrt(x) sqrt(x)
#define fft_atan(x) atan(x)
#define fft_cos(x) cos(x)
#define fft_sin(x) sin(x)
#define FC_HALF ((fft_float) 0.5)
#define FC_PI_2 ((fft_float) 1.570796326794896619231321691639751442098584)
#define WR5000 ((fft_float) 0.707106781186547524400844362104849039284835)
#define WR2500 ((fft_float) 0.923879532511286756128183189396788286822416)
#define WI2500 ((fft_float) 0.382683432365089771728459984030398866761344)
#define WR1250 ((fft_float) 0.980785280403230449126182236134239036973933)
#define WI1250 ((fft_float) 0.195090322016128267848284868477022240927691)
#define WR3750 ((fft_float) 0.831469612302545237078788377617905756738560)
#define WI3750 ((fft_float) 0.555570233019602224742830813948532874374937)
#endif
#endif


#ifdef USE_CDFT_PTHREADS
#ifndef USE_CDFT_THREADS
#define USE_CDFT_THREADS
#ifndef CDFT_THREADS_BEGIN_N
#define CDFT_THREADS_BEGIN_N 8192
#endif
#ifndef CDFT_4THREADS_BEGIN_N
#define CDFT_4THREADS_BEGIN_N 65536
#endif
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define cdft_thread_t pthread_t
#define cdft_thread_create(thp,func,argp) { \
    if (pthread_create(thp, NULL, func, (void *) argp) != 0) { \
        fprintf(stderr, "cdft thread error\n"); \
        exit(1); \
    } \
}
#define cdft_thread_wait(th) { \
    if (pthread_join(th, NULL) != 0) { \
        fprintf(stderr, "cdft thread error\n"); \
        exit(1); \
    } \
}
#endif
#endif /* USE_CDFT_PTHREADS */


#ifdef USE_CDFT_WINTHREADS
#ifndef USE_CDFT_THREADS
#define USE_CDFT_THREADS
#ifndef CDFT_THREADS_BEGIN_N
#define CDFT_THREADS_BEGIN_N 32768
#endif
#ifndef CDFT_4THREADS_BEGIN_N
#define CDFT_4THREADS_BEGIN_N 524288
#endif
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define cdft_thread_t HANDLE
#define cdft_thread_create(thp,func,argp) { \
    DWORD thid; \
    *(thp) = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, (LPVOID) argp, 0, &thid); \
    if (*(thp) == 0) { \
        fprintf(stderr, "cdft thread error\n"); \
        exit(1); \
    } \
}
#define cdft_thread_wait(th) { \
    WaitForSingleObject(th, INFINITE); \
    CloseHandle(th); \
}
#endif
#endif /* USE_CDFT_WINTHREADS */


#ifndef CDFT_LOOP_DIV  /* control of the CDFT's speed & tolerance */
#define CDFT_LOOP_DIV 32
#endif

#ifndef RDFT_LOOP_DIV  /* control of the RDFT's speed & tolerance */
#define RDFT_LOOP_DIV 64
#endif

#ifndef DCST_LOOP_DIV  /* control of the DCT,DST's speed & tolerance */
#define DCST_LOOP_DIV 64
#endif


#ifndef F_CMUL_BUF
#define F_CMUL_BUF 512
#endif

#ifndef SWAP_FILE_FFT1
#define SWAP_FILE_FFT1 "tmp1fft.swp"
#endif
#ifndef SWAP_FILE_VAR1
#define SWAP_FILE_VAR1 "tmp1var.swp"
#endif
#ifndef SWAP_FILE_VAR2
#define SWAP_FILE_VAR2 "tmp2var.swp"
#endif
#ifndef SWAP_FILE_TMPA
#define SWAP_FILE_TMPA "tmpavar.swp"
#endif
#ifndef SWAP_FILE_TMPC
#define SWAP_FILE_TMPC "tmpcvar.swp"
#endif

