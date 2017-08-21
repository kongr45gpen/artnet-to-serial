#include <DmxSimple.h>

// Directly use the dmxBuffer variable from DmxSimple.cpp
// so that we don't have to call an expensive function
// 512 times for every DMX update
extern volatile uint8_t dmxBuffer[DMX_SIZE];

enum operation {
  waiting,
  dmx,
  unknown
};

void setup() {
  Serial.begin(230400);
  Serial.print("r&c");
  DmxSimple.maxChannel(DMX_SIZE);
  pinMode(13, OUTPUT); // error LED
  pinMode(4, OUTPUT); // debugging LED
  pinMode(3, OUTPUT); // output pin
  digitalWrite(4, LOW);
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
  Serial.print("Got code ");
  Serial.println((int) code);

  if (code == 125) {
    // Operation code
    while(!Serial.available());
    op = Serial.read();

    Serial.print("Got oper ");
    Serial.println((int) op);
    
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
    Serial.print("Setting ");
    Serial.print((int) channel);
    Serial.print(" to ");
    Serial.println((int) code);
    dmxBuffer[channel++] = code;
  } else {
    // error
   digitalWrite(13, HIGH); 
  }
}
