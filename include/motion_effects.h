#pragma once
#include <stdint.h>
class MotionEffects {
 public:
  enum Mode{None,Swing,Dance,Tour,Wave,Nod,Scan};
  struct Cue{int value;int8_t direction;};
  bool start(Mode mode,uint32_t now){if(active()||mode==None||mode>Scan)return false;mode_=mode;index_=0;waiting_=pause_=false;started_=last_=now;return true;}
  void stop(){mode_=None;}
  bool active()const{return mode_!=None;}
  bool servo()const{return mode_>=Wave;}
  Mode mode()const{return mode_;}
  bool next(uint32_t now,bool busy,Cue &out){
    if(!active())return false;
    if(uint32_t(now-started_)>=120000){stop();return false;}
    if(busy)return false;
    if(waiting_){waiting_=false;pause_=true;last_=now;return false;}
    if(pause_&&uint32_t(now-last_)<600)return false;
    pause_=false;
    const Cue swing[]={{512,1},{1024,-1},{512,1}};
    const Cue dance[]={{128,1},{128,-1},{256,1},{256,-1},{128,1},{128,-1}};
    const Cue tour[]={{1024,1},{1024,1},{1024,1},{1024,1}};
    const Cue wave[]={{60,0},{120,0},{60,0},{120,0},{90,0}};
    const Cue nod[]={{80,0},{100,0},{80,0},{100,0},{90,0}};
    const Cue scan[]={{60,0},{75,0},{90,0},{105,0},{120,0},{105,0},{90,0},{75,0},{60,0},{90,0}};
    const Cue *c=nullptr;uint8_t count=0;
    switch(mode_){case Swing:c=swing;count=3;break;case Dance:c=dance;count=6;break;case Tour:c=tour;count=4;break;case Wave:c=wave;count=5;break;case Nod:c=nod;count=5;break;case Scan:c=scan;count=10;break;default:break;}
    if(index_>=count){stop();return false;}
    out=c[index_++];waiting_=true;return true;
  }
 private:
  Mode mode_=None;uint8_t index_=0;bool waiting_=false,pause_=false;uint32_t started_=0,last_=0;
};
