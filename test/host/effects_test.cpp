#include <assert.h>
#include "light_effects.h"
#include "motion_effects.h"
#include "servo_motion.h"
#include "stepper_motion.h"
int main(){
  LightEffects l;
  assert(!l.active()&&l.frame(0).first.r==0);
  for(int mode=1;mode<=5;++mode){
    l.start(LightEffects::Mode(mode),100);
    for(uint32_t t=100;t<20000;t+=37){
      auto f=l.frame(t);
      if(mode<=2){assert(f.first.r>f.first.g);assert(f.first.b<10);}
      if(mode!=5){assert(f.first.r==f.second.r&&f.first.g==f.second.g&&f.first.b==f.second.b);}
      else assert(f.first.r!=f.second.r||f.first.g!=f.second.g||f.first.b!=f.second.b);
    }
  }
  l.start(LightEffects::Breathe,0);assert(l.frame(2000).first.b==255);assert(l.frame(0).first.b<30);
  l.start(LightEffects::Candle,UINT32_MAX-20);auto wrap=l.frame(79);l.start(LightEffects::Candle,0);assert(l.frame(100).first.r==wrap.first.r);
  for(int mode=1;mode<=6;++mode){
    MotionEffects fx;StepperMotion motor;ServoMotion servo;int total=0,count=0;
    assert(fx.start(MotionEffects::Mode(mode),0));assert(!fx.start(MotionEffects::Swing,0));
    for(uint32_t t=0;t<120001&&fx.active();++t){
      motor.tick(t);servo.tick(t);MotionEffects::Cue cue;
      if(fx.next(t,motor.running()||servo.active(),cue)){
        ++count;
        if(fx.servo()){assert(cue.value>=60&&cue.value<=120);assert(servo.move(cue.value,t));}
        else{total+=cue.direction*cue.value;motor.start(cue.direction,t,cue.value);}
      }
    }
    assert(!fx.active());const int expected[]={0,3,6,4,5,5,10};assert(count==expected[mode]);
    if(mode<=3){assert(total==(mode==3?4096:0));assert(motor.position()==0);}
    else assert(servo.angle()==90);
  }
  MotionEffects fx;MotionEffects::Cue cue;
  fx.start(MotionEffects::Swing,0);assert(!fx.next(120000,true,cue));assert(!fx.active());
  fx.start(MotionEffects::Wave,UINT32_MAX-10);assert(fx.next(UINT32_MAX-10,false,cue));
  fx.stop();assert(!fx.next(20,false,cue));assert(!fx.active());
}
