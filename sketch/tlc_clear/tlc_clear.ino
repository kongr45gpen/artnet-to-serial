// Target: Arduino/Genuino Mega or Mega 2560

/* ================== WARNING ==================!
 * Do not forget the following DEFINE in tlc_config.h!
#define TLC_TIMER3_GSCLK 1
#define GSCLK_PIN    PE3
#define GSCLK_PORT   PORTE
#define GSCLK_DDR    DDRE
#define VPRG_ENABLED 1
(define the GSCLKs after the inclusion of the pin .h)
 * ================== WARNING ==================!
 * 
 * Connect TLC pin 27 (VPRG) to Arduino Mega pin 50
 * 
 */

#include "Tlc5940.h"

#ifndef TLC_TIMER3_GSCLK
  #error Please define the required values
#endif

void setup() {
    /**
     * TLC INITIALISATION
     */
    Tlc.init();
    
    // TLC test run
    Tlc.setAll(0);
    Tlc.update();
    delay(100);
    Tlc.setAll(4095);
    Tlc.update();

    pinMode(13, OUTPUT);
}

void loop() {
  delay(1000);
  digitalWrite(13, HIGH);
  
  Tlc.setAllDC(255);
  
  delay(200);
  digitalWrite(13, LOW);
}
