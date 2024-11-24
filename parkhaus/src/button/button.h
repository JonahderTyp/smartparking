#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
 public:
  // Constructor to initialize the button pin and hold time threshold
  Button(int pin, unsigned long holdTimeThreshold = 1000);

  // Method to update the button state, should be called in the loop
  void handle();

  // Method to check if a short press has been detected
  bool isShortPress();

  // Method to check if a long press has been detected
  bool isLongPress();

 private:
  int buttonPin;                    // Pin number where the button is connected
  unsigned long holdTimeThreshold;  // Time threshold to differentiate between
                                    // short and long press
  unsigned long
      lastDebounceTime;           // Time when the button state was last changed
  unsigned long debounceDelay;    // Debounce delay time
  unsigned long buttonPressTime;  // Time when the button was pressed

  bool buttonState;         // Current state of the button
  bool lastButtonState;     // Previous state of the button
  bool shortPressDetected;  // Flag for short press detection
  bool longPressDetected;   // Flag for long press detection
  bool longPressHandled;  // Flag to indicate if the long press has been handled
};

#endif  // BUTTON_H