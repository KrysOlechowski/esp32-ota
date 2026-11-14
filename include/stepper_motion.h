#pragma once
#include <stdint.h>

// Ograniczony ruch testowy; bez nadrabiania kroków seriami po opóźnieniu pętli.
class StepperMotion {
 public:
  static constexpr uint32_t INTERVAL_MS = 10;
  static constexpr uint32_t TIMEOUT_MS = 5000;
  static constexpr uint16_t TEST_STEPS = 128;

  static constexpr uint16_t STEPS_PER_TURN = 4096; // nominalnie, wymaga kalibracji egzemplarza
  void start(int8_t direction, uint32_t now, uint16_t steps = TEST_STEPS, uint16_t speed = 100) {
    stop();
    if ((direction != -1 && direction != 1) || !steps || steps > STEPS_PER_TURN ||
        (speed != 50 && speed != 100 && speed != 200)) return;
    interval_ = 1000u / speed;
    direction_ = direction;
    remaining_ = steps;
    timeout_ = steps * interval_ + 2000;
    if (timeout_ < TIMEOUT_MS) timeout_ = TIMEOUT_MS;
    last_ = started_ = now;
  }
  void startDiagnostic(uint32_t now) {
    stop();
    diagnostic_ = true;
    zeroKnown_ = false;
    timeout_ = TIMEOUT_MS;
    direction_ = 1;
    remaining_ = 4;
    last_ = started_ = now;
  }
  void stop() { direction_ = 0; remaining_ = 0; mask_ = 0; diagnostic_ = false; }
  void tick(uint32_t now) {
    if (!running()) return;
    if (uint32_t(now - started_) >= timeout_) { stop(); return; }
    if (uint32_t(now - last_) < (diagnostic_ ? 500u : interval_)) return;
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
    position_ = (position_ + direction_ + STEPS_PER_TURN) % STEPS_PER_TURN;
    --remaining_;
  }
  void setZero() { if (!running()) { position_ = 0; zeroKnown_ = true; } }
  void home(uint32_t now, uint16_t speed = 100) {
    if (running() || !zeroKnown_ || !position_) return;
    // Najkrótsza droga do umownego zera w obrębie jednego obrotu.
    if (position_ <= STEPS_PER_TURN / 2) start(-1, now, position_, speed);
    else start(1, now, STEPS_PER_TURN - position_, speed);
  }
  uint16_t position() const { return position_; }
  bool zeroKnown() const { return zeroKnown_; }
  bool running() const { return direction_ != 0; }
  int8_t direction() const { return direction_; }
  uint8_t mask() const { return mask_; }
  bool diagnostic() const { return diagnostic_; }
 private:
  uint32_t last_ = 0, started_ = 0, timeout_ = TIMEOUT_MS, interval_ = INTERVAL_MS;
  uint16_t remaining_ = 0, position_ = 0;
  bool zeroKnown_ = false;
  int8_t direction_ = 0;
  uint8_t phase_ = 7, mask_ = 0;
  bool diagnostic_ = false;
};
