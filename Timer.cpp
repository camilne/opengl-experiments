#include "Timer.h"
#include <iostream>

void Timer::start() {
  isRunning = true;
  startTime = Clock::now();
}

std::chrono::duration<float> Timer::getElapsedTime() const {
  if (isRunning) {
    return Clock::now() - startTime;
  } else {
    std::cout << "Clock not running" << std::endl;
    return Clock::duration(0);
  }
}

double Timer::getElapsedTimeInSeconds() const {
  return std::chrono::duration_cast<std::chrono::microseconds>(getElapsedTime()).count() * 1e-6;
}


void Timer::reset() {
  startTime = Clock::now();
}

void Timer::stop() {
  isRunning = false;
}
