// Target: Arduino UNO

#include "Tlc5940.h"


enum operation {
  waiting,
  dmx,
  unknown
};

void setup() {

  Serial.begin(230400);
  Serial.print("r&c");

  pinMode(4, HIGH);
  delay(100);
  pinMode(4, LOW);
  
  Tlc.init();

    for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      Tlc.set(j, (i==j) ? 0 : 4095);
    }
    Tlc.update();
    delay(400);
  }
}

uint8_t code;
uint8_t op;

operation status;

uint16_t channel = 0;
uint16_t start;
uint8_t length;
uint16_t value;

float a = 3;

void loop() {
  digitalWrite(4, LOW);

  // Read operation
  while(!Serial.available());
  digitalWrite(4, HIGH);
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
      } else {
        // error
        digitalWrite(4, HIGH);
      }
      return;
    }
  }
  
  if (status == dmx) {
    if (channel >= 14) {
      channel = 0;
      digitalWrite(4, HIGH);
    }

    float inter = code/255.0;
    inter = (exp(-a*inter)-1)/(exp(-a)-1);
    value = 4095*inter;

    // DMX channels 7-12 are used to power PWM devices, e.g. a LED strip,
    // connected on the Arduino's "analog" output pins.
    Tlc.set(channel,value);
    Tlc.update();
  } else {
    // error
   digitalWrite(4, HIGH); 
  }
}
