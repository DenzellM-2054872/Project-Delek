#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "BluetoothA2DPSink.h"
#include "BluetoothSerial.h"

#define volumeKnobPin 27
#define abianceStripPin 32
#define playButton 33
#define prevButton 17
#define nextButton 2

int volume;
BluetoothA2DPSink a2dpsink;
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(3, abianceStripPin, NEO_RGB + NEO_KHZ800);
BluetoothSerial SerialBT;
String colour = "";
boolean prevPausePlay = false;
boolean paused = false;
boolean skip = false;
boolean prev = false;

unsigned long skipDebounceTime = 0;
unsigned long prevDebounceTime = 0;
unsigned long debounceDelay = 200;

int readVolume(int pin){
  return map(analogRead(pin), 0, 4095, 0, 100);
}

void setColour(int r, int g, int b)
{
  uint32_t ColorPicked =  ledStrip.Color(g, r, b);
  ledStrip.setPixelColor(0, ColorPicked);
  ledStrip.setPixelColor(1, ColorPicked);
  ledStrip.setPixelColor(2, ColorPicked);
  ledStrip.show();
}

void nextSong(){
  unsigned long debounceTime = millis();
  if(debounceTime - skipDebounceTime > debounceDelay){
    skip = true;
  }
  skipDebounceTime = debounceTime;
}

void prevSong(){
  unsigned long debounceTime = millis();
  if(debounceTime - prevDebounceTime > debounceDelay){
    prev = true;
  }
  prevDebounceTime = debounceTime;
}

void setTextColour()
{
  Serial.println(colour);
  if(colour == "red"){
    setColour(255, 0, 0);
  } else if(colour == "green"){
    setColour(0, 255, 0);
  } else if(colour == "blue"){
    setColour(0, 0, 255);
  } else if(colour == "magenta"){
    setColour(255, 0, 255);
  } else if(colour == "cyan"){
    setColour(0, 255, 255);
  } else if(colour == "yellow"){
    setColour(255, 255, 0);
  } else if(colour == "white"){
    setColour(255, 255, 255);
  } else if(colour == "off"){
    setColour(0, 0, 0);
  }
}



void audioSetup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32Test");
  a2dpsink.start("ESP32Test");
  pinMode(volumeKnobPin, INPUT);
  pinMode(playButton, INPUT);

  pinMode(prevButton, INPUT_PULLDOWN);
  attachInterrupt(prevButton, prevSong, RISING);

  pinMode(nextButton, INPUT_PULLDOWN);
  attachInterrupt(nextButton, nextSong, RISING);

  pinMode(abianceStripPin, OUTPUT);

  volume = readVolume(volumeKnobPin);
  ledStrip.begin();
  ledStrip.setBrightness(15);
  ledStrip.show();
  // Serial.print("looking for device");

  // while (!a2dpsink.is_connected())
  // {
  //   Serial.print(".");
  //   delay(500);
  // }
  
}

void audioLoop() {
  int loopVolume = readVolume(volumeKnobPin);
  boolean pausePlay = analogRead(playButton) < 1000;
  String inputColour = "";

  if(SerialBT.available()){
    inputColour = SerialBT.readString();
    Serial.println(inputColour);
    inputColour.toLowerCase();
    inputColour.trim();
  }

  if(!(inputColour.length() == 0) && !(inputColour == colour)){
    colour = inputColour;
    setTextColour();
  }

  loopVolume = round(loopVolume / 10) * 10;
  if(volume + 10 < loopVolume || loopVolume < volume - 10){
    Serial.print("changing volume to:");
    Serial.println(loopVolume);
    a2dpsink.set_volume(loopVolume);
    volume = loopVolume;
  }

  if(pausePlay && !prevPausePlay){
    if(paused){
        Serial.println("resuming");
        a2dpsink.play();
    }else{
        Serial.println("pausing");
        a2dpsink.pause();
    }
    paused = !paused;

  }else if(skip){
    Serial.println("skipping to the next song");
    a2dpsink.next();
    skip = false;
    if(paused)
      a2dpsink.pause();

  }else if(prev){
    Serial.println("returning to the previous song");
    a2dpsink.previous();
    prev = false;
    if(paused)
      a2dpsink.pause();
  }

  prevPausePlay = pausePlay;
  delay(20);
}

#endif