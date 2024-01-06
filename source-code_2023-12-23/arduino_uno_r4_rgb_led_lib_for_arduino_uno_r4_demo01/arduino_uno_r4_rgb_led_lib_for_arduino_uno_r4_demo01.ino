// これらのプログラムの使用に当たってはご自分の責任において使用してください
// これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません


#include "rgb_led_lib_for_arduino_uno_r4.h"


#define NEO_PIXEL_PIN 2                 // Arduino Uno R4でLED制御信号を出力するピンの番号を指定してください
#define PIXEL_N 25                      // NeoPixelのLEDの個数を指定してください


rgb_led_lib_for_arduino_uno_r4 rgb_led01 = rgb_led_lib_for_arduino_uno_r4();

void setup() {
  Serial.begin(115200);
  rgb_led01.initPixelColor(NEO_PIXEL_PIN, PIXEL_N);
  rgb_led01.set_interrupt_disable_mode(1);
    // mode = 0 割り込み禁止を行わない
    // mode = 1 8bit送信中に割り込み禁止にする(8bit送ったら割り込み禁止解除)
    // mode = 2 全RGB LEDにデータを送信中に割り込み禁止にする(割り込み禁止時間が長くなります)

  rgb_led01.setBrightness(5);

  for (int i = 0; i < PIXEL_N; i++) {
    rgb_led01.setPixelColor(i, rgb_led01.rgb_color(0, 0, 255));
  }
  rgb_led01.showPixelColor();
  delay(3000);
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(rgb_led01.rgb_color(255, 0, 0), 50); // Red
  colorWipe(rgb_led01.rgb_color(0, 255, 0), 50); // Green
  colorWipe(rgb_led01.rgb_color(0, 0, 255), 50); // Blue
  //colorWipe(rgb_color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(rgb_led01.rgb_color(127, 127, 127), 50); // White
  theaterChase(rgb_led01.rgb_color(127, 0, 0), 50); // Red
  theaterChase(rgb_led01.rgb_color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<rgb_led01.getPixelNum(); i++) {
    rgb_led01.setPixelColor(i, c);
    rgb_led01.showPixelColor();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<rgb_led01.getPixelNum(); i++) {
      rgb_led01.setPixelColor(i, Wheel((i+j) & 255));
    }
    rgb_led01.showPixelColor();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< rgb_led01.getPixelNum(); i++) {
      rgb_led01.setPixelColor(i, Wheel(((i * 256 / rgb_led01.getPixelNum()) + j) & 255));
    }
    rgb_led01.showPixelColor();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < rgb_led01.getPixelNum(); i=i+3) {
        rgb_led01.setPixelColor(i+q, c);    //turn every third pixel on
      }
      rgb_led01.showPixelColor();

      delay(wait);

      for (uint16_t i=0; i < rgb_led01.getPixelNum(); i=i+3) {
        rgb_led01.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < rgb_led01.getPixelNum(); i=i+3) {
        rgb_led01.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      rgb_led01.showPixelColor();

      delay(wait);

      for (uint16_t i=0; i < rgb_led01.getPixelNum(); i=i+3) {
        rgb_led01.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return rgb_led01.rgb_color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return rgb_led01.rgb_color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return rgb_led01.rgb_color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
