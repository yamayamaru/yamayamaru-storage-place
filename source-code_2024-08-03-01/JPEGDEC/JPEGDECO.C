#include "JPEGDeco.h"

#include <string.h>
extern void draw_string(int x, int y, char *str1);
extern void uint_to_str(char *str1, int a);
char str01[1024];
char str02[256];

extern int second(void);
extern long time_us_64();
extern long millis01();
extern void delay01(int a);


static FILE *JPEGDecoder_g_pInFile;
static pjpeg_scan_type_t JPEGDecoder_scan_type;
static pjpeg_image_info_t JPEGDecoder_image_info;

static int JPEGDecoder_is_available;
static int JPEGDecoder_mcu_x01;
static int JPEGDecoder_mcu_y01;
static unsigned int JPEGDecoder_g_nInFileSize;
static unsigned int JPEGDecoder_g_nInFileOfs;
static unsigned int JPEGDecoder_row_pitch;
static unsigned int JPEGDecoder_decoded_width, JPEGDecoder_decoded_height;
static unsigned int JPEGDecoder_row_blocks_per_mcu, JPEGDecoder_col_blocks_per_mcu;
static unsigned char JPEGDecoder_status;
static unsigned char JPEGDecoder_reduce;
static unsigned char JPEGDecoder_rdtmbuf[1024];
static unsigned int JPEGDecoder_rdtmbufpt;
static unsigned int JPEGDecoder_rdbufttp;

// public:
unsigned char *JPEGDecoder_pImage;
int JPEGDecoder_width;
int JPEGDecoder_height;
int JPEGDecoder_comps;
int JPEGDecoder_MCUSPerRow;
int JPEGDecoder_MCUSPerCol;
pjpeg_scan_type_t JPEGDecoder_scanType;
int JPEGDecoder_MCUWidth;
int JPEGDecoder_MCUHeight;
int JPEGDecoder_MCUx;
int JPEGDecoder_MCUy;
unsigned int JPEGDecoder_nextjpgpos = 0;
unsigned char JPEGDecoder_mjpegflag;



unsigned char JPEGDecoder_pjpeg_callback(unsigned char* pBuf, unsigned int buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
unsigned char JPEGDecoder_pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned int buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
int JPEGDecoder_decode_mcu(void);




void JPEGDecoder_init(){
    JPEGDecoder_mcu_x01 = 0 ;
    JPEGDecoder_mcu_y01 = 0 ;
    JPEGDecoder_is_available = 0;
    JPEGDecoder_reduce = 0;
    JPEGDecoder_pImage = 0;
    JPEGDecoder_mjpegflag = 0;
    JPEGDecoder_nextjpgpos = 0;
//    JPEGDecoder_thisPtr = this;
}


void JPEGDecoder_close(){
    if (JPEGDecoder_pImage != NULL) {
      free(JPEGDecoder_pImage);
      JPEGDecoder_pImage = NULL;
    }
}

unsigned char JPEGDecoder_pjpeg_callback(unsigned char* pBuf, unsigned int buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    JPEGDecoder_pjpeg_need_bytes_callback(pBuf, buf_size, pBytes_actually_read, pCallback_data);

    return (unsigned char)0;
}

unsigned char JPEGDecoder_pjpeg_need_bytes_callback(unsigned char *pBuf, unsigned int buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    unsigned int n;
    unsigned int lstpt;
    unsigned int rrdofs;
    unsigned int rdlen;
//    pCallback_data;


    n = JPEGDecoder_min(JPEGDecoder_g_nInFileSize - JPEGDecoder_g_nInFileOfs - JPEGDecoder_nextjpgpos, buf_size);

    // set next data
    lstpt = JPEGDecoder_g_nInFileOfs + n;
    if(lstpt > (JPEGDecoder_rdbufttp + sizeof(JPEGDecoder_rdtmbuf))){
      rrdofs = JPEGDecoder_rdbufttp + sizeof(JPEGDecoder_rdtmbuf) - JPEGDecoder_g_nInFileOfs;
      memcpy(JPEGDecoder_rdtmbuf, JPEGDecoder_rdtmbuf + sizeof(JPEGDecoder_rdtmbuf) - rrdofs, rrdofs);
      rdlen = ((JPEGDecoder_g_nInFileSize - JPEGDecoder_g_nInFileOfs - JPEGDecoder_nextjpgpos) > (sizeof(JPEGDecoder_rdtmbuf) - rrdofs)) ?
                        sizeof(JPEGDecoder_rdtmbuf) - rrdofs : JPEGDecoder_g_nInFileSize - JPEGDecoder_g_nInFileOfs - JPEGDecoder_nextjpgpos;
      fread(JPEGDecoder_rdtmbuf + rrdofs, 1, rdlen, JPEGDecoder_g_pInFile);
      JPEGDecoder_rdbufttp = JPEGDecoder_g_nInFileOfs;
    }

    memcpy(pBuf, JPEGDecoder_rdtmbuf + JPEGDecoder_g_nInFileOfs - JPEGDecoder_rdbufttp, n);

    *pBytes_actually_read = (unsigned char)(n);
    JPEGDecoder_g_nInFileOfs += n;
    return (unsigned char)0;
}


int JPEGDecoder_decode(char* pFilename, unsigned char pJPEGDecoder_reduce){
    fpos_t fsize = 0;

    JPEGDecoder_mcu_x01 = 0 ;  JPEGDecoder_mcu_y01 = 0 ;

    if(pJPEGDecoder_reduce) JPEGDecoder_reduce = pJPEGDecoder_reduce;

    if(JPEGDecoder_pImage != NULL){
        free(JPEGDecoder_pImage);
        JPEGDecoder_pImage = NULL;
        if (JPEGDecoder_g_pInFile != NULL) {
            fclose(JPEGDecoder_g_pInFile);
            JPEGDecoder_g_pInFile = NULL;
        }
//        delay01(1);
    }


    JPEGDecoder_g_pInFile = fopen(pFilename, "rb");
    if (!JPEGDecoder_g_pInFile) {
//        printf("file open error : %s\n", pFilename);
        return -1;
    }

/*
    fseek(JPEGDecoder_g_pInFile, 0, SEEK_END);
    fgetpos(JPEGDecoder_g_pInFile, &fsize);
    JPEGDecoder_g_nInFileSize = fsize;
    fclose(JPEGDecoder_g_pInFile);
*/

    JPEGDecoder_g_nInFileSize = _filelength(_fileno(JPEGDecoder_g_pInFile));
    fclose(JPEGDecoder_g_pInFile);




    JPEGDecoder_g_pInFile = fopen(pFilename, "rb");
    if (!JPEGDecoder_g_pInFile) {
//    printf("file open error : %s\n", pFilename);
        return -1;
    }

    if(JPEGDecoder_mjpegflag) fseek(JPEGDecoder_g_pInFile, JPEGDecoder_nextjpgpos, SEEK_SET);
    else JPEGDecoder_nextjpgpos = 0;


    JPEGDecoder_g_nInFileOfs = 0;
    JPEGDecoder_rdtmbufpt = 0;
    JPEGDecoder_rdbufttp = 0;
    fread(JPEGDecoder_rdtmbuf, 1, (JPEGDecoder_g_nInFileSize > sizeof(JPEGDecoder_rdtmbuf)) ? sizeof(JPEGDecoder_rdtmbuf) : JPEGDecoder_g_nInFileSize, JPEGDecoder_g_pInFile);

    JPEGDecoder_status = pjpeg_decode_init(&JPEGDecoder_image_info, JPEGDecoder_pjpeg_callback, NULL, (unsigned char)JPEGDecoder_reduce);

    if (JPEGDecoder_status)
    {
        #ifdef DEBUG
//        printf("pjpeg_decode_init() failed with JPEGDecoder_status ");
//        printf("%d\n", (int)JPEGDecoder_status);

        if (JPEGDecoder_status == PJPG_UNSUPPORTED_MODE)\
        {
//            printf("Progressive JPEG files are not supported.\n");
        }
        #endif
        if (JPEGDecoder_g_pInFile != NULL) {
          fclose(JPEGDecoder_g_pInFile);
          JPEGDecoder_g_pInFile = NULL;
        }
        return -1;
    }

    // In JPEGDecoder_reduce mode output 1 pixel per 8x8 block.
    JPEGDecoder_decoded_width = JPEGDecoder_reduce ? (JPEGDecoder_image_info.m_MCUSPerRow * JPEGDecoder_image_info.m_MCUWidth) / 8 : JPEGDecoder_image_info.m_width;
    JPEGDecoder_decoded_height = JPEGDecoder_reduce ? (JPEGDecoder_image_info.m_MCUSPerCol * JPEGDecoder_image_info.m_MCUHeight) / 8 : JPEGDecoder_image_info.m_height;

    JPEGDecoder_row_pitch = JPEGDecoder_image_info.m_MCUWidth * JPEGDecoder_image_info.m_comps;
    JPEGDecoder_pImage = (unsigned char *)malloc(JPEGDecoder_image_info.m_MCUWidth * JPEGDecoder_image_info.m_MCUHeight * JPEGDecoder_image_info.m_comps);
    if (!JPEGDecoder_pImage)
    {
        if (JPEGDecoder_g_pInFile != NULL) {
            fclose(JPEGDecoder_g_pInFile);
            JPEGDecoder_g_pInFile = NULL;
        }
        #ifdef DEBUG
//        printf("Memory Allocation Failure\n");
        #endif
        draw_string(0, 100, "Memory Allocation Failure");
        while(!kbhit());

        return -1;
    }
    memset(JPEGDecoder_pImage , 0 , JPEGDecoder_image_info.m_MCUWidth * JPEGDecoder_image_info.m_MCUHeight * JPEGDecoder_image_info.m_comps);

    JPEGDecoder_row_blocks_per_mcu = JPEGDecoder_image_info.m_MCUWidth >> 3;
    JPEGDecoder_col_blocks_per_mcu = JPEGDecoder_image_info.m_MCUHeight >> 3;

    JPEGDecoder_is_available = 1 ;

    JPEGDecoder_width = JPEGDecoder_decoded_width;
    JPEGDecoder_height = JPEGDecoder_decoded_height;
    JPEGDecoder_comps = JPEGDecoder_image_info.m_comps;
    JPEGDecoder_MCUSPerRow = JPEGDecoder_image_info.m_MCUSPerRow;
    JPEGDecoder_MCUSPerCol = JPEGDecoder_image_info.m_MCUSPerCol;
    JPEGDecoder_scanType = JPEGDecoder_image_info.m_scanType;
    JPEGDecoder_MCUWidth = JPEGDecoder_image_info.m_MCUWidth;
    JPEGDecoder_MCUHeight = JPEGDecoder_image_info.m_MCUHeight;

    return JPEGDecoder_decode_mcu();
}

int JPEGDecoder_decode_mcu(void){

    JPEGDecoder_status = pjpeg_decode_mcu();

    if (JPEGDecoder_status)
    {
        unsigned int npos;
        JPEGDecoder_is_available = 0 ;

        // check next jpeg picture
        npos = JPEGDecoder_nextjpgpos + JPEGDecoder_g_nInFileOfs;
        JPEGDecoder_nextjpgpos = 0;
        if(JPEGDecoder_mjpegflag){
          if((JPEGDecoder_g_nInFileSize - npos) > 64){
            int rlen;
            int i;
            rlen = (npos > 256) ? 256 : npos;
            fseek(JPEGDecoder_g_pInFile, npos - rlen, SEEK_SET);
//            unsigned char rbuf[rlen+64];
            unsigned char rbuf[256+64];       // rlen <= 256
            fread(rbuf, 1, rlen+64, JPEGDecoder_g_pInFile);
            for(i = 0 ; i < rlen + 64 - 2 ; i++){
              if((rbuf[i] == (unsigned char)0xFF) && (rbuf[i+1] == (unsigned char)0xD8) && (rbuf[i+2] == (unsigned char)0xFF)){
                JPEGDecoder_nextjpgpos = npos - rlen + i;
                break;
              }
            }
          }
        }

        if (JPEGDecoder_g_pInFile != NULL) {
            fclose(JPEGDecoder_g_pInFile);
            JPEGDecoder_g_pInFile = NULL;
        }

        if (JPEGDecoder_status != PJPG_NO_MORE_BLOCKS)
        {
            #ifdef DEBUG
//            printf("pjpeg_JPEGDecoder_decode_mcu() failed with JPEGDecoder_status ");
//            printf("%d\n", (int)JPEGDecoder_status);
            #endif
            if(JPEGDecoder_pImage != NULL) {
                free(JPEGDecoder_pImage);
                JPEGDecoder_pImage = NULL;
            }
            return -1;
        }
    }
    return 1;
}


int JPEGDecoder_read(void)
{
    unsigned char *pDst_row = NULL;
    unsigned int src_ofs = 0;
    int y = 0;
    int x = 0;
    int by_limit = 0;
    unsigned char *pSrcR = NULL;
    unsigned char *pSrcG = NULL;
    unsigned char *pSrcB = NULL;
    unsigned char *pDst_block = NULL;
    unsigned char *pDst = NULL;
    int bx_limit = 0;
    int bx = 0;
    int by = 0;

    if(JPEGDecoder_is_available == 0) return 0;

    if (JPEGDecoder_mcu_y01 >= JPEGDecoder_image_info.m_MCUSPerCol)
    {
        if(JPEGDecoder_pImage != NULL) {
            free(JPEGDecoder_pImage);
            JPEGDecoder_pImage = NULL;
        }
        if (JPEGDecoder_g_pInFile != NULL) {
            fclose(JPEGDecoder_g_pInFile);
            JPEGDecoder_g_pInFile = NULL;
        }
        return 0;
    }

    if (JPEGDecoder_reduce && JPEGDecoder_pImage)
    {
        // In JPEGDecoder_reduce mode, only the first pixel of each 8x8 block is valid.
        pDst_row = JPEGDecoder_pImage;
        if (JPEGDecoder_image_info.m_scanType == PJPG_GRAYSCALE)
        {
            *pDst_row = JPEGDecoder_image_info.m_pMCUBufR[0];
        }
        else
        {
            for (y = 0; y < JPEGDecoder_col_blocks_per_mcu; y++)
            {
                src_ofs = (y * 128U);
                for (x = 0; x < JPEGDecoder_row_blocks_per_mcu; x++)
                {
                    pDst_row[0] = JPEGDecoder_image_info.m_pMCUBufR[src_ofs];
                    pDst_row[1] = JPEGDecoder_image_info.m_pMCUBufG[src_ofs];
                    pDst_row[2] = JPEGDecoder_image_info.m_pMCUBufB[src_ofs];
                    pDst_row += 3;
                    src_ofs += 64;
                }

                pDst_row += JPEGDecoder_row_pitch - 3 * JPEGDecoder_row_blocks_per_mcu;
            }
        }
    }
    else if(JPEGDecoder_pImage)
    {
        // Copy MCU's pixel blocks into the destination bitmap.
        pDst_row = JPEGDecoder_pImage;
        for (y = 0; y < JPEGDecoder_image_info.m_MCUHeight; y += 8)
        {
            by_limit = JPEGDecoder_min(8, JPEGDecoder_image_info.m_height - (JPEGDecoder_mcu_y01 * JPEGDecoder_image_info.m_MCUHeight + y));

            for (x = 0; x < JPEGDecoder_image_info.m_MCUWidth; x += 8)
            {
                pDst_block = pDst_row + x * JPEGDecoder_image_info.m_comps;

                // Compute source byte offset of the block in the decoder's MCU buffer.
                src_ofs = (x * 8U) + (y * 16U);
                pSrcR = JPEGDecoder_image_info.m_pMCUBufR + src_ofs;
                pSrcG = JPEGDecoder_image_info.m_pMCUBufG + src_ofs;
                pSrcB = JPEGDecoder_image_info.m_pMCUBufB + src_ofs;

                bx_limit = JPEGDecoder_min(8, JPEGDecoder_image_info.m_width - (JPEGDecoder_mcu_x01 * JPEGDecoder_image_info.m_MCUWidth + x));

                if (JPEGDecoder_image_info.m_scanType == PJPG_GRAYSCALE)
                {
                    for (by = 0; by < by_limit; by++)
                    {
                        pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++) {
                            *pDst = *pSrcR;
                             pDst++;
                             pSrcR++;
                        }

                        pSrcR = pSrcR + (8 - bx_limit);

                        pDst_block = pDst_block + JPEGDecoder_row_pitch;
                    }
                }
                else
                {
                    for (by = 0; by < by_limit; by++)
                    {
                        pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++)
                        {
                            pDst[0] = *pSrcR;
                            pSrcR++;
                            pDst[1] = *pSrcG;
                            pSrcG++;
                            pDst[2] = *pSrcB;
                            pSrcB++;

                            pDst = pDst + 3;
                        }

                        pSrcR = pSrcR + (8 - bx_limit);
                        pSrcG = pSrcG + (8 - bx_limit);
                        pSrcB = pSrcB + (8 - bx_limit);

                        pDst_block = pDst_block + JPEGDecoder_row_pitch;
                    }
                }
            }
            pDst_row = pDst_row + (JPEGDecoder_row_pitch * 8);
        }
    }

    JPEGDecoder_MCUx = JPEGDecoder_mcu_x01;
    JPEGDecoder_MCUy = JPEGDecoder_mcu_y01;

    JPEGDecoder_mcu_x01++;
    if (JPEGDecoder_mcu_x01 == JPEGDecoder_image_info.m_MCUSPerRow)
    {
        JPEGDecoder_mcu_x01 = 0;
        JPEGDecoder_mcu_y01++;
    }

    if(JPEGDecoder_decode_mcu()==-1) JPEGDecoder_is_available = 0 ;

    return 1;
}
