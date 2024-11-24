#include <Arduino.h>

class Timer {
 private:
  unsigned long startTime;
  unsigned long duration;
  bool running;

 public:
  // Constructor to initialize the timer with a specific duration (in
  // milliseconds)
  Timer(unsigned long durationMs) {
    duration = durationMs;
    startTime = 0;
    running = false;
  }

  // Start the timer
  void start() {
    startTime = millis();
    running = true;
  }

  // Check if the timer has elapsed
  bool hasElapsed() {
    if (running && (millis() - startTime >= duration)) {
      running = false;  // Stop the timer after it has elapsed
      return true;
    }
    return false;
  }

  // Reset the timer manually
  void reset() {
    running = false;
    startTime = 0;
  }

  // Check if the timer is currently running
  bool isRunning() { return running; }
};
