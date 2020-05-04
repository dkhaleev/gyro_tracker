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
  long int counter = 0;         //system counter
  unsigned long packet_id = 0;  //packet ID
  unsigned long core_time = 0;  //sensor unit core time  
  int16_t ax = 0;               //acceleration by X axis
  int16_t ay = 0;               //acceleration by Y axis
  int16_t az = 0;               //acceleration by Z axis  
  int16_t gx = 0;               //orientation by X axis
  int16_t gy = 0;               //orientation by Y axis
  int16_t gz = 0;               //orientation by Z axis
  int16_t mx = 0;               //magnetic inclination by X axis
  int16_t my = 0;               //magnetic inclination by Y axis
  int16_t mz = 0;               //magnetic inclination by Z axis
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
  radio.setPALevel (RF24_PA_HIGH);         //Can be RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
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

//    printf("Got Packet %ld with time %ld \r\n", payback.packet_id, payback.core_time);
 //Time section
 Serial.print(payload.packet_id);
 Serial.print("\t");
 Serial.print(payload.core_time);
 Serial.print("\t");
 //Accelerometer debug section
 Serial.print(payload.ax, DEC);
 Serial.print("\t");
 Serial.print(payload.ay, DEC);
 Serial.print("\t");
 Serial.print(payload.az, DEC);
 Serial.print("\t");
 //Gyroscope section
 Serial.print(payload.gx, DEC);
 Serial.print("\t");
 Serial.print(payload.gy, DEC);
 Serial.print("\t");
 Serial.print(payload.gz, DEC);
 Serial.print("\t");
 //Magnetometer section
Serial.print (payload.mx+200,DEC);
Serial.print ("\t");
Serial.print (payload.my-70,DEC);
Serial.print ("\t");
Serial.print (payload.mz-700,DEC);
Serial.print ("\t");
 Serial.print("\r\n");
 
    radio.writeAckPayload(pipeNo, &payload, sizeof(Payload));
//    printf("Sent response packet %ld with time %ld \r\n", payback.packet_id, payback.core_time);
  }
}
