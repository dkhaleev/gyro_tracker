#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//radio setup
//set nRF24L01 SPI bus pins(9, 10)
#define CE_PIN                    9
#define CSN_PIN                   10

RF24 radio(CE_PIN, CSN_PIN);

unsigned long packet_id = 0;
unsigned long core_time = 0;

struct Payload {
  long int counter = 0;
  unsigned long packet_id = 0;
  unsigned long core_time = 0;
};

// Topology
byte addresses[][6] = {"1Node", "2Node"};   // Radio pipe addresses for the 2 nodes to communicate.

char szStr[20];

void setup() {
  Serial.begin(9600);
  printf_begin();
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(15, 15);                // Smallest time between retries, max no. of retries
  radio.setPayloadSize(32);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, addresses[0]);     // Open a reading pipe on address 0, pipe 1
  radio.startListening();                 // Start listening
  radio.powerUp();
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  radio.setPALevel (RF24_PA_LOW);
  radio.setDataRate (RF24_1MBPS);
}

void loop() {
  byte pipeNo;
  Payload payload;
  Payload payback;
  while (radio.available(&pipeNo)) {
    radio.read(&payload, sizeof(Payload));

    packet_id = payload.packet_id;
    payback.packet_id = packet_id;
    core_time = payload.core_time;
    payback.core_time = core_time;

    printf("Got Packet %ld with time %ld \r\n", payback.packet_id, payback.core_time);
    radio.writeAckPayload(pipeNo, &payload, sizeof(Payload));
    printf("Sent response packet %ld with time %ld \r\n", payback.packet_id, payback.core_time);
  }
}
