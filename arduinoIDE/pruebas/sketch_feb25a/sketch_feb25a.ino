#include <Arduino.h>

// pin configuration
#define pinHall 19

// global variables
volatile byte counts = 0;
volatile unsigned long lastTime = 0;
volatile unsigned long currentTime = 0;
volatile unsigned long deltaTime = 0;

// functions
void addCount() {
  counts++;
}

void setup() {
  Serial.begin(9600);
  pinMode(pinHall, INPUT_PULLUP);
  attachInterrupt(pinHall, addCount, FALLING);
}

void loop() {
  currentTime = millis();
  deltaTime = currentTime - lastTime;
  if (deltaTime >= 5000) {
    Serial.println(counts * 0.18);
    counts = 0;
    lastTime = currentTime;
  }
}