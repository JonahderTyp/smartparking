#include "button.h"

#include <Arduino.h>

Button::Button(int pin, unsigned long holdTimeThreshold) 
  : buttonPin(pin), holdTimeThreshold(holdTimeThreshold), lastDebounceTime(0),
    debounceDelay(50), buttonPressTime(0), buttonState(LOW), 
    lastButtonState(LOW), shortPressDetected(false), longPressDetected(false), 
    longPressHandled(false) {
  pinMode(buttonPin, INPUT_PULLUP);
}

// Method to update the button state
void Button::handle() {
  int reading = !digitalRead(buttonPin);

  // Check for state change and handle debouncing
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Detect button press
      if (buttonState == HIGH) {
        buttonPressTime = millis();
        shortPressDetected = false;
        longPressDetected = false;
        longPressHandled = false;
      }
      // Detect button release
      else {
        if (!longPressHandled &&
            (millis() - buttonPressTime) < holdTimeThreshold) {
          shortPressDetected = true;
        }
        longPressHandled = false;  // Reset long press handled flag on release
      }
    }

    // Check for long press while button is held down
    if (buttonState == HIGH &&
        (millis() - buttonPressTime) >= holdTimeThreshold) {
      if (!longPressHandled) {
        longPressDetected = true;
        longPressHandled =
            true;  // Ensure long press is handled only once per press
      }
    }
  }

  lastButtonState = reading;
}

// Method to check if a short press has been detected
bool Button::isShortPress() {
  if (shortPressDetected) {
    shortPressDetected = false;
    return true;
  }
  return false;
}

// Method to check if a long press has been detected
bool Button::isLongPress() {
  if (longPressDetected) {
    longPressDetected = false;
    return true;
  }
  return false;
}