#pragma once
#include <stdint.h>
#include <math.h>

namespace ClimateColor {
struct RGB { uint8_t r, g, b; };
struct Range { float cold = 17, warm = 22, dry = 30, wet = 70; };
inline float unit(float x) { return fmaxf(0, fminf(1, x)); }
// Skaluje nasycenie wybranego RGB, zachowując jego maksymalny kanał (HSV V).
inline RGB withSaturation(RGB color, float percent) {
  float maximum = fmaxf(color.r, fmaxf(color.g, color.b));
  float factor = unit(percent / 100);
  return {uint8_t(lroundf(maximum + (color.r - maximum) * factor)),
          uint8_t(lroundf(maximum + (color.g - maximum) * factor)),
          uint8_t(lroundf(maximum + (color.b - maximum) * factor))};
}
// Temperatura zmienia odcień 240° (niebieski) -> 0° (czerwony).
// Wilgotność zmienia nasycenie 55% -> 100%, nie przesuwa skali temperatury.
inline RGB fromReading(float temperature, float humidity, const Range &range) {
  if (!isfinite(temperature) || !isfinite(humidity) || range.warm <= range.cold || range.wet <= range.dry)
    return {0, 0, 0};
  float hue = 4 * (1 - unit((temperature - range.cold) / (range.warm - range.cold)));
  float saturation = .55f + .45f * unit((humidity - range.dry) / (range.wet - range.dry));
  float x = saturation * (1 - fabsf(fmodf(hue, 2) - 1));
  float r = 0, g = 0, b = 0;
  if (hue < 1) { r = saturation; g = x; }
  else if (hue < 2) { r = x; g = saturation; }
  else if (hue < 3) { g = saturation; b = x; }
  else { g = x; b = saturation; }
  float m = 1 - saturation;
  return {uint8_t(lroundf((r + m) * 255)), uint8_t(lroundf((g + m) * 255)), uint8_t(lroundf((b + m) * 255))};
}
}
