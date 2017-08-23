// Target: Arduino UNO

#include <DmxSimple.h>

// Directly use the dmxBuffer variable from DmxSimple.cpp
// so that we don't have to call an expensive function
// 512 times for every DMX update
extern volatile uint8_t dmxBuffer[DMX_SIZE];
extern uint16_t dmxMax;

enum operation {
  waiting,
  dmx,
  unknown
};

void setup() {
  Serial.begin(230400);
  Serial.print("r&c");
  DmxSimple.usePin(2);
  
  pinMode(13, OUTPUT); // error LED
  pinMode(4, OUTPUT); // debugging LED
  //pinMode(2, OUTPUT); // output pin
  digitalWrite(4, LOW);
  digitalWrite(13, HIGH);

  // RGB led strip pins
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);

  // TODO: Add support for DMX channels beyond 16
  for (int i = 1; i <= 16; i++) {
    DmxSimple.write(i, 255);
    delay(200);
    DmxSimple.write(i, 0);
  }

  digitalWrite(13, LOW);
}

uint8_t code;
uint8_t op;

operation status;

uint16_t channel = 0;
uint16_t start;
uint8_t length;
uint8_t value;

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
        digitalWrite(13, LOW);
      } else {
        // error
        digitalWrite(13, HIGH);
      }
      return;
    }
  }
  
  if (status == dmx) {
    dmxBuffer[channel++] = code;
    if (channel >= DMX_SIZE) {
      channel = 0;
      digitalWrite(13, HIGH);
    }

    // DMX channels 7-12 are used to power PWM devices, e.g. a LED strip,
    // connected on the Arduino's "analog" output pins.
    if (channel == 7) {
      analogWrite(3, code);
    } else if (channel == 8) {
      analogWrite(5, code);
    } else if (channel == 9) {
      analogWrite(6, code);
    } else if (channel == 10) {
      analogWrite(9, code);
    } else if (channel == 11) {
      analogWrite(10,code);
    } else if (channel == 12) {
      analogWrite(11,code);
    }
  } else {
    // error
   digitalWrite(13, HIGH); 
  }
}
