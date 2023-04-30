#ifndef _WRITE_8BIT_PARALLEL
#if defined(__IMXRT1062__) && (defined(ARDUINO_TEENSY40) | defined(ARDUINO_TEENSY41))

#define _WRITE_8BIT_PARALLEL

#pragma once
#include <core_pins.h>


extern "C" {
  inline void write_8bit_parallel(uint8_t c);
  inline void set_8bit_parallel(uint8_t c);
}


#if TFT_D0 == 19
/*
// bit0  19
// bit1  18
// bit2  17
// bit3  16
// bit4  22
// bit5  23
// bit6  20
// bit7  21

#define CORE_PIN19_BIT    16
#define CORE_PIN18_BIT    17
#define CORE_PIN17_BIT    22
#define CORE_PIN16_BIT    23
#define CORE_PIN22_BIT    24
#define CORE_PIN23_BIT    25
#define CORE_PIN20_BIT    26
#define CORE_PIN21_BIT    27


// 1111 0000 0011 1100 1111 1111 1111 1111
// f03cffff


#define CORE_PIN16_PORTREG  GPIO6_DR
#define CORE_PIN17_PORTREG  GPIO6_DR
#define CORE_PIN18_PORTREG  GPIO6_DR
#define CORE_PIN19_PORTREG  GPIO6_DR
#define CORE_PIN20_PORTREG  GPIO6_DR
#define CORE_PIN21_PORTREG  GPIO6_DR
#define CORE_PIN22_PORTREG  GPIO6_DR
#define CORE_PIN23_PORTREG  GPIO6_DR
*/

inline void write_8bit_parallel(uint8_t c) {
  uint32_t a;
  a = (CORE_PIN16_PORTREG & 0xf03cffff);
  CORE_PIN16_PORTREG = (a | ((uint32_t)c & 0xfc) << (22 - 2) | ((uint32_t)c & 0x03) << 16);
  digitalWriteFast(TFT_WR, LOW);
  // asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  digitalWriteFast(TFT_WR, HIGH);
}

void setup_8bit_parallel() {
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
}

#elif TFT_D0 == 2
/*
// bit0   2
// bit1   3
// bit2   4
// bit3   5
// bit4   6
// bit5   9
// bit6   8
// bit7   7

#define CORE_PIN2_BIT   4               // レジスタのbitの位置
#define CORE_PIN3_BIT   5
#define CORE_PIN4_BIT   6
#define CORE_PIN5_BIT   8
#define CORE_PIN6_BIT   10
#define CORE_PIN9_BIT   11
#define CORE_PIN8_BIT   16
#define CORE_PIN7_BIT   17

// 1111 1110 1000 1111
// fffffe8f
// 1111 1111 1111 1100 1111 0011 1111 1111
// fffcf3ff


#define CORE_PIN2_PORTREG GPIO9_DR
#define CORE_PIN3_PORTREG GPIO9_DR
#define CORE_PIN4_PORTREG GPIO9_DR
#define CORE_PIN5_PORTREG GPIO9_DR
#define CORE_PIN6_PORTREG GPIO7_DR
#define CORE_PIN7_PORTREG GPIO7_DR
#define CORE_PIN8_PORTREG GPIO7_DR
#define CORE_PIN9_PORTREG GPIO7_DR
*/
inline void write_8bit_parallel(uint8_t c) {
  uint32_t a1, a2, b1, b2;
  a1 = (CORE_PIN2_PORTREG & 0xfffffe8f);
  a2 = (CORE_PIN6_PORTREG & 0xfffcf3ff);
  b1 = a1 | ((uint32_t)c & 0x03) << 4 | (((uint32_t)c & 0x04) | (((uint32_t)c & 0x8) << 1)) << (6 - 2);
  b2 = a2 | ((uint32_t)c & 0x30) << (10 - 4) | ((uint32_t)c & 0xc0) << (16 - 6);
  CORE_PIN2_PORTREG = b1;
  CORE_PIN6_PORTREG = b2;
  digitalWriteFast(TFT_WR, LOW);
//  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  asm volatile(" \tnop \n \tnop \n \tnop\n");
  digitalWriteFast(TFT_WR, HIGH);
}

void setup_8bit_parallel() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
}
#endif

#endif  // defined(__IMXRT1062__) && (defined(ARDUINO_TEENSY40) | defined(ARDUINO_TEENSY41))

#endif  //_WRITE_8BIT_PARALLEL
