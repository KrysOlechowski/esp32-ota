#include <assert.h>
#include <stdint.h>
#include "stepper_motion.h"

int main() {
  StepperMotion m;
  assert(!m.running() && m.mask() == 0);
  m.start(1, 0);
  m.tick(9); assert(m.mask() == 0);
  const uint8_t sequence[] = {1, 3, 2, 6, 4, 12, 8, 9};
  for (uint32_t i = 0; i < 128; ++i) {
    m.tick((i + 1) * 10);
    assert(m.running() && m.mask() == sequence[i % 8]);
  }
  m.tick(1290); assert(!m.running() && m.mask() == 0);
  m.start(-1, 2000);
  m.tick(2010); assert(m.mask() == 8);  // odwrócona kolejność z ostatniej fazy
  m.tick(2020); assert(m.mask() == 12);
  m.stop(); assert(!m.running() && m.mask() == 0);
  m.tick(2030); assert(m.mask() == 0);
  m.start(1, 3000);
  m.tick(4000); assert(m.mask() == 8); // tylko jeden krok po długiej przerwie
  m.tick(4000); assert(m.mask() == 8); // bez serii nadrabiającej
  m.tick(8000); assert(!m.running() && m.mask() == 0); // limit czasu
  const uint32_t nearWrap = UINT32_MAX - 5;
  m.start(1, nearWrap);
  m.tick(4); assert(m.mask() == 9); // millis() przechodzi przez zero
  m.tick(4994); assert(!m.running() && m.mask() == 0);
  m.start(0, 0); assert(!m.running());
  m.setZero(); assert(m.zeroKnown() && m.position() == 0);
  m.start(1, 0, 4096);
  for(uint32_t i=1; i<=4096; ++i) m.tick(i*10);
  assert(m.running() && m.position() == 0);
  m.tick(40970); assert(!m.running());
  m.start(-1, 50000, 1024);
  for(uint32_t i=1; i<=1025; ++i) m.tick(50000+i*10);
  assert(!m.running() && m.position() == 3072);
  m.home(70000); assert(m.direction() == 1);
  for(uint32_t i=1; i<=1025; ++i) m.tick(70000+i*10);
  assert(!m.running() && m.position() == 0);
  m.start(1, 90000, 4096); m.tick(90010); m.stop();
  assert(m.position() == 1);
  m.setZero(); assert(m.position() == 0);
  m.start(1, 0, 4097); assert(!m.running());
  m.start(1, 0, 4096, 50);
  m.tick(10); assert(m.mask() == 0);
  for(uint32_t i=1;i<=4097;++i)m.tick(i*20);
  assert(!m.running()); // wolny pełny obrót nie przekracza limitu czasu
  m.start(1, 0, 8, 200); m.tick(4); assert(m.mask()==0);
  m.tick(5); assert(m.mask()!=0);
  m.stop(); m.start(1, 0, 8, 123); assert(!m.running());
  m.startDiagnostic(10000); assert(!m.zeroKnown());
  m.tick(10499); assert(m.mask() == 0 && m.diagnostic());
  for (uint32_t i = 0; i < 4; ++i) {
    m.tick(10500 + i * 500);
    assert(m.mask() == (1u << i) && m.running());
  }
  m.tick(12500); assert(m.mask() == 0 && !m.running() && !m.diagnostic());
  m.startDiagnostic(13000); m.tick(13500); m.stop();
  assert(m.mask() == 0 && !m.running() && !m.diagnostic());
  m.startDiagnostic(14000); m.tick(19000);
  assert(m.mask() == 0 && !m.running());
}
