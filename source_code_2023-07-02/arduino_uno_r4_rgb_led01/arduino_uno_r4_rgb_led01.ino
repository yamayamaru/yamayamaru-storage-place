/*
    注意：NEO_PIXEL_PINの番号は2固定です（可変にできるようにしたかったのですがタイミングが間に合わなかったです)

    このプログラムの新バージョンがあります。
    新バージョンではNEO_PIXEL_PINに任意の番号を指定できます。
    https://github.com/yamayamaru/yamayamaru-storage-place/tree/main/source_code_2023-07-02-0002/arduino_uno_r4_rgb_led02

*/

#define NEO_PIXEL_PIN 2
#define PIXEL_N 12

uint32_t rgb_color(int r, int g, int b);
void setPixelColor(int pixel_number, uint32_t color);
void showPixelColor();
int initPixelColor(int pin, int pixel_number);
int getPixelNum();
void setup() {
  Serial.begin(115200);

  pinMode(NEO_PIXEL_PIN, OUTPUT);
  set_pin_no(NEO_PIXEL_PIN);
  pin_clear();

  initPixelColor(NEO_PIXEL_PIN, PIXEL_N);
  for (int i = 0; i < PIXEL_N; i++) {
    setPixelColor(i, rgb_color(0, 0, 255));
  }
  showPixelColor();
  delay(3000);
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(rgb_color(255, 0, 0), 50); // Red
  colorWipe(rgb_color(0, 255, 0), 50); // Green
  colorWipe(rgb_color(0, 0, 255), 50); // Blue
//colorWipe(rgb_color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(rgb_color(127, 127, 127), 50); // White
  theaterChase(rgb_color(127, 0, 0), 50); // Red
  theaterChase(rgb_color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<getPixelNum(); i++) {
    setPixelColor(i, c);
    showPixelColor();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<getPixelNum(); i++) {
      setPixelColor(i, Wheel((i+j) & 255));
    }
    showPixelColor();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< getPixelNum(); i++) {
      setPixelColor(i, Wheel(((i * 256 / getPixelNum()) + j) & 255));
    }
    showPixelColor();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < getPixelNum(); i=i+3) {
        setPixelColor(i+q, c);    //turn every third pixel on
      }
      showPixelColor();

      delay(wait);

      for (uint16_t i=0; i < getPixelNum(); i=i+3) {
        setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < getPixelNum(); i=i+3) {
        setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      showPixelColor();

      delay(wait);

      for (uint16_t i=0; i < getPixelNum(); i=i+3) {
        setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return rgb_color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return rgb_color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return rgb_color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t rgb_color(int r, int g, int b) {
  return (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
}

inline uint8_t color_r(uint32_t color) {
  return (uint8_t)((color >> 16) & 0xff);
}

inline uint8_t color_g(uint32_t color) {
  return (uint8_t)((color >> 8) & 0xff);
}

inline uint8_t color_b(uint32_t color) {
  return (uint8_t)(color & 0xff);
}

#define MAX_NEOPIXEL_COUNT  256

uint8_t neoPixels[MAX_NEOPIXEL_COUNT * 3];

int _pixel_num = 0;
int _pixel_pin = 0;
int initPixelColor(int pin, int pixel_number) {
  if (pixel_number > MAX_NEOPIXEL_COUNT) return -1;
  _pixel_num = pixel_number;
  _pixel_pin = pin;

  pinMode(_pixel_pin, OUTPUT);
  memset(neoPixels, 0, MAX_NEOPIXEL_COUNT * 3);
  for (int i = 0; i < _pixel_num; i++) {
    setPixelColor(i, 0);
  }
  showPixelColor();
  delay(200);

  return 0;
}

int getPixelNum(){
  return _pixel_num;
}

void setPixelColor(int pixel_number, uint32_t color) {
  if (pixel_number >= _pixel_num) return;

  neoPixels[pixel_number * 3 + 0] = color_b(color);
  neoPixels[pixel_number * 3 + 1] = color_g(color);
  neoPixels[pixel_number * 3 + 2] = color_r(color);
}

void showPixelColor() {
    uint8_t *p, pix, mask;
    int32_t  flag1, flag2;
    p = neoPixels;
    int32_t  num1, num2, num3, num4;
//    num1 = 21;  // T1H
//    num2 =  8;  // T0H
//    num3 = 15;  // T1L
//    num4 = 21;  // T0L
    num1 =  7;  // T1H
    num2 =  2;  // T0H
    num3 =  7;  // T1L
    num4 =  7;  // T0L

    flag2 = 0;
    while (p < (neoPixels + (_pixel_num * 3))) {
      pix = *p++;
      for (mask = 0x80; mask; mask >>= 1) {
        if (flag2) {
          if (flag1) {
            asm volatile("movs   r0, %[num3]\n"
                       "loop03:\n\t"
                       "sub    r0, #1\n\t"
                       "cmp    r0, #0\n\t"
                       "bne    loop03\n\t"
                       :
                       : [num3]"r"(num3)
                       : "r0"
          );
          } else {
            asm volatile("movs   r0, %[num4]\n"
                       "loop04:\n\t"
                       "sub    r0, #1\n\t"
                       "cmp    r0, #0\n\t"
                       "bne    loop04\n\t"
                       :
                       : [num4]"r"(num4)
                       : "r0"
            );
          }
        }
        flag2 = 1;
//        sio_hw->gpio_set = (1ul << _pixel_pin);
        pin_set();
        flag1 = pix & mask;
        if (flag1) {
          asm volatile("movs   r0, %[num1]\n"
                       "loop01:\n\t"
                       "sub    r0, #1\n\t"
                       "cmp    r0, #0\n\t"
                       "bne    loop01\n\t"
                       :
                       : [num1]"r"(num1)
                       : "r0"
          );
        } else {
/*
          asm volatile("movs   r0, %[num2]\n"
                       "loop02:\n\t"
                       "sub    r0, #1\n\t"
                       "cmp    r0, #0\n\t"
                       "bne    loop02\n\t"
                       :
                       : [num2]"r"(num2)
                       : "r0"
          );
*/
        }
//      sio_hw->gpio_clr = (1ul << _pixel_pin);
        pin_clear();
      }
    }
    if (flag1) {
      asm volatile("movs   r0, %[num3]\n"
                "loop05:\n\t"
                "sub    r0, #1\n\t"
                "cmp    r0, #0\n\t"
                "bne    loop05\n\t"
                :
                : [num3]"r"(num3)
                : "r0"
    );
    } else {
      asm volatile("movs    r0, %[num4]\n"
                "loop06:\n\t"
                "sub    r0, #1\n\t"
                "cmp    r0, #0\n\t"
                "bne    loop06\n\t"
                :
                : [num4]"r"(num4)
                : "r0"
      );
    }
}


R_PORT0_Type *r_port_n[10] = {R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7, R_PORT8, R_PORT9};

void digitalWrite01(uint8_t pin_no, uint8_t value) {
  if (value != (uint8_t)0) {
    r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR = (1 << (g_pin_cfg[pin_no].pin & 0xff)) | (r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR);
  } else {
    r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR = ~((uint32_t)((uint32_t)1 << (g_pin_cfg[pin_no].pin & 0xff))) & (r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR);
  }
}


R_PORT0_Type *r_port_n_number;
uint32_t r_port_n_pin_mask, r_port_n_pin_mask_not;
__IOM uint16_t *r_port_n_podr01;

void set_pin_no(uint8_t pin_no) {
  r_port_n_number = r_port_n[g_pin_cfg[pin_no].pin >> 8];
  r_port_n_pin_mask = 1 << (g_pin_cfg[pin_no].pin & 0xff);
  r_port_n_pin_mask_not = ~r_port_n_pin_mask;
  r_port_n_podr01 = &(r_port_n_number->PODR);
}

inline void pin_set(void) {
//  r_port_n_number->PODR = (1 << r_port_n_pin_mask) | r_port_n_number->PODR;
  r_port_n_number->PODR = 0x0020 | r_port_n_number->PODR;
//  *r_port_n_podr01 = r_port_n_pin_mask | *r_port_n_podr01;
}

inline void pin_clear(void) {
//  r_port_n_number->PODR = ~((uint32_t)((uint32_t)1 << r_port_n_pin_mask)) & r_port_n_number->PODR;
  r_port_n_number->PODR = 0xffffffdf & r_port_n_number->PODR;
//  *r_port_n_podr01 = r_port_n_pin_mask_not & *r_port_n_podr01;
}

// renesas_uno/1.0.1/variants/MINIMA/variant.cpp:
// extern "C" const PinMuxCfg_t g_pin_cfg[] = {
//   { BSP_IO_PORT_03_PIN_01,    P301   }, /* (0) D0  -------------------------  DIGITAL  */
//   { BSP_IO_PORT_03_PIN_02,    P302   }, /* (1) D1  */
//   { BSP_IO_PORT_01_PIN_05,    P105   }, /* (2) D2  */
//   { BSP_IO_PORT_01_PIN_04,    P104   }, /* (3) D3~ */
//   { BSP_IO_PORT_01_PIN_03,    P103   }, /* (4) D4  */
//   { BSP_IO_PORT_01_PIN_02,    P102   }, /* (5) D5~ */
//   { BSP_IO_PORT_01_PIN_06,    P106   }, /* (6) D6~ */
//   { BSP_IO_PORT_01_PIN_07,    P107   }, /* (7) D7  */
//   { BSP_IO_PORT_03_PIN_04,    P304   }, /* (8) D8  */
//   { BSP_IO_PORT_03_PIN_03,    P303   }, /* (9) D9~  */
//   { BSP_IO_PORT_01_PIN_12,    P112   }, /* (10) D10~ */
//   { BSP_IO_PORT_01_PIN_09,    P109   }, /* (11) D11~ */
//   { BSP_IO_PORT_01_PIN_10,    P110   }, /* (12) D12 */
//   { BSP_IO_PORT_01_PIN_11,    P111   }, /* (13) D13 */
//   { BSP_IO_PORT_00_PIN_14,    P014   }, /* (14) A0  --------------------------  ANALOG  */
//   { BSP_IO_PORT_00_PIN_00,    P000   }, /* (15) A1  */
//   { BSP_IO_PORT_00_PIN_01,    P001   }, /* (16) A2  */
//   { BSP_IO_PORT_00_PIN_02,    P002   }, /* (17) A3  */
//   { BSP_IO_PORT_01_PIN_01,    P101   }, /* (18) A4/SDA  */
//   { BSP_IO_PORT_01_PIN_00,    P100   }, /* (19) A5/SCL  */
//   { BSP_IO_PORT_05_PIN_00,    P500   }, /* (20) Analog voltage measure pin  */
// };

// renesas_uno/1.0.1/variants/MINIMA/includes/ra/fsp/src/bsp/mcu/all/bsp_io.h:    typedef enum e_bsp_io_port_pin_t
// {
//     BSP_IO_PORT_00_PIN_00 = 0x0000,    ///< IO port 0 pin 0
//     BSP_IO_PORT_00_PIN_01 = 0x0001,    ///< IO port 0 pin 1
//     BSP_IO_PORT_00_PIN_02 = 0x0002,    ///< IO port 0 pin 2
//     BSP_IO_PORT_00_PIN_03 = 0x0003,    ///< IO port 0 pin 3
//     BSP_IO_PORT_00_PIN_04 = 0x0004,    ///< IO port 0 pin 4
//     BSP_IO_PORT_00_PIN_05 = 0x0005,    ///< IO port 0 pin 5
//     BSP_IO_PORT_00_PIN_06 = 0x0006,    ///< IO port 0 pin 6
// 
//     ************************ 中略 *****************************
// 
//     BSP_IO_PORT_14_PIN_11 = 0x0E0B,    ///< IO port 14 pin 11
//     BSP_IO_PORT_14_PIN_12 = 0x0E0C,    ///< IO port 14 pin 12
//     BSP_IO_PORT_14_PIN_13 = 0x0E0D,    ///< IO port 14 pin 13
//     BSP_IO_PORT_14_PIN_14 = 0x0E0E,    ///< IO port 14 pin 14 
//     BSP_IO_PORT_14_PIN_15 = 0x0E0F,    ///< IO port 14 pin 15
//   } bsp_io_port_pin_t;
// 
// 上位8bitがポート番号、下位8bitがそのポートのピンの番号
