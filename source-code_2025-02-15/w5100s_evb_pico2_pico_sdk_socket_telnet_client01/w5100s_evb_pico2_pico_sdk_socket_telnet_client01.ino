extern "C" {
  #include "port_common.h"

  #include "wizchip_conf.h"
  #include "w5x00_spi.h"
  #include "socket.h"
}

#include <SPI.h>

/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2) // Send and receive cache size
#define _LOOPBACK_DEBUG_

wiz_NetInfo net_info = {
  .mac = {0x00, 0x08, 0xdc, 0x16, 0xed, 0x11}, // Define MAC variables
  .ip =  {192, 168, 11, 2},                    // Define IP variables
  .sn =  {255, 255, 255, 0},                    // Define subnet variables
  .gw =  {192, 168, 11, 1},                     // Define gateway variables
  .dns = {192, 168, 11, 1},                    // Define DNS  variables
  .dhcp = NETINFO_STATIC};                     // Define the DNCP mode

static uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
  0,
};
static void set_clock_khz(void);


uint8_t socket_number = 0;
void setup() {
  uint8_t dst_addr[] = { 192, 168, 11, 10 };
  int16_t dst_port = 6000;
  int8_t ret01;
  int32_t j;


  Serial.begin(115200);
  
  set_clock_khz();

  wizchip_spi_initialize();
  wizchip_cris_initialize();
  wizchip_reset();
  wizchip_initialize(); // spi initialization
  wizchip_check();

  network_initialize(net_info);

  print_network_information(net_info); // Read back the configuration information and print it



  for (j = 0; j < 20; j++) {
    if ((ret01 = socket(socket_number, Sn_MR_TCP, 0x0, 0x0)) != socket_number) {
      Serial.printf("socket open filed\r\n");
      while(1);
    }
    delay(500);
    ret01 = connect(socket_number, dst_addr, dst_port);
    if (ret01 == SOCK_OK) break;
    Serial.printf("connection failed\r\n");
    disconnect(socket_number);
    close(socket_number);
    delay(1000);
  }
  if (j == 20) {
      while(1);
  }
  Serial.printf("connected\r\n");
}

void loop() {
  int32_t ret02;
  char c;

  if (getSn_SR(socket_number) == SOCK_ESTABLISHED) {
    if (getSn_RX_RSR(socket_number) > 0) {
       ret02 = recv(socket_number, ethernet_buf, 1);
       if (ret02 > 0) {
         c = (char)ethernet_buf[0];
         Serial.print(c);
       }
    }
  }

  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (getSn_SR(socket_number) == SOCK_ESTABLISHED) {
      ethernet_buf[0] = (uint8_t)inChar;
      ret02 = send(socket_number, ethernet_buf, 1);
    }
  }

  if (getSn_SR(socket_number) != SOCK_ESTABLISHED) {
    if (getSn_RX_RSR(socket_number) > 0) {
       ret02 = recv(socket_number, ethernet_buf, 1);
       if (ret02 > 0) {
         c = (char)ethernet_buf[0];
         Serial.print(c);
       }
    } else {
      Serial.printf("\r\n");
      Serial.printf("disconnecting.\r\n");
      disconnect(socket_number);
      close(socket_number);
      // do nothing:
      while (1) {
        sleep_ms(1);
      }
    }
  }
}

static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}
