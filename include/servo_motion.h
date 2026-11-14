#pragma once
#include <stdint.h>

// Początkowo ograniczony zakres, bez dojazdu do mechanicznych końców SG90.
class ServoMotion {
 public:
  bool move(int angle, uint32_t now, uint16_t speed = 50) {
    if (angle < 45 || angle > 135 || (speed != 25 && speed != 50 && speed != 100)) return false;
    interval_ = 1000u / speed;
    target_ = angle; active_ = true; last_ = settled_ = now;
    return true;
  }
  void release() { active_ = false; }
  void tick(uint32_t now) {
    if (!active_) return;
    if (angle_ != target_) {
      if (uint32_t(now - last_) < interval_) return;
      last_ = now;
      angle_ += angle_ < target_ ? 1 : -1;
      settled_ = now;
    } else if (uint32_t(now - settled_) >= 1000) release();
  }
  int angle() const { return angle_; }
  int target() const { return target_; }
  bool active() const { return active_; }
  uint32_t duty() const {
    // 0..180 -> 1000..2000 us, ramka 20000 us, PWM 16 bitów.
    return uint32_t((1000u + uint32_t(angle_) * 1000u / 180u) * 65535u / 20000u);
  }
 private:
  int angle_ = 90, target_ = 90;
  bool active_ = false;
  uint32_t last_ = 0, settled_ = 0, interval_ = 20;
};
