#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

unsigned long lastTick  = 0;   // For 1-second ticks
unsigned long lastFlash = 0;   // For alternating in last 10 seconds
int seconds  = 0;
int minutes  = 1;              // Start counting at 1 (human order)
bool showNextRound = false;    // Toggles between seconds and next round in last 10 seconds

uint8_t frame[8][12];

// 5×7 large font for digits 0–9
const uint8_t bigDigits[10][7] = {
  {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110}, // 0
  {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110}, // 1
  {0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111}, // 2
  {0b11110,0b00001,0b00001,0b01110,0b00001,0b00001,0b11110}, // 3
  {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010}, // 4
  {0b11111,0b10000,0b10000,0b11110,0b00001,0b00001,0b11110}, // 5
  {0b01110,0b10000,0b10000,0b11110,0b10001,0b10001,0b01110}, // 6
  {0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000}, // 7
  {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110}, // 8
  {0b01110,0b10001,0b10001,0b11110,0b00001,0b00001,0b01110}  // 9
};

// Draw any 0–99 number in big digits (normal mode)
void drawNumber(int value) {
  if (value < 0) {
    value = 0;
  }
  if (value > 99) {
    value = 99;
  }

  int tens = value / 10;
  int ones = value % 10;

  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 12; c++) {
      frame[r][c] = 0;
    }
  }

  for (int r = 0; r < 7; r++) {
    for (int c = 0; c < 5; c++) {
      if (bigDigits[tens][r] & (0b10000 >> c)) {
        frame[r][c] = 1;
      }
    }
  }

  for (int r = 0; r < 7; r++) {
    for (int c = 0; c < 5; c++) {
      if (bigDigits[ones][r] & (0b10000 >> c)) {
        frame[r][c + 6] = 1;
      }
    }
  }

  matrix.renderBitmap(frame, 8, 12);
}

// Draw number inverted: background ON, digits OFF
void drawNumberInverted(int value) {
  if (value < 0) {
    value = 0;
  }
  if (value > 99) {
    value = 99;
  }

  int tens = value / 10;
  int ones = value % 10;

  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 12; c++) {
      frame[r][c] = 1;
    }
  }

  for (int r = 0; r < 7; r++) {
    for (int c = 0; c < 5; c++) {
      if (bigDigits[tens][r] & (0b10000 >> c)) {
        frame[r][c] = 0;
      }
      if (bigDigits[ones][r] & (0b10000 >> c)) {
        frame[r][c + 6] = 0;
      }
    }
  }

  matrix.renderBitmap(frame, 8, 12);
}

void setup() {
  matrix.begin();
  seconds       = 0;
  minutes       = 1;
  lastTick      = millis();
  lastFlash     = millis();
  showNextRound = false;

  drawNumber(0);
}

void loop() {
  unsigned long now = millis();

  if (now - lastTick >= 1000) {
    lastTick += 1000;

    seconds++;
    if (seconds >= 60) {
      seconds = 0;

      if (minutes < 99) {
        minutes++;
      } else {
        minutes = 1;   // wrap safely, stays human-ordered
      }
    }

    if (seconds < 50) {
      drawNumber(seconds);
    } else {
      int nextMinute = (minutes < 99) ? (minutes + 1) : 99;

      if (showNextRound) {
        drawNumberInverted(nextMinute);
      } else {
        drawNumber(seconds);
      }
    }
  }

  if (seconds >= 50) {
    if (now - lastFlash >= 1000) {
      lastFlash = now;
      showNextRound = !showNextRound;

      int nextMinute = (minutes < 99) ? (minutes + 1) : 99;

      if (showNextRound) {
        drawNumberInverted(nextMinute);
      } else {
        drawNumber(seconds);
      }
    }
  } else {
    showNextRound = false;
  }
}

