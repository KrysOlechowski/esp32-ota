#pragma once
#include "climate_color.h"
#include <stdint.h>
#include <math.h>
class LightEffects {
 public:
  enum Mode { Off, Candle, Fire, Breathe, Rainbow, Duet, Aurora, Ocean, Sunset, Heartbeat, PingPong };
  struct Frame { ClimateColor::RGB first, second; };
  void start(Mode m,uint32_t now){mode_=m;started_=now;}
  bool active()const{return mode_!=Off;}
  Mode mode()const{return mode_;}
  Frame frame(uint32_t now)const{
    const uint32_t t=now-started_;
    ClimateColor::RGB a={0,0,0},b={0,0,0};
    if(mode_==Candle||mode_==Fire){
      const uint32_t period=mode_==Candle?170:90;
      const uint32_t n=t/period;
      const float f=float(t%period)/period;
      const float x=noise(n)*(1-f)+noise(n+1)*f;
      const float v=mode_==Candle?.72f+.28f*x:.3f+.7f*x;
      a={uint8_t(255*v),uint8_t((mode_==Candle?105:60)*v+25*x),uint8_t(mode_==Candle?8*v:0)};b=a;
    }else if(mode_==Breathe){
      const float v=.08f+.92f*(.5f-.5f*cosf(float(t%4000)*6.2831853f/4000));
      a={uint8_t(35*v),uint8_t(150*v),uint8_t(255*v)};b=a;
    }else if(mode_==Rainbow||mode_==Duet){
      const float h=float(t%12000)/2000;
      a=wheel(h);b=mode_==Duet?wheel(fmodf(h+3,6)):a;
    }else if(mode_==Aurora){
      const float f=wave(t,14000);
      a=mix({0,255,95},{190,15,255},f);
      b=mix({0,255,95},{190,15,255},1-f);
    }else if(mode_==Ocean){
      a=mix({0,25,150},{0,220,255},wave(t,6000));
      b=mix({0,25,150},{0,220,255},wave(t%6000+3000,6000));
    }else if(mode_==Sunset){
      const float f=wave(t,24000);
      a=f<.5f?mix({255,100,0},{255,10,65},2*f)
                :mix({255,10,65},{120,0,210},2*f-1);
      b=a;
    }else if(mode_==Heartbeat){
      a=scale({255,8,30},heartbeat(t));
      b=scale({255,8,30},heartbeat(t%2000+1000));
    }else if(mode_==PingPong){
      const float f=wave(t,4000);
      a=scale({255,35,130},f);
      b=scale({255,35,130},1-f);
    }
    return {a,b};
  }
 private:
  static float wave(uint32_t t,uint32_t period){return .5f-.5f*cosf(float(t%period)*6.2831853f/period);}
  static ClimateColor::RGB mix(ClimateColor::RGB a,ClimateColor::RGB b,float f){
    return {uint8_t(lroundf(a.r+(b.r-a.r)*f)),uint8_t(lroundf(a.g+(b.g-a.g)*f)),uint8_t(lroundf(a.b+(b.b-a.b)*f))};
  }
  static ClimateColor::RGB scale(ClimateColor::RGB c,float f){return mix({0,0,0},c,f);}
  static float heartbeat(uint32_t t){
    const uint32_t p=t%2000;
    if(p<240)return sinf(float(p)*3.14159265f/240);
    if(p>=340&&p<620)return .7f*sinf(float(p-340)*3.14159265f/280);
    return 0;
  }
  static float noise(uint32_t x){x=(x+1)*747796405u+2891336453u;x=(x^(x>>16))*2246822519u;return float(x&65535)/65535;}
  static ClimateColor::RGB wheel(float h){
    const float x=255*(1-fabsf(fmodf(h,2)-1));
    if(h<1)return {255,uint8_t(x),0};
    if(h<2)return {uint8_t(x),255,0};
    if(h<3)return {0,255,uint8_t(x)};
    if(h<4)return {0,uint8_t(x),255};
    if(h<5)return {uint8_t(x),0,255};
    return {255,0,uint8_t(x)};
  }
  Mode mode_=Off; uint32_t started_=0;
};
