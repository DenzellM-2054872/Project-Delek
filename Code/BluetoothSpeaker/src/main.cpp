#include <TFT_eSPI.h>

// #include "AudioManager.h"
#include "WiFi.h"
#include "config.h"

AdafruitIO_Feed *btnFeed = io.feed("feed_btn_1");





void setup() {
  // audioSetup();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void loop(){
  // audioLoop();
}