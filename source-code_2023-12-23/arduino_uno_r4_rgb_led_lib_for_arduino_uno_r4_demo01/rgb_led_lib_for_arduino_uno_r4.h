#include <Arduino.h>

#define MAX_NEOPIXEL_COUNT  256

class rgb_led_lib_for_arduino_uno_r4 {
protected:
  uint8_t neoPixels[MAX_NEOPIXEL_COUNT * 3];
  int _pixel_num = 0;
  int _pixel_pin = 0;


  R_PORT0_Type *r_port_n[10] = {R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7, R_PORT8, R_PORT9};
  void digitalWrite01(uint8_t pin_no, uint8_t value);

  R_PORT0_Type *r_port_n_number;
  uint32_t r_port_n_pin_mask, r_port_n_pin_mask_not;
  __IOM uint16_t *r_port_n_podr01;
  void set_pin_no(uint8_t pin_no);
  inline void pin_set(void);
  inline void pin_clear(void);

  int32_t interrupt_disable_mode_num = 0;
  inline void enable_irq();
  inline void disable_irq();

public:
  rgb_led_lib_for_arduino_uno_r4();
  uint32_t rgb_color(int r, int g, int b);
  inline uint8_t color_r(uint32_t color);
  inline uint8_t color_g(uint32_t color);
  inline uint8_t color_b(uint32_t color);

  void setPixelColor(int pixel_number, uint32_t color);
  void showPixelColor();
  int initPixelColor(int pin, int pixel_number);
  int getPixelNum();

  int32_t get_interrupt_disable_mode();
  void set_interrupt_disable_mode(uint32_t mode);
    // mode = 0 割り込み禁止を行わない
    // mode = 1 8bit送信中に割り込み禁止にする(8bit送ったら割り込み禁止解除)
    // mode = 2 全RGB LEDにデータを送信中に割り込み禁止にする(割り込み禁止時間が長くなります)
  
};

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