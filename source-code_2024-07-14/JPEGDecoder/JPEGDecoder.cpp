/*
 JPEGDecoder.cpp

 JPEG Decoder for Arduino
 Public domain, Makoto Kurauchi <http://yushakobo.jp>

 June/2018 Modified by Itaru Satoh

*/

#include "JPEGDecoder.h"
#include "picojpeg.h"
#include <dos.h>


#include <string.h>
void draw_string(int x, int y, char *str1);
void uint_to_str(char *str1, int a);
void uint64_to_str(char *str1, long long a);
void hex64_to_str(char *str1, long long a);
char str01[1024];
char str02[256];

void delay01(unsigned int millis1);


JPEGDecoder JpegDec;

JPEGDecoder::JPEGDecoder(){
    mcu_x = 0 ;
    mcu_y = 0 ;
    is_available = 0;
    reduce = 0;
    pImage = 0;
    mjpegflag = 0;
    thisPtr = this;
}


JPEGDecoder::~JPEGDecoder(){
	if (pImage != NULL) {
      free(pImage);
      pImage = NULL;
	}
}

unsigned char JPEGDecoder::pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    JPEGDecoder *thisPtr = JpegDec.thisPtr ;
    thisPtr->pjpeg_need_bytes_callback(pBuf, buf_size, pBytes_actually_read, pCallback_data);

	return 0;
}


unsigned char JPEGDecoder::pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    uint32_t n;

//    pCallback_data;

    n = min(g_nInFileSize - g_nInFileOfs - nextjpgpos, buf_size);

    // set next data
    uint32_t lstpt = g_nInFileOfs + n;
    if(lstpt > (rdbufttp + sizeof(rdtmbuf))){
      uint32_t rrdofs = rdbufttp + sizeof(rdtmbuf) - g_nInFileOfs;
      memcpy(rdtmbuf, rdtmbuf + sizeof(rdtmbuf) - rrdofs, rrdofs);
      uint32_t rdlen = ((g_nInFileSize - g_nInFileOfs - nextjpgpos) > (sizeof(rdtmbuf) - rrdofs)) ?
                        sizeof(rdtmbuf) - rrdofs : g_nInFileSize - g_nInFileOfs - nextjpgpos;
      fread(rdtmbuf + rrdofs, 1, rdlen, g_pInFile);
      rdbufttp = g_nInFileOfs;
    }
    memcpy(pBuf, rdtmbuf + g_nInFileOfs - rdbufttp, n);

    *pBytes_actually_read = (unsigned char)(n);
    g_nInFileOfs += n;
    return 0;
}


int JPEGDecoder::decode(char* pFilename, unsigned char pReduce){
    fpos_t fsize = 0;

    mcu_x = 0 ;  mcu_y = 0 ;

    if(pReduce) reduce = pReduce;

    if(pImage != NULL){
      free(pImage);
      pImage = NULL;
	  if (g_pInFile != NULL) {
          fclose(g_pInFile);
		  g_pInFile = NULL;
	  }
      delay01(1);
    }

/*
    struct stat st;
    if (stat(pFilename, &st) == -1) {
        perror("stat");
        return -1;
    }
    g_nInFileSize = st.st_size;
*/

    g_pInFile = fopen(pFilename, "rb");
    if (!g_pInFile) {
//        printf("file open error : %s\n", pFilename);
        return -1;
    }
    fseek(g_pInFile, 0, SEEK_END);
    fgetpos(g_pInFile, &fsize);
    g_nInFileSize = fsize;
    fclose(g_pInFile);

    g_pInFile = fopen(pFilename, "rb");
    if (!g_pInFile) {
//	printf("file open error : %s\n", pFilename);
        return -1;
    }

//    if(mjpegflag) g_pInFile.seek(nextjpgpos);  // set next jpeg position
    if(mjpegflag) fseek(g_pInFile, nextjpgpos, SEEK_SET);
    else nextjpgpos = 0;


    g_nInFileOfs = 0;
    rdtmbufpt = 0;
    rdbufttp = 0;
    fread(rdtmbuf, 1, (g_nInFileSize > sizeof(rdtmbuf)) ? sizeof(rdtmbuf) : g_nInFileSize, g_pInFile);

    status = pjpeg_decode_init(&image_info, pjpeg_callback, NULL, (unsigned char)reduce);

    if (status)
    {
        #ifdef DEBUG
//        printf("pjpeg_decode_init() failed with status ");
//        printf("%d\n", (int)status);

        if (status == PJPG_UNSUPPORTED_MODE)\
        {
//            printf("Progressive JPEG files are not supported.\n");
        }
        #endif
		if (g_pInFile != NULL) {
          fclose(g_pInFile);
		  g_pInFile = NULL;
		}
        return -1;
    }

    // In reduce mode output 1 pixel per 8x8 block.
    decoded_width = reduce ? (image_info.m_MCUSPerRow * image_info.m_MCUWidth) / 8 : image_info.m_width;
    decoded_height = reduce ? (image_info.m_MCUSPerCol * image_info.m_MCUHeight) / 8 : image_info.m_height;

    row_pitch = image_info.m_MCUWidth * image_info.m_comps;
    pImage = (uint8_t *)malloc(image_info.m_MCUWidth * image_info.m_MCUHeight * image_info.m_comps);
    if (!pImage)
    {
        if (g_pInFile != NULL) {
			fclose(g_pInFile);
			g_pInFile = NULL;
		}
        #ifdef DEBUG
//        printf("Memory Allocation Failure\n");
        #endif

        return -1;
    }
    memset(pImage , 0 , sizeof(pImage));

    row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
    col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

    is_available = 1 ;

    width = decoded_width;
    height = decoded_height;
    comps = image_info.m_comps;
    MCUSPerRow = image_info.m_MCUSPerRow;
    MCUSPerCol = image_info.m_MCUSPerCol;
    scanType = image_info.m_scanType;
    MCUWidth = image_info.m_MCUWidth;
    MCUHeight = image_info.m_MCUHeight;

    return decode_mcu();
}


int JPEGDecoder::decode_mcu(void){

    status = pjpeg_decode_mcu();

    if (status)
    {
        is_available = 0 ;

        // check next jpeg picture
        uint32_t npos = nextjpgpos + g_nInFileOfs;
        nextjpgpos = 0;
        if(mjpegflag){
          if((g_nInFileSize - npos) > 64){
            int rlen = (npos > 256) ? 256 : npos;
            fseek(g_pInFile, npos - rlen, SEEK_SET);
//            uint8_t rbuf[rlen+64];
            uint8_t *rbuf;
            rbuf = (uint8_t *)malloc(rlen+64);
            fread(rbuf, 1, rlen+64, g_pInFile);
            for(int i = 0 ; i < rlen + 64 - 2 ; i++){
              if((rbuf[i] == 0xFF) && (rbuf[i+1] == 0xD8) && (rbuf[i+2] == 0xFF)){
                nextjpgpos = npos - rlen + i;
                break;
              }
            }
            free(rbuf);
          }
        }

        if (g_pInFile != NULL) {
            fclose(g_pInFile);
			g_pInFile = NULL;
		}

        if (status != PJPG_NO_MORE_BLOCKS)
        {
            #ifdef DEBUG
//            printf("pjpeg_decode_mcu() failed with status ");
//            printf("%d\n", (int)status);
            #endif
            if(pImage != NULL) {
				free(pImage);
                pImage = NULL;
			}
            return -1;
        }
    }
    return 1;
}


int JPEGDecoder::read(void)
{
    int y, x;
    uint8 *pDst_row;

    if(is_available == 0) return 0;

    if (mcu_y >= image_info.m_MCUSPerCol)
    {
        if(pImage != NULL) {
			free(pImage);
            pImage = NULL;
		}
		if (g_pInFile != NULL) {
            fclose(g_pInFile);
			g_pInFile = NULL;
		}
        return 0;
    }

    if (reduce && pImage)
    {
        // In reduce mode, only the first pixel of each 8x8 block is valid.
        pDst_row = pImage;
        if (image_info.m_scanType == PJPG_GRAYSCALE)
        {
            *pDst_row = image_info.m_pMCUBufR[0];
        }
        else
        {
            uint y, x;
            for (y = 0; y < col_blocks_per_mcu; y++)
            {
                uint src_ofs = (y * 128U);
                for (x = 0; x < row_blocks_per_mcu; x++)
                {
                    pDst_row[0] = image_info.m_pMCUBufR[src_ofs];
                    pDst_row[1] = image_info.m_pMCUBufG[src_ofs];
                    pDst_row[2] = image_info.m_pMCUBufB[src_ofs];
                    pDst_row += 3;
                    src_ofs += 64;
                }

                pDst_row += row_pitch - 3 * row_blocks_per_mcu;
            }
        }
    }
    else if(pImage)
    {
        // Copy MCU's pixel blocks into the destination bitmap.
        pDst_row = pImage;
        for (y = 0; y < image_info.m_MCUHeight; y += 8)
        {
            const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

            for (x = 0; x < image_info.m_MCUWidth; x += 8)
            {
                uint8 *pDst_block = pDst_row + x * image_info.m_comps;

                // Compute source byte offset of the block in the decoder's MCU buffer.
                uint src_ofs = (x * 8U) + (y * 16U);
                const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
                const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
                const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

                const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

                if (image_info.m_scanType == PJPG_GRAYSCALE)
                {
                    int bx, by;
                    for (by = 0; by < by_limit; by++)
                    {
                        uint8 *pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++)
                            *pDst++ = *pSrcR++;

                        pSrcR += (8 - bx_limit);

                        pDst_block += row_pitch;
                    }
                }
                else
                {
                    int bx, by;
                    for (by = 0; by < by_limit; by++)
                    {
                        uint8 *pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++)
                        {
                            pDst[0] = *pSrcR++;
                            pDst[1] = *pSrcG++;
                            pDst[2] = *pSrcB++;

                            pDst += 3;
                        }

                        pSrcR += (8 - bx_limit);
                        pSrcG += (8 - bx_limit);
                        pSrcB += (8 - bx_limit);

                        pDst_block += row_pitch;
                    }
                }
            }
            pDst_row += (row_pitch * 8);
        }
    }

    MCUx = mcu_x;
    MCUy = mcu_y;

    mcu_x++;
    if (mcu_x == image_info.m_MCUSPerRow)
    {
        mcu_x = 0;
        mcu_y++;
    }

    if(decode_mcu()==-1) is_available = 0 ;

    return 1;
}
