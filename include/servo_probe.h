#pragma once
#include <stdint.h>
class ServoProbe {
 public:
  bool start(int us, uint32_t now) {
    if (active_ || us < 500 || us > 2500 ||
        (us != 1500 && (us - pulse_ > 25 || pulse_ - us > 25))) return false;
    pulse_ = us; started_ = now; active_ = true; return true;
  }
  void tick(uint32_t now) { if (active_ && uint32_t(now-started_) >= 600) release(); }
  void release() { active_ = false; }
  void reset() { release(); pulse_ = 1500; }
  bool active() const { return active_; }
  int pulse() const { return pulse_; }
  uint32_t duty() const { return uint32_t(pulse_) * 65535u / 20000u; }
 private:
  int pulse_ = 1500;
  bool active_ = false;
  uint32_t started_ = 0;
};
