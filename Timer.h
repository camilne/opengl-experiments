#pragma once
#include <chrono>

class Timer {
  using Clock = std::chrono::high_resolution_clock;

  std::chrono::time_point<Clock> startTime;
  bool isRunning;
public:
  Timer()
    : isRunning(false)
  {}

  void start();
  std::chrono::duration<float> getElapsedTime() const;
  double getElapsedTimeInSeconds() const;
  void reset();
  void stop();
};
