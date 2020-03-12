#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//radio setup
//set nRF24L01 SPI bus pins(9, 10)
#define CE_PIN                    9
#define CSN_PIN                   10

RF24 radio(CE_PIN, CSN_PIN);

//byte payload[2];
typedef struct{
  unsigned long int core_time;  
}
P_t;
P_t payload;
                                            // Topology
byte addresses[][6] = {"1Node","2Node"};    // Radio pipe addresses for the 2 nodes to communicate.

char szStr[20];

void setup() {
  Serial.begin(9600);
  printf_begin();
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
  radio.startListening();                 // Start listening
  radio.powerUp();
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop() {
  byte pipeNo;
  while(radio.available(&pipeNo)){
    radio.read(&payload, sizeof(payload));
    printf("Got Packet %lu \r\n", payload.core_time);
    radio.writeAckPayload(pipeNo, &payload, sizeof(payload));
    printf("Sent response %lu \r\n", payload.core_time);
//    sprintf( szStr, "%04X\t%ld", payload[0], payload[0] );
//    Serial.print(szStr);
//    Serial.print("\t");
//    Serial.println("");
  }
}
