#include <Arduino.h>

#include "config.h"

uint8_t rows[] = {R0, R1, R2, R3, R4, R5, R6, R7};
uint8_t cols[] = {C0, C1, C2, C3, C4, C5, C6, C7};

void activate_row(uint8_t row) { digitalWrite(rows[row], HIGH); }

void deactivate_row(uint8_t row) { digitalWrite(rows[row], LOW); }

void activate_col(uint8_t col) { digitalWrite(cols[col], LOW); }

void deactivate_col(uint8_t col) { digitalWrite(cols[col], HIGH); }

void display_matrix(bool matrix[8][8], uint16_t loops = 4) {
  for (uint16_t i = 0; i < loops; i++) {
    for (uint8_t row = 0; row < 8; row++) {
      activate_row(row);
      for (uint8_t col = 0; col < 8; col++) {
        if (matrix[row][col])
          activate_col(col);
        else
          deactivate_col(col);
        _delay_us(1);
      }
      delay(2);
      for (uint8_t col = 0; col < 8; col++) deactivate_col(col);
      deactivate_row(row);
    }
  }
}

bool arrow_right_down[8][8] = {
    {1, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1}, {0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 1}, {0, 0, 0, 0, 1, 1, 1, 1}};

bool arrow_left_down[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0},
    {1, 0, 0, 1, 0, 0, 0, 0}, {1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 0, 0, 0, 0}};

bool cross[8][8] = {{1, 0, 0, 0, 0, 0, 0, 1}, {0, 1, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
                    {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0},
                    {0, 1, 0, 0, 0, 0, 1, 0}, {1, 0, 0, 0, 0, 0, 0, 1}};

bool fill[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}};

void setup() {
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  for (auto row : rows) {
    pinMode(row, OUTPUT);
    digitalWrite(row, HIGH);
  }

  for (auto col : cols) {
    pinMode(col, OUTPUT);
    digitalWrite(col, LOW);
  }

  display_matrix(fill, 16);
}

void loop() {
  bool p1 = digitalRead(A4);
  bool p2 = digitalRead(A5);

  if (p1 && p2) {
    display_matrix(cross);
  } else if (p1) {
    display_matrix(arrow_left_down);
  } else if (p2) {
    display_matrix(arrow_right_down);
  }
}
