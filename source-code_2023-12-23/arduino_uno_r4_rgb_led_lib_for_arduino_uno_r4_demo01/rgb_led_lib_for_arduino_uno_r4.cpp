#include "rgb_led_lib_for_arduino_uno_r4.h"

rgb_led_lib_for_arduino_uno_r4::rgb_led_lib_for_arduino_uno_r4() {

}

uint32_t rgb_led_lib_for_arduino_uno_r4::rgb_color(int r, int g, int b) {
  return (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
}

inline uint8_t rgb_led_lib_for_arduino_uno_r4::color_r(uint32_t color) {
  return (uint8_t)((color >> 16) & 0xff);
}

inline uint8_t rgb_led_lib_for_arduino_uno_r4::color_g(uint32_t color) {
  return (uint8_t)((color >> 8) & 0xff);
}

inline uint8_t rgb_led_lib_for_arduino_uno_r4::color_b(uint32_t color) {
  return (uint8_t)(color & 0xff);
}


int rgb_led_lib_for_arduino_uno_r4::initPixelColor(int pin, int pixel_number) {
  if (pixel_number > MAX_NEOPIXEL_COUNT) return -1;
  _pixel_num = pixel_number;
  _pixel_pin = pin;

  pinMode(_pixel_pin, OUTPUT);
  set_pin_no(_pixel_pin);
  pin_clear();

  memset(neoPixels, 0, MAX_NEOPIXEL_COUNT * 3);
  for (int i = 0; i < _pixel_num; i++) {
    setPixelColor(i, 0);
  }
  showPixelColor();
  delay(200);
  
  return 0;
}

int rgb_led_lib_for_arduino_uno_r4::getPixelNum(){
  return _pixel_num;
}

void rgb_led_lib_for_arduino_uno_r4::setPixelColor(int pixel_number, uint32_t color) {
  if (pixel_number >= _pixel_num) return;

  neoPixels[pixel_number * 3 + 0] = color_b(color);
  neoPixels[pixel_number * 3 + 1] = color_g(color);
  neoPixels[pixel_number * 3 + 2] = color_r(color);
}

void rgb_led_lib_for_arduino_uno_r4::showPixelColor() {
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

    if (interrupt_disable_mode_num == 2) {
        disable_irq();
    }
    while (p < (neoPixels + (_pixel_num * 3))) {
      pix = *p++;
      if (interrupt_disable_mode_num == 1) {
          disable_irq();
      }
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
      if (interrupt_disable_mode_num == 1) {
          enable_irq();
      }
    }
    if (interrupt_disable_mode_num == 2) {
        enable_irq();
    }
}



void rgb_led_lib_for_arduino_uno_r4::digitalWrite01(uint8_t pin_no, uint8_t value) {
  if (value != (uint8_t)0) {
    r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR = (1 << (g_pin_cfg[pin_no].pin & 0xff)) | (r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR);
  } else {
    r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR = ~((uint32_t)((uint32_t)1 << (g_pin_cfg[pin_no].pin & 0xff))) & (r_port_n[g_pin_cfg[pin_no].pin >> 8]->PODR);
  }
}

void rgb_led_lib_for_arduino_uno_r4::set_pin_no(uint8_t pin_no) {
  r_port_n_number = r_port_n[g_pin_cfg[pin_no].pin >> 8];
  r_port_n_pin_mask = 1 << (g_pin_cfg[pin_no].pin & 0xff);
  r_port_n_pin_mask_not = ~r_port_n_pin_mask;
  r_port_n_podr01 = &(r_port_n_number->PODR);
}

inline void rgb_led_lib_for_arduino_uno_r4::pin_set(void) {
//  r_port_n_number->PODR = (1 << r_port_n_pin_mask) | r_port_n_number->PODR;
  r_port_n_number->PODR = 0x0020 | r_port_n_number->PODR;
//  *r_port_n_podr01 = r_port_n_pin_mask | *r_port_n_podr01;
}

inline void rgb_led_lib_for_arduino_uno_r4::pin_clear(void) {
//  r_port_n_number->PODR = ~((uint32_t)((uint32_t)1 << r_port_n_pin_mask)) & r_port_n_number->PODR;
  r_port_n_number->PODR = 0xffffffdf & r_port_n_number->PODR;
//  *r_port_n_podr01 = r_port_n_pin_mask_not & *r_port_n_podr01;
}


inline void rgb_led_lib_for_arduino_uno_r4::enable_irq() {
    asm volatile("CPSIE i":::"memory");
}

inline void rgb_led_lib_for_arduino_uno_r4::disable_irq() {
    asm volatile("CPSID i":::"memory");
}

void rgb_led_lib_for_arduino_uno_r4::set_interrupt_disable_mode(uint32_t mode) {
// mode = 0 割り込み禁止を行わない
// mode = 1 8bit送信中に割り込み禁止にする(8bit送ったら割り込み禁止解除)
// mode = 2 全RGB LEDにデータを送信中に割り込み禁止にする(割り込み禁止時間が長くなります)

    interrupt_disable_mode_num = mode;
}

int32_t rgb_led_lib_for_arduino_uno_r4::get_interrupt_disable_mode() {
    return interrupt_disable_mode_num;
}
