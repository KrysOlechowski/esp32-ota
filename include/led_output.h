#pragma once
#include <stdint.h>
#include <math.h>
namespace LedOutput {
// Wspólne przeliczenie dla obu RGB; korekcja kanału skaluje wynikowy PWM.
inline uint8_t duty(uint8_t channel, uint8_t brightness, uint8_t gain = 100) {
  if (brightness > 100) brightness = 100;
  if (gain > 100) gain = 100;
  const long base = lroundf(powf(channel / 255.0f * brightness / 100.0f, 2.2f) * 255);
  return uint8_t(lroundf(base * gain / 100.0f));
}
}
