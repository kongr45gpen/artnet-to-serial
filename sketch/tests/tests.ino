/* ================== WARNING ==================!
 * Do not forget the following DEFINE in tlc_config.h!
#define TLC_TIMER3_GSCLK 1
#define GSCLK_PIN    PE3
#define GSCLK_PORT   PORTE
#define GSCLK_DDR    DDRE
(define the GSCLKs after the inclusion of the pin .h)
 * ================== WARNING ==================!
 */

#include <DmxSimple.h>
#include "Tlc5940.h"

#ifndef TLC_TIMER3_GSCLK
  #error Please define the required values
#endif

int shift = 0;

float inter;
float freq = 6;
float a = 3;
float lag = 6.28/3-1;

void setup() {
  /* The most common pin for DMX output is pin 3, which DmxSimple
  ** uses by default. If you need to change that, do it here. */
  DmxSimple.usePin(4);

  /* DMX devices typically need to receive a complete set of channels
  ** even if you only need to adjust the first channel. You can
  ** easily change the number of channels sent here. If you don't
  ** do this, DmxSimple will set the maximum channel number to the
  ** highest channel you DmxSimple.write() to. */
  //DmxSimple.maxChannel(70);

  pinMode(2,OUTPUT);
  analogWrite(2,125);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  Tlc.init();
}

void loop() {
  int brightness;
  /* Simple loop to ramp up brightness */
  for (brightness = 0; brightness <= 255; brightness+=1) {
    for (int channel = 1;channel <= 3; channel+=1) {
      inter = (1+sin(freq*micros()/1000000.0 + (channel-1)*lag))/2.0;
      inter = (exp(a*inter)-1)/(exp(a)-1);
      brightness = ceil(255*inter);
      
      /* Update DMX channel 1 to new brightness */
      if(channel==3) DmxSimple.write(shift+channel, brightness);
    }
    DmxSimple.write(shift+4, 0);
    DmxSimple.write(shift+5, 0);
    DmxSimple.write(shift+6, 0);
    DmxSimple.write(shift+7,1);
    analogWrite(2, brightness);

    //if (brightness <2) delay(20);
    
    /* Small delay to slow down the ramping */
    delay(4);
    inter=1-inter;
    Tlc.setAll(4095*inter);
    Tlc.update();
  }

}
