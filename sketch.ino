#include <DmxSimple.h>

// Directly use the dmxBuffer variable from DmxSimple.cpp
// so that we don't have to call an expensive function
// 512 times for every DMX update
extern volatile uint8_t dmxBuffer[DMX_SIZE];

void setup() {
  Serial.begin(230400);
  Serial.print("r&c");
  DmxSimple.maxChannel(DMX_SIZE);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  pinMode(4, OUTPUT); // debugging LED
  pinMode(3, OUTPUT); // output pin
  digitalWrite(4, LOW);
}

uint8_t op;

uint8_t channel;
uint16_t start;
uint8_t length;
uint8_t value;

void loop() {
  digitalWrite(4, LOW);

  // Read operation
  while(!Serial.available());
  digitalWrite(4, HIGH);
  op = Serial.read();

  if ('f' == op) {
    // Full 512-channel package
    while(Serial.available() < 512);
    Serial.readBytes((uint8_t*) dmxBuffer, 512);
  } else if ('o' == op) {
    // One channel (for testing purposes)
    while(!Serial.available());
    channel = Serial.read();
    while(!Serial.available());
    dmxBuffer[channel] = Serial.read();
  } else if ('m' == op) {
    // Many channels (at most 256)
    while(Serial.available() < 3);
    Serial.readBytes((uint8_t*) (&start), 2);
    length = Serial.read();
    if (start >= 512 || length + start >= 512) {
      Serial.write('e'); // error
      return;
    }
    while (Serial.available() < length);
    Serial.readBytes((uint8_t*) (dmxBuffer+start), length);
  } else if ('p' == op) {
    // Ping
    Serial.write('p'); // pong
  } 
    // ===== LEAST COMMON OPERATIONS =====
  else if ('r' == op) {
    // Reset
    for (start = 0; start != 512; ++start) {
      dmxBuffer[start] = 0;
    }
  } else if ('a' == op) {
    // All channels to value
    while(!Serial.available());
    value = Serial.read();
    for (start = 0; start != 512; ++start) {
      dmxBuffer[start] = value;
    }
  } else {
    Serial.write('e'); // error
  }
}
