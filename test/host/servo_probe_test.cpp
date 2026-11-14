#include <assert.h>
#include <stdint.h>
#include "servo_probe.h"
int main(){ServoProbe p;assert(!p.start(500,0));assert(p.start(1500,0));assert(!p.start(1525,1));p.tick(599);assert(p.active());p.tick(600);assert(!p.active());assert(p.start(1475,700));p.release();assert(!p.start(1400,800));assert(p.start(1500,900));p.release();for(int us=1475;us>=500;us-=25){assert(p.start(us,1000));p.release();}assert(!p.start(475,1100));assert(p.start(1500,1200));p.release();for(int us=1525;us<=2500;us+=25){assert(p.start(us,1300));p.release();}assert(!p.start(2525,1400));p.reset();assert(p.pulse()==1500);assert(p.start(1500,UINT32_MAX-5));p.tick(594);assert(!p.active());}
