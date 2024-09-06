#include "RP2350.h"

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  PPB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

inline uint32_t diff_DWT_CYCCNT(uint32_t end_time, uint32_t start_time) {

  return (end_time - start_time);
}

__attribute__((noinline)) void __not_in_flash_func(dummy01)(uint32_t *start, uint32_t *end);
__attribute__((noinline)) void __not_in_flash_func(test01)(uint32_t *start, uint32_t *end);


void loop() {
  uint32_t t1, t2, t3, t4;

  //asm volatile("cpsid   if");

  dummy01(&t3, &t4);
  test01(&t1, &t2);

  //asm volatile("cpsie   if");

  Serial.print("t1 = ");
  Serial.println(t1);
  Serial.print("t2 = ");
  Serial.println(t2);
  Serial.print("dummy_diff(t4, t3) = ");
  Serial.println(diff_DWT_CYCCNT(t4, t3));
  Serial.print("test01_diff(t2, t1) = ");
  Serial.println(diff_DWT_CYCCNT(t2, t1));
  Serial.print("test01_diff(t2, t1) - dummy_diff(t4, t3) = ");
  Serial.println(diff_DWT_CYCCNT(t2, t1) - diff_DWT_CYCCNT(t4, t3));

  delay(5000);
}

__attribute__((noinline)) void __not_in_flash_func(dummy01)(uint32_t *start, uint32_t *end) {
    *start = DWT->CYCCNT; 
    asm volatile (
            "nop\n\t"
            : 
            : 
            :
    );
    *end = DWT->CYCCNT;
}
__attribute__((noinline)) void __not_in_flash_func(test01)(uint32_t *start, uint32_t *end) {
    *start = DWT->CYCCNT; 
    asm volatile (
            "nop\n\t"


            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"

            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"
            "add r0, r0, r1\n\t"

            : 
            : 
            : "r0", "r1", "r2"
  );
  *end = DWT->CYCCNT;
}

