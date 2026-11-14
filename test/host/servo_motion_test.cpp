#include <assert.h>
#include <stdint.h>
#include "servo_motion.h"
int main() {
  ServoMotion s;
  assert(!s.active());
  assert(!s.move(90,0,60));
  ServoMotion slow, fast;
  slow.move(100,0,25); slow.tick(39); assert(slow.angle()==90);
  slow.tick(40); assert(slow.angle()==91);
  fast.move(100,0,100); fast.tick(10); assert(fast.angle()==91);
  fast.tick(20); assert(fast.angle()==92);
  assert(!s.move(44, 0) && !s.move(136, 0));
  assert(!s.active() && s.target() == 90);
  assert(s.move(135, 0));
  assert(s.duty() == 4915);
  s.tick(19); assert(s.angle() == 90);
  for(uint32_t t=20; t<=900; t+=20)s.tick(t);
  assert(s.angle() == 135 && s.duty() == 5734 && s.active());
  s.tick(1899); assert(s.active());
  s.tick(1900); assert(!s.active());
  s.move(45, 2000); s.tick(3000); assert(s.angle() == 134); // bez serii nadrabiającej
  s.release(); s.tick(5000); assert(!s.active() && s.angle() == 134);
  assert(!s.move(-1,5000));
  s.move(45, UINT32_MAX-5); s.tick(14); assert(s.angle() == 133);
  for(uint32_t t=34;t<=1774;t+=20)s.tick(t);
  assert(s.angle() == 45 && s.duty() == 4095);
}
