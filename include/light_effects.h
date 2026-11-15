#pragma once
#include "climate_color.h"
#include <stdint.h>
#include <math.h>
class LightEffects {
 public:
  enum Mode { Off, Candle, Fire, Breathe, Rainbow, Duet };
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
    }
    return {a,b};
  }
 private:
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
