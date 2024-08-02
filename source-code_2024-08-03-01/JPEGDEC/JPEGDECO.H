#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picojpeg.h"
#include "stdint.h"

// #define DEBUG

//------------------------------------------------------------------------------
#ifndef JPEGDecoder_max
#define JPEGDecoder_max(a,b)     ((((unsigned int)a) > ((unsigned int)b)) ? ((unsigned int)a) : ((unsigned int)b))
#endif
#ifndef JPEGDecoder_min
#define JPEGDecoder_min(a,b)     ((((unsigned int)a) < ((unsigned int)b)) ? ((unsigned int)a) : ((unsigned int)b))
#endif
//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;
//------------------------------------------------------------------------------

//public:

    extern unsigned char *JPEGDecoder_pImage;

    extern int JPEGDecoder_width;
    extern int JPEGDecoder_height;
    extern int JPEGDecoder_comps;
    extern int JPEGDecoder_MCUSPerRow;
    extern int JPEGDecoder_MCUSPerCol;
    extern pjpeg_scan_type_t JPEGDecoder_scanType;
    extern int JPEGDecoder_MCUWidth;
    extern int JPEGDecoder_MCUHeight;
    extern int JPEGDecoder_MCUx;
    extern int JPEGDecoder_MCUy;
    extern unsigned int JPEGDecoder_nextjpgpos;
    extern unsigned char JPEGDecoder_mjpegflag;

    void JPEGDecoder_init();
    void JPEGDecoder_close();
    int JPEGDecoder_decode(char* pFilename, unsigned char pReduce);
    int JPEGDecoder_available(void);
    int JPEGDecoder_read(void);

#endif // JPEGDECODER_H
