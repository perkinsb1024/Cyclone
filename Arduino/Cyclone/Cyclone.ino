#include <Wire.h>
#include <toneAC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731_Mod.h>

const bool SOUND_ON_BY_DEFAULT = true;

const int buttonPin = 2;
const int shutdownPin = 4;

bool ledStatus;
Adafruit_IS31FL3731 ledmatrix = Adafruit_IS31FL3731();
int statusReg;

void setup() {
  statusReg = MCUSR;
  pinMode(shutdownPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  // Enable driver by pulling pin 4 high
  digitalWrite(shutdownPin, HIGH);

  ledStatus = false;
  ledmatrix.begin();
  
  // Disable the LEDs we're not using
  ledmatrix.disableLedPage(1);
  ledmatrix.disableLedPage(3);
  ledmatrix.disableLedPage(5);
  ledmatrix.disableLedPage(7);
  //ledmatrix.disableLedPage(8); // Blinkers
  ledmatrix.disableLedPage(9);
  ledmatrix.disableLedPage(10);
  ledmatrix.disableLedPage(11);
  ledmatrix.disableLedPage(12);
  ledmatrix.disableLedPage(13);
}

void playSound(uint8_t melody) {
  switch(melody) {
  case 0:
      toneAC(220, 5, 125, true);
      delay(125);
      toneAC(294, 5, 125, true);
      delay(125);
      toneAC(440, 5, 350, true);
    break;
  case 1:
    toneAC(140, 5, 500, true);
    break;
  }
}

void drawSequential(uint8_t led, uint8_t brightness) {
  uint8_t x, y;
  if(led < 32) {
    y = led / 8;
    x = led % 8; 
    ledmatrix.drawPixel(x, y, brightness);
  }
}

void drawFlash(bool alt, uint8_t brightness) {
  ledmatrix.drawPixel(0, 4, alt ? brightness : 0);
  ledmatrix.drawPixel(1, 4, !alt ? brightness : 0);
}

void cyclone(bool audio) {
  uint8_t head, tail1, tail2, tail3, tail4 = 0;
  uint8_t tailLength = 0;
  bool buttonLockout = true;
  bool clockwise = false;
  bool altEffect = false;
  while(1) {
    if(buttonLockout && digitalRead(buttonPin) == HIGH) {
      buttonLockout = false;
    }
    if(clockwise) {
      head -= 1;
      tail1 = head + 1;
      tail2 = head + 2;
      tail3 = head + 3;
    } else {
      head += 1;
      tail1 = head - 1;
      tail2 = head - 2;
      tail3 = head - 3;
    }
    ledmatrix.clear();
    if(!(head % 8)) {
      altEffect = !altEffect;
    }
    drawFlash(altEffect, 128);
    drawSequential(head % 32, 255);
    if(tailLength >= 1) {
      drawSequential(tail1 % 32, 32);
    }
    if(tailLength >= 2) {
      drawSequential(tail2 % 32, 16);
    }
    if(tailLength >= 3) {
      drawSequential(tail3 % 32, 8);
    } else {
      tailLength++;
    }
    for(uint8_t b = 0; b < 5; b++) {
      if(!buttonLockout && digitalRead(buttonPin) == LOW) {
        ledmatrix.clear();
        drawSequential(head % 32, 255);
        
        if(head % 32 == 8) {
          if(audio) {
            playSound(0);
          }
          alternate(125, 3);
          ledmatrix.clear();
          drawSequential(head % 32, 255);
          delay(125);
        }
        else {
          if(audio) {
            playSound(1);
          }
          delay(1000);
        }
        buttonLockout = true;
        tailLength = 0;
        clockwise = !clockwise;
      }
      delay(1);
    }
  }
}

void cycle(uint8_t delayMs, uint8_t numLoops) {
  for(; numLoops > 0; numLoops--) {
    for(uint8_t y = 0; y < 4; y++) {
      for (uint8_t x = 0; x < 8; x++) {
        ledmatrix.clear();
        ledmatrix.drawPixel(x, y, 255);
        delay(delayMs);
      }
    }
  }
}

void breath(uint8_t delayMs, uint8_t numLoops) {
  for(; numLoops > 0; numLoops--) {
    for(int intensity = 0; intensity <= 254; intensity+=2) {
      for(uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          ledmatrix.drawPixel(x, y, intensity);
          //delay(delayMs);
          //while(digitalRead(buttonPin) == LOW) {}
        }
      }
    }
    for(int intensity = 255; intensity >= 1; intensity-=2) {
      for(uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          ledmatrix.drawPixel(x, y, intensity);
          //delay(delayMs);
          //while(digitalRead(buttonPin) == LOW) {}
        }
      }
    }
    delay(100);
  }
}

void alternate(uint8_t delayMs, uint8_t numLoops) {
  for(; numLoops > 0; numLoops--) {
    for(uint8_t i = 0; i < 2; i++) {
      ledmatrix.clear();
      for(uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          ledmatrix.drawPixel(x, y, x % 2 == i ? 255 : 0);
        }
      }
      delay(delayMs);
    }
  }
}

void loop() {
  //foo();
  cyclone(digitalRead(buttonPin) == SOUND_ON_BY_DEFAULT);
  //cycle(5, 3);
  //breath(1, 1);
  //alternate(175, 5);
}
