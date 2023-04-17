// ArduinoはEarle F.PhilhowerさんのArduinoを使用してください
// このプログラムはPico WのUSBポーとのシリアルもしくはUARTから受けた
// RS-MIDIのデータをWiFiでTCP/IPで転送するプログラムです
// TCP/IPネットワークからLinuxマシンでデータを受け取ってRS232Cアダプターから
// RS-MIDI対応のMIDI音源につないでデータを流すことでRS-MIDI対応音源を鳴らすことができます
//
// WindowsのPCではHairless MIDI<->Serial Bridgeというフリーウェアを使うことで
// MIDIからSerialのデータに変換できます
//
// 25行目のSerialをSerial1などに変更すればPico WのUARTから受けたデータを転送できます
//
// あくまで実験です、途中でデータが止まった後に一気にデータが流れることがあります。
//
// このプログラムで生じた損害などは一切保障しません
// 自己責任で行ってください

#include <WiFi.h>

#ifndef STASSID
#define STASSID "e392bb-e76034-2"
#define STAPSK "c^B}q7QwboFicqIy|Oaw.=2tBgX-8__fV%dm]Ghu8Q]rB_7QwboFicqIy|Oaw@9"
#endif


#define SERIAL0 Serial          // USBポートのシリアルからの入力
//#define SERIAL0 Serial1         // UART0からの入力


const char* ssid = STASSID;
const char* password = STAPSK;

int port = 4242;

WiFiServer server(port);

WiFiClient client;
unsigned long time01 = 0;
unsigned long time02 = 0;

uint8_t c;

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("PicoW2");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  }

  server.begin();


  SERIAL0.begin(38400);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  static int count = 0;

  if (!client) {
    if ((millis() - time01) > 3000) {
      client = server.available();
      if (!client) {
        time01 = millis();      
      }
    }
  }

  if (SERIAL0.available() > 0) {
    c = SERIAL0.read();
    if(client) {
      client.write(c);
    }
    count++;
    if ((count & 0x3f) == 0) digitalWrite(LED_BUILTIN, HIGH);
    if ((count & 0x3f) == 0x20) digitalWrite(LED_BUILTIN, LOW); 
  }

}
