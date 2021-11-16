/*
 * High level pseudeocode:
 * Initialize variables, functions for different buzzer tunes, etc.
 * In the loop,
 *  if motion is detected:
 *    Randomly play a tune from a library using a buzzer
 *    Wait 10 secs (assuming the door closes before then
 */

//used for the buzzer
//syntax is: tone(pin number, note name, length of tone)
#include "pitches.h"
#include "themes.h"

//used for wifi
#include <WiFi.h>

#define pirPin 0
#define buzzerPin 0
#define muteButtonPin 0
#define LEDPin 0 


String tune;
String tuneDisplay[] = {"Pirates", "CrazyFrog", "MarioUW", "Titanic"};
int* tuneNotes[] = {Pirates_note, CrazyFrog_note, MarioUW_note, Titanic_note};
int* tuneDurations[] = {Pirates_duration, CrazyFrog_duration, MarioUW_duration, Titanic_duration};
int numberOfTunes = 4;
int tempo = 1000; //increase to play slower
int tempo2 = 1.05; //increase to play slower
float hoursSinceBoot;
int buttonValue;
int oldButtonValue;
int currentTime;
int mutePeriod = 30000;
bool muted = false;




String playRandomTune() {
  int randomNum = random(numberOfTunes);

  for (int thisNote = 0; thisNote < (sizeof(tuneDurations[randomNum])/sizeof(int)); thisNote++) {
    //convert duration into time delay
    int noteDuration = tempo / tuneDurations[randomNum][thisNote]; 
    tone(buzzerPin, tuneNotes[randomNum][thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * tempo2;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }

  return tuneDisplay[randomNum];
}




void setup() {
  Serial.begin(115200);

  //for random number
  randomSeed(analogRead(0));

  
  pinMode(pirPin, INPUT);
  pinMode(muteButtonPin, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);

}







void loop() {
  buttonValue = digitalRead(muteButtonPin);

  //if button is released after being pressed, mute speaker
  if (buttonValue == 1 && oldButtonValue == 0) {
    currentTime = millis();
    muted == true;
  }
  
  //unmute after 30 seconds
  if (millis() > (currentTime + mutePeriod)) {
      muted == false;
  }
  
  //if motion is detected by the PIR sensor
  if (digitalRead(pirPin) == HIGH) {
    //LED turns on
    digitalWrite(LEDPin, HIGH);
    
    //find hours since boot/reset
    hoursSinceBoot = millis() / 3600000.0;

    //if unmuted, play tune and display as such
    if (!muted) {
      tune = playRandomTune();
      Serial.print(tune);
      Serial.print(" was played ");
      Serial.print(String(hoursSinceBoot));
      Serial.println(" hours after boot.");
    }
    //if muted, display the tune that would have played
    else {
      Serial.print(tune);
      Serial.print(" was muted ");
      Serial.print(String(hoursSinceBoot));
      Serial.println(" hours after boot.");
    }

  }

  //if nothing is detected by the motion sensor
  else {
    digitalWrite(LEDPin, LOW);
  }

  
  oldButtonValue = buttonValue;
}
