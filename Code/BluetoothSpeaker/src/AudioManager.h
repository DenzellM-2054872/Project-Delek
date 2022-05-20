#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Arduino.h>
#include "BluetoothA2DPSink.h"

#define volumeKnobPin 27
#define prevButton 17
#define nextButton 2

int volume;
BluetoothA2DPSink a2dpsink;
boolean paused = false;
boolean prevSkip = false;
boolean anotherPrevSkip = false;
boolean prevPrev = false;
boolean anotherPrevPrev = false;

unsigned long skipDebounceTime = 0;
unsigned long prevDebounceTime = 0;
unsigned long debounceDelay = 50;

int readVolume(int pin){
  return map(analogRead(pin), 0, 4095, 0, 100);
}

void audioSetup() {
  // SerialBT.begin("ESP32Test");
  a2dpsink.start("ESP32Test");

  pinMode(volumeKnobPin, INPUT);
  pinMode(prevButton, INPUT_PULLDOWN);
  pinMode(nextButton, INPUT_PULLDOWN);
  volume = readVolume(volumeKnobPin);
}

void audioLoop() {
  int loopVolume = readVolume(volumeKnobPin);
  boolean readSkip = !(boolean) digitalRead(nextButton);
  boolean readPrev = !(boolean) digitalRead(prevButton);

  loopVolume = round(loopVolume / 10) * 10;
  if(volume + 10 < loopVolume || loopVolume < volume - 10){
    Serial.print("changing volume to:");
    Serial.println(loopVolume);
    a2dpsink.set_volume(loopVolume);
    volume = loopVolume;
  }
  boolean skip = false;
  boolean prev = false;
  
  if(readSkip != prevSkip){
    skipDebounceTime = millis();
  }
  if(readPrev != prevPrev){
    prevDebounceTime = millis();
  }

  if((millis() - skipDebounceTime) > debounceDelay){
    if(readSkip && !anotherPrevSkip){
      skip = true;
    }
    anotherPrevSkip = readSkip;
  }

  if((millis() - prevDebounceTime) > debounceDelay){
    if(readPrev && !anotherPrevPrev){
      prev = true;
    }
    anotherPrevPrev = readPrev;
  }


  if(skip && prev){
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
  }else if(prev){
    Serial.println("returning to the previous song");
    a2dpsink.previous();
  }
  prevPrev = readPrev;
  prevSkip = readSkip;
  delay(20);
}

#endif