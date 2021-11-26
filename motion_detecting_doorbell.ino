/*
 * High level pseudeocode:
 * Initialize variables, functions for different buzzer tunes, etc.
 * In the loop,
 *  if motion is detected:
 *    Randomly play a tune from a library using a buzzer
 *    Wait 10 secs (assuming the door closes before then
 */

//includes definitions for certain pitches
#include "pitches.h"
//includes the 4 themes and the "Song" structure
#include "themes.h"

//pin layout
const int pirPin = 23;
const int buzzerPin = 33;
const int muteButtonPin = 22;
const int LEDPin = 15;
const int highPin = 0;

const Song songChoices[] = {pirates, crazyFrog, marioUW, titanic};
const int numOfSongChoices = 4;
const int mutePeriod = 30000;
bool muted = false;

////////////////////////////////////////////////////////////////////
/*
 * Credit goes to Phil Schatzmann for the ESP32 tone function
 * https://www.pschatzmann.ch/home/2021/01/07/esp32-tone/
 */
hw_timer_t* timer = NULL;
bool value = true;
void IRAM_ATTR onTimer() {
  value = !value;
  digitalWrite(buzzerPin, value);
}
void setupTimer() {
  // Use 1st timer of 4  - 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up
  timer = timerBegin(0, 80, true);//div 80
  timerAttachInterrupt(timer, &onTimer, true);
}
void setFrequency(long frequencyHz) {
  if (frequencyHz == 0) {
    timerAlarmDisable(timer);
  }
  else {
    timerAlarmDisable(timer);
    timerAlarmWrite(timer, 1000000l / frequencyHz, true);
    timerAlarmEnable(timer);
  }
}
void tone(long frequencyHz, long durationMs) {
  setFrequency(frequencyHz);
  delay(durationMs);
  setFrequency(0);
}
////////////////////////////////////////////////////////////////////



String playRandomTune() {
  int pauseBetweenNotes;
  int noteDuration;
  int tempo = 1000; //increase to play slower
  float tempo2 = 1.05; //increase to play slower
  
  int randomNum = random(numOfSongChoices);
  Song song = songChoices[randomNum];

  for (int thisNote = 0; thisNote < song.numberOfNotes; thisNote++) {
    //convert duration into time delay
    noteDuration = tempo / song.noteDurations[thisNote];
    tone(song.notes[thisNote], noteDuration);
    Serial.print(String(song.notes[thisNote]));
    Serial.println(thisNote);

    pauseBetweenNotes = noteDuration * tempo2;
    delay(pauseBetweenNotes);
  }

  return song.songName;
}


void setup() {
  Serial.begin(115200);
  //for random number
  randomSeed(analogRead(0));

  pinMode(buzzerPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(muteButtonPin, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  //PIR sensor needs to be connected to 5V
  pinMode(highPin, OUTPUT);
  digitalWrite(highPin, HIGH);

  //used for the tone function
  setupTimer();
}



void loop() {
  float hoursSinceBoot;
  static int buttonValue;
  static int oldButtonValue;
  static int currentTime;
  static bool muted = false;
  static int mutedState = 0;
  

  buttonValue = digitalRead(muteButtonPin);

  //if button is released after being pressed, mute speaker
  if (buttonValue == 1 && oldButtonValue == 0) {
    currentTime = millis();
    muted = true;
  }

  //unmute after 30 seconds
  if (millis() > (currentTime + mutePeriod)) {
      muted = false;
  }
  
  //if motion is detected by the PIR sensor
  if (digitalRead(pirPin) == HIGH) {
    //LED turns on
    digitalWrite(LEDPin, HIGH);
    
    //find hours since boot/reset
    hoursSinceBoot = millis() / 3600000.0;

    //if unmuted, play tune and display as such
    if (!muted) {
      Serial.print(playRandomTune());
      Serial.print(" was played ");
      Serial.print(String(hoursSinceBoot));
      Serial.println(" hours after boot.");
    }
    //if muted, display the tune that would have played
    else if (mutedState == 0) {
      Serial.print("Motion detected but buzzer was muted ");
      Serial.print(String(hoursSinceBoot));
      Serial.println(" hours after boot.");
      mutedState = 1;
    }
  }

  //if nothing is detected by the motion sensor
  else {
    digitalWrite(LEDPin, LOW);
    mutedState = 0;
  }

  
  oldButtonValue = buttonValue;
}
