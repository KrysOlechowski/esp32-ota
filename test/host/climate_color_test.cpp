// Uruchom: c++ -std=c++11 -Iinclude test/host/climate_color_test.cpp -o /tmp/climate-color-test && /tmp/climate-color-test
#include "climate_color.h"
#include <cassert>
#include <cstdio>
using namespace ClimateColor;
int main() {
  RGB pastel = withSaturation({255, 0, 0}, 55);
  assert(pastel.r == 255 && pastel.g == 115 && pastel.b == 115);
  RGB white = withSaturation({0, 100, 255}, 0);
  assert(white.r == 255 && white.g == 255 && white.b == 255);
  RGB original = withSaturation({10, 50, 120}, 100);
  assert(original.r == 10 && original.g == 50 && original.b == 120);
  RGB black = withSaturation({0, 0, 0}, 0);
  assert(black.r == 0 && black.g == 0 && black.b == 0);
  Range range;
  RGB cold = fromReading(17, 70, range), warm = fromReading(22, 70, range);
  assert(cold.b == 255 && cold.r == 0 && cold.g == 0);
  assert(warm.r == 255 && warm.g == 0 && warm.b == 0);
  RGB mid = fromReading(19.5, 70, range);
  assert(mid.g == 255 && mid.r == 0 && mid.b == 0);
  RGB below = fromReading(10, 70, range), above = fromReading(30, 70, range);
  assert(below.b == cold.b && below.r == cold.r && above.r == warm.r && above.b == warm.b);
  RGB dry = fromReading(17, 30, range);
  assert(dry.b == 255 && dry.r > cold.r && dry.g > cold.g);
  RGB invalid = fromReading(NAN, 50, range);
  assert(invalid.r == 0 && invalid.g == 0 && invalid.b == 0);
  range.warm = range.cold;
  invalid = fromReading(20, 50, range);
  assert(invalid.r == 0 && invalid.g == 0 && invalid.b == 0);
  for (int t = 170; t < 220; ++t) {
    RGB a = fromReading(t / 10.0f, 70, Range{}), b = fromReading((t+1) / 10.0f, 70, Range{});
    assert(abs(int(a.r)-b.r) <= 22 && abs(int(a.g)-b.g) <= 22 && abs(int(a.b)-b.b) <= 22);
  }
  puts("OK: cold/warm/midpoint, clamping, humidity, invalid input, continuous gradient");
}
