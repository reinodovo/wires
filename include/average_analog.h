#ifndef AVERAGE_ANALOG_H
#define AVERAGE_ANALOG_H

#include <Arduino.h>
#include <queue>

const int SAMPLES = 128;

struct AverageAnalogRead {
  std::queue<int> values;
  int sum;
  int pin;
  AverageAnalogRead(){};
  AverageAnalogRead(int pin) : pin(pin), sum(0) {}
  void update() {
    int val = analogRead(pin);
    sum += val;
    values.push(val);
    if (values.size() > SAMPLES) {
      sum -= values.front();
      values.pop();
    }
  }
  int value() {
    if (values.size() == 0)
      return 0;
    return sum / values.size();
  }
};

#endif // AVERAGE_ANALOG_H