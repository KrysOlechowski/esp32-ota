#include <assert.h>
#include "led_output.h"
int main(){
  assert(LedOutput::duty(255,100)==255);
  assert(LedOutput::duty(255,100,50)==128);
  assert(LedOutput::duty(255,100,0)==0);
  assert(LedOutput::duty(255,255,255)==255);
  for(int c=0;c<=255;++c){
    assert(LedOutput::duty(c,0)==0);
    assert(LedOutput::duty(0,c)==0);
    for(int b=0;b<=100;++b){
      assert(LedOutput::duty(c,b)==LedOutput::duty(c,b,100));
      assert(LedOutput::duty(c,b,35)<=LedOutput::duty(c,b));
    }
  }
}
