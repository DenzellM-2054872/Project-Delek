#include <TFT_eSPI.h>

// #include "AudioManager.h"
#include "WiFi.h"
#include "config.h"
#include <vector>

#define prevButton 17
#define nextButton 2

boolean prevNext = false;
boolean anotherPrevNext = false;
boolean prevPrev = false;
boolean anotherPrevPrev = false;
boolean newMessage = false;

unsigned long nextDebounceTime = 0;
unsigned long prevDebounceTime = 0;
unsigned long debounceDelay = 50;

TFT_eSPI tft = TFT_eSPI();
AdafruitIO_Feed *notiFeed = io.feed("Notification");
std::vector<String> messages = std::vector<String>();
int i = 0;

void handleNotification(AdafruitIO_Data *data){
  String message = data->toString();
  messages.push_back(message);
  Serial.print("notification: ");
  Serial.println(message);
}

std::vector<String> sliceMessage(String message){
  std::vector<String> out = std::vector<String>();
  String buffer = "";
  char ch = 'a';
  boolean in = false;
  while (!message.isEmpty())
  {
    ch = message.charAt(0);
    message.remove(0, 1);

    if(ch == ')'){
      out.push_back(String(buffer));
      buffer.clear();
      in = false;
    }else if(ch == '('){
      in = true;
    }else if(in){
      buffer.concat(ch);
    }
  }
  
  return out;
}


void printEmpty(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("No notifications");
  tft.setCursor(0, 30, 2);
  tft.println("Lol you have no friends loser");
  tft.println("massive L bozo");
  tft.println("hold this ratio");
  tft.println("you've got as many messages as you have maidens");
  Serial.println("printed");
}

void printMessage(int index){
  if(messages.size() == 0 ){
    printEmpty();
    return;
  }
  String message = messages[index];
  std::vector<String> splitMessage = sliceMessage(message);
  String app = splitMessage[0];
  String title = splitMessage[1];
  String body = splitMessage[2];

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(app + ": " + title);
  tft.setCursor(0, 30, 2);
  tft.println(body);
}

void setup() {
  messages.push_back("(Discord) (Beast) (Hello wanna play a game)");
  messages.push_back("(Messages) (Mama) (You are my favorite child)");
  messages.push_back("(Messages) (Papa) (You are a disgrace to this family)");
  // audioSetup();
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);

  pinMode(prevButton, INPUT_PULLDOWN);
  pinMode(nextButton, INPUT_PULLDOWN);

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
  if(newMessage){
    printMessage(i);
  }
    

  boolean readNext = !(boolean) digitalRead(nextButton);
  boolean readPrev = !(boolean) digitalRead(prevButton);

  boolean Next = false;
  boolean prev = false;
  
  if(readNext != prevNext){
    nextDebounceTime = millis();
  }
  if(readPrev != prevPrev){
    prevDebounceTime = millis();
  }

  if((millis() - nextDebounceTime) > debounceDelay){
    if(readNext && !anotherPrevNext){
      Next = true;
    }
    anotherPrevNext = readNext;
  }

  if((millis() - prevDebounceTime) > debounceDelay){
    if(readPrev && !anotherPrevPrev){
      prev = true;
    }
    anotherPrevPrev = readPrev;
  }

  if(messages.size() > 0){
    if(Next){
      ++i;
      i = ( i % messages.size());
      newMessage = true;
    }else if(prev){
      --i;
      i = (i % messages.size());
      newMessage = true;
    }else{
       newMessage = false;
    }
  }else{
    newMessage = false;
  }

  prevPrev = readPrev;
  prevNext = readNext;
  delay(20);
}