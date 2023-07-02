/*
    注意：NEO_PIXEL_PINの番号は0から7まで指定できます。8以降はまだ未対応
*/

#define NEO_PIXEL_PIN 2
#define PIXEL_N 12


extern R_PORT0_Type *r_port_n_number;
extern uint32_t r_port_n_pin_mask, r_port_n_pin_mask_not;
extern __IOM uint16_t *r_port_n_podr01;


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

  initPixelColor(NEO_PIXEL_PIN, 12);
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
    int32_t  flag1;
    p = neoPixels;
    int32_t  num1, num2, num3, num4;
//    num1 = 21;  // T1H
//    num2 =  8;  // T0H
//    num3 = 15;  // T1L
//    num4 = 21;  // T0L
    num1 =  7;  // T1H
    num2 =  2;  // T0H
    num3 =  4;  // T1L
    num4 =  4;  // T0L

    while (p < (neoPixels + (_pixel_num * 3))) {
      pix = *p++;
      for (mask = 0x80; mask; mask >>= 1) {
//        sio_hw->gpio_set = (1ul << _pixel_pin);
        flag1 = pix & mask;
/*
        pin_set();
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

//          asm volatile("movs   r0, %[num2]\n"
//                       "loop02:\n\t"
//                       "sub    r0, #1\n\t"
//                       "cmp    r0, #0\n\t"
//                       "bne    loop02\n\t"
//                       :
//                       : [num2]"r"(num2)
//                       : "r0"
//          );

        }
//      sio_hw->gpio_clr = (1ul << _pixel_pin);
        pin_clear();
*/
        asm volatile(
                  "\tmov      r4, %[r_port_n_podr01]\n\t"
                  "  ldrh     r3, [r4]\n\r"
                  "  and      r1, r3, %[r_port_n_pin_mask_not]\n\t"
                  "  orr      r2, r3, %[r_port_n_pin_mask]\n\t"
                  "  strh     r2, [r4]\n\t"
                  "  movs     r0, %[flag1]\n\t"
                  "  bne      jmp_showPixelColor01\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  b        jmp_showPixelColor02\n"
                  "jmp_showPixelColor01:\n\t"
                  "  movs     r0, %[num1]\n"
                  "loop_showPixelColor01:\n\t"
                  "  sub      r0, #1\n\t"
                  "  cmp      r0, #0\n\t"
                  "  bne      loop_showPixelColor01\n"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "  nop\n\t"
                  "jmp_showPixelColor02:\n\t"
                  "  strh     r1, [r4]\n"
                  :
                  :[num1]"r"(num1), [flag1]"r"(flag1), [r_port_n_podr01]"r"(r_port_n_podr01), [r_port_n_pin_mask]"r"(r_port_n_pin_mask), [r_port_n_pin_mask_not]"r"(r_port_n_pin_mask_not)
                  :"r0", "r1", "r2", "r3", "r4"
        );
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
    }
}



/*
C:\Users\username\AppData\Local\Arduino15\packages\arduino\hardware\renesas_uno\1.0.1\variants\MINIMA\includes\ra\fsp\src\bsp\cmsis\Device\RENESAS\Include
0:P301
1:P302
2:P105
3:P104
4:P103
5:P102
6:P106
7:P107
*/
uint8_t pin01[8][2] = {{3, 1}, {3, 2}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 6}, {1, 7}};
R_PORT0_Type *r_port_n[10] = {R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7, R_PORT8, R_PORT9};

void digitalWrite01(uint8_t pin_no, uint8_t value) {
  if (value != (uint8_t)0) {
    r_port_n[pin01[pin_no][0]]->PODR = (1 << pin01[pin_no][1]) | (r_port_n[pin01[pin_no][0]]->PODR);
  } else {
    r_port_n[pin01[pin_no][0]]->PODR = ~((uint32_t)((uint32_t)1 << pin01[pin_no][1])) & r_port_n[pin01[pin_no][0]]->PODR;
  }
}

R_PORT0_Type *r_port_n_number;
uint32_t r_port_n_pin_mask, r_port_n_pin_mask_not;
__IOM uint16_t *r_port_n_podr01;
void set_pin_no(uint8_t pin_no) {
  r_port_n_number = r_port_n[pin01[pin_no][0]];
  // r_port_n_pin_mask = pin01[pin_no][1];
  r_port_n_pin_mask = 1 << (pin01[pin_no][1]);
  r_port_n_pin_mask_not = ~((uint32_t)((uint32_t)1 << (pin01[pin_no][1])));
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

