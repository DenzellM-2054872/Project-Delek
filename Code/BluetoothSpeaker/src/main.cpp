#include <Arduino.h>
#define volumeKnobPin 25
#define prevButton 21
#define nextButton 22

int volume;
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

void setup() {
  Serial.begin(115200);
  pinMode(volumeKnobPin, INPUT);
  pinMode(prevButton, INPUT_PULLDOWN);
  pinMode(nextButton, INPUT_PULLDOWN);
  volume = readVolume(volumeKnobPin);
}

void loop() {
  int loopVolume = readVolume(volumeKnobPin);
  boolean readSkip = !(boolean) digitalRead(nextButton);
  boolean readPrev = !(boolean) digitalRead(prevButton);

  if(volume + 2 < loopVolume || volume - 2 > loopVolume){
    Serial.print("changing volume to:");
    Serial.println(loopVolume);
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
    }else{
        Serial.println("pausing");
    }
    paused = !paused;
  }else if(skip){
    Serial.println("skipping to the next song");
  }else if(prev){
    Serial.println("returning to the previous song");
  }
  prevPrev = readPrev;
  prevSkip = readSkip;
}