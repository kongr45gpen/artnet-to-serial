// Target: Arduino/Genuino Mega or Mega 2560

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


enum operation {
  waiting,
  dmx,
  unknown
};

void setup() {

  Serial.begin(230400);
  
  Serial.print("r&c");

  // ERROR pin
  pinMode(4, OUTPUT);
  // INPUT pin
  pinMode(31, OUTPUT);
  // DMX MAX ERROR pin
  pinMode(32, OUTPUT);

  digitalWrite(4, HIGH);
  delay(100);
  digitalWrite(4, LOW);

  {
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
  }

  {
    /**
     * DMX INITIALISATION
     */
     DmxSimple.usePin(3);

     for (int i = 1; i <= 16; i++) {
      DmxSimple.write(i, 25);
      delay(50);
      DmxSimple.write(i, 0);
     }
  }

  // Flash the lights so we know everything is OK
  digitalWrite(4, HIGH);
  digitalWrite(31, HIGH);
  digitalWrite(32, HIGH);
  delay(10);
  digitalWrite(4, LOW);
  digitalWrite(31, LOW);
  digitalWrite(32, LOW);
}

uint8_t code;
uint8_t op;

operation status;

uint16_t channel = 0;
uint16_t start;
uint8_t length;
uint16_t value;

float a = 7;

void loop() {
  digitalWrite(31, LOW);

  // Read operation
  while(!Serial.available());
  digitalWrite(31, HIGH);
  code = Serial.read();

  if (code == 125) {
    // Operation code
    while(!Serial.available());
    op = Serial.read();
    
    if (code == op) {
      // pass the code to the next command
    } else {
      status = waiting;
      if (op == 'f') {
        // full dmx package
        status = dmx;
        channel = 0;
      } else if (op == 'e') {
        // reset error
        digitalWrite(4, LOW);
        digitalWrite(32, LOW);
      } else {
        // error
        digitalWrite(4, HIGH);
      }
      return;
    }
  }
  
  if (status == dmx) {
    if (channel >= 512) {
      channel = 0;
      digitalWrite(32, HIGH);
    }

    DmxSimple.write(channel + 1, code);

    float inter = code/255.0;
    inter = (exp(a*inter)-1)/(exp(a)-1);
    value = 4095*inter;
    value = 4095-value;

    // DMX channels 7-12 are used to power PWM devices, e.g. a LED strip,
    // connected on the Arduino's "analog" output pins.
    if (channel < 16) {
      Tlc.set(channel,value);
    }
    if (channel == 15) {
      Tlc.update();
    }

    channel++;
  } else {
    // error
   digitalWrite(4, HIGH); 
  }
}
