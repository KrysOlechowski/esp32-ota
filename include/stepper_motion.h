#pragma once
#include <stdint.h>

// Ograniczony ruch testowy; bez nadrabiania kroków seriami po opóźnieniu pętli.
class StepperMotion {
 public:
  static constexpr uint32_t INTERVAL_MS = 10;
  static constexpr uint32_t TIMEOUT_MS = 5000;
  static constexpr uint16_t TEST_STEPS = 128;

  void start(int8_t direction, uint32_t now) {
    stop();
    if (direction != -1 && direction != 1) return;
    direction_ = direction;
    remaining_ = TEST_STEPS;
    last_ = started_ = now;
  }
  void startDiagnostic(uint32_t now) {
    stop();
    diagnostic_ = true;
    direction_ = 1;
    remaining_ = 4;
    last_ = started_ = now;
  }
  void stop() { direction_ = 0; remaining_ = 0; mask_ = 0; diagnostic_ = false; }
  void tick(uint32_t now) {
    if (!running()) return;
    if (uint32_t(now - started_) >= TIMEOUT_MS) { stop(); return; }
    if (uint32_t(now - last_) < (diagnostic_ ? 500u : INTERVAL_MS)) return;
    last_ = now;
    if (!remaining_) { stop(); return; }
    if (diagnostic_) {
      mask_ = uint8_t(1u << (4 - remaining_));
      --remaining_;
      return;
    }
    static const uint8_t sequence[] = {1, 3, 2, 6, 4, 12, 8, 9};
    phase_ = uint8_t((phase_ + direction_ + 8) % 8);
    mask_ = sequence[phase_];
    --remaining_;
  }
  bool running() const { return direction_ != 0; }
  int8_t direction() const { return direction_; }
  uint8_t mask() const { return mask_; }
  bool diagnostic() const { return diagnostic_; }
 private:
  uint32_t last_ = 0, started_ = 0;
  uint16_t remaining_ = 0;
  int8_t direction_ = 0;
  uint8_t phase_ = 7, mask_ = 0;
  bool diagnostic_ = false;
};
