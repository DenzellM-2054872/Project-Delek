#include <TFT_eSPI.h>

// #include "AudioManager.h"
#include "WiFi.h"
#include "config.h"
#include <vector>

AdafruitIO_Feed *notiFeed = io.feed("Notification");

std::vector<String> splitMessage(String message){
  String buffer = "";
  std::vector<String> out = std::vector<String>();
  bool in = false;
  while (!message.isEmpty())
  {
    char ch = message[0];
    message.remove(0, 1);
    if(ch = '('){
      in = true;

    }else if(ch == ')'){
      out.push_back(buffer);
      buffer.clear();
      in = false;
    }else if(in){
      buffer.concat(ch);
    }
  }

 return out;
}

void handleNotification(AdafruitIO_Data *data){
  String message = data->toString();

  String buffer = "";
  std::vector<String> splitMsg = std::vector<String>();
  bool in = false;
  while (!message.isEmpty())
  {
    char ch = message[0];
    message.remove(0, 1);
    if(ch = '('){
      in = true;

    }else if(ch == ')'){
      splitMsg.push_back(buffer);
      buffer.clear();
      in = false;
    }else if(in){
      buffer.concat(ch);
    }
  }

  Serial.print("app: ");
  Serial.println(splitMsg[0]);

  Serial.print("title: ");
  Serial.println(splitMsg[1]);

    Serial.print("body: ");
  Serial.println(splitMsg[2]);
}

void setup() {
  // audioSetup();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Serial.println();

  // Serial.println("Connected!");
  // Serial.println(WiFi.localIP());

  io.connect();
  notiFeed->onMessage(handleNotification);
  
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.println(io.statusText());

  
}

void loop(){
  // audioLoop();
  io.run();
}