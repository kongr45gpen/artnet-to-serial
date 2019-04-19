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
//#include "Tlc5940.h"

//#ifndef TLC_TIMER3_GSCLK
//  #error Please define the required values
//#endif


enum operation {
  waiting,
  dmx,
  unknown
};

void setup() {
  Serial.begin(250000);
  
  Serial.print("r&c");

  // Deactivate DMX, just to be sure
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  // ERROR pin
  pinMode(4, OUTPUT);
  // INPUT pin
  pinMode(31, OUTPUT);
  // DMX MAX ERROR pin
  pinMode(32, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);

  {
    /**
     * TLC INITIALISATION
     */
    //Tlc.init();

    /**
     * Set timer prescaler to /8 to reduce PWM frequency
     * Remove these lines if your hardware can handle high frequencies
     */
 /*   TCCR1B &= ~_BV(CS10);
    TCCR1B |=  _BV(CS11);
    TCCR3B &= ~_BV(CS30);
    TCCR3B |=  _BV(CS31);*/
  
    // TLC test run
    /*
    Tlc.setAll(0);
    Tlc.update();
    delay(500);
    Tlc.setAll(4095);
    Tlc.update();
    */
  }

  {
    /**
     * DMX INITIALISATION
     */
     DmxSimple.usePin(3);

     for (int i = 1; i <= 3; i++) {
      DmxSimple.write(i, 255);
      delay(150);
      DmxSimple.write(i, 25);
      delay(150);
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

float a_tlc = 7;
float a_dmx = 3;

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

    float origin = code/255.0;

    float inter = (exp(a_dmx*origin)-1)/(exp(a_dmx)-1);
    DmxSimple.write(channel + 1, 255*inter);

    if (channel == 46 || channel == 47 || channel == 48) {

      inter = (exp(a_tlc*origin)-1)/(exp(a_tlc)-1);
      value = 255*inter;
      //value = 4095-value;

      analogWrite(channel-37, value);
    }

    // DMX channels 7-12 are used to power PWM devices, e.g. a LED strip,
    // connected on the Arduino's "analog" output pins.
    /*if (channel >= 7 && channel < 7 + 16) {
      Tlc.set(channel - 7,value);
    }
    if (channel == 15 + 7) {
      Tlc.update();
    }*/

    channel++;
  } else {
    // error
   digitalWrite(4, HIGH); 
  }
}
