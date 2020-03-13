#include <Wire.h>
#include <TimerOne.h>

//libs required for radio
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//radio setup
//set nRF24l01 SPI bus pins 9 and 10
#define CE_PIN                        9
#define CSN_PIN                       10

RF24 radio(CE_PIN, CSN_PIN);

struct Payload {
  long int counter = 0;
  unsigned long packet_id = 0;
  unsigned long core_time = 0;
};

byte rssi;
int transmitted_packages = 1, failed_packages;
unsigned long RSSI_timer;

// Topology
byte addresses[][6] = {"1Node", "2Node"};   // Radio pipe addresses for the 2 nodes to communicate.

//IMU setup
#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G         0x00
#define    ACC_FULL_SCALE_4_G         0x08
#define    ACC_FULL_SCALE_8_G         0x10
#define    ACC_FULL_SCALE_16_G        0x18

#define    INTERRUPT_CALLBACK_PIN     8

// This function read Nbytes bytes from I2C device at address Address.
// Put read bytes starting at register Register in the Data array.
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();

  // Read Nbytes
  Wire.requestFrom(Address, Nbytes);
  uint8_t index = 0;
  while (Wire.available())
    Data[index++] = Wire.read();
}

// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}


// Initial time
unsigned long ti;

volatile bool intFlag = false;

// Initializations
void setup()
{
  // Arduino initializations
  Wire.begin();
  Serial.begin(9600);

  //Radio setup
  printf_begin();                             //
  radio.begin();                              //initialize radio
  radio.setAutoAck(true);                        //enable autoACK
  radio.enableAckPayload();                   //allow optional ACK payloads
  radio.setRetries(15, 15);                    // Smallest time between retries, max no. of retries
  radio.setPayloadSize(32);                    // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(addresses[0]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, addresses[1]);     // Open a reading pipe on address 0, pipe 1
  radio.stopListening();
  radio.printDetails();                       // Dump the configuration of the rf unit for debugging
  radio.setPALevel (RF24_PA_LOW);
  radio.setDataRate (RF24_1MBPS);

  int dataSize = sizeof(Payload);
  printf("Size of Payload %d\n", dataSize);
  if (dataSize > 32) {
    printf("!!!*** === Payload size exceeds 32 Bytes === ***!!! \n");
  }

  // Set accelerometers low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS, 29, 0x06);
  // Set gyroscope low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS, 26, 0x06);


  // Configure gyroscope range
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);
  // Configure accelerometers range
  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);
  // Set by pass mode for the magnetometers
  I2CwriteByte(MPU9250_ADDRESS, 0x37, 0x02);

  // Request continuous magnetometer measurements in 16 bits
  I2CwriteByte(MAG_ADDRESS, 0x0A, 0x16);

  pinMode(INTERRUPT_CALLBACK_PIN, OUTPUT);
  Timer1.initialize(10000);         // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt

  // Store initial time
  ti = millis();
}

// Counter
long int cpt = 0;

void callback()
{
  intFlag = true;
  digitalWrite(INTERRUPT_CALLBACK_PIN, digitalRead(INTERRUPT_CALLBACK_PIN) ^ 1);
}

unsigned long packet_id = 0;
// Main loop, read and display data
void loop()
{
  Payload payload; //initialize structure
  Payload payback;

  while (!intFlag);
  intFlag = false;

  // _______________
  // ::: Counter :::
  cpt++;
  // Display data counter
  //  Serial.print (cpt,DEC);
  //  Serial.print ("\t");


  // Display time
  //  Serial.print (millis()-ti,DEC);
  //  Serial.print ("\t");

  payload.core_time = millis() - ti;

  // ____________________________________
  // :::  accelerometer and gyroscope :::

  // Read accelerometer and gyroscope
  //  uint8_t Buf[14];
  //  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);

  // Create 16 bits values from 8 bits data

  // Accelerometer
  //  int16_t ax=-(Buf[0]<<8 | Buf[1]);
  //  int16_t ay=-(Buf[2]<<8 | Buf[3]);
  //  int16_t az=Buf[4]<<8 | Buf[5];

  // Gyroscope
  //  int16_t gx=-(Buf[8]<<8 | Buf[9]);
  //  int16_t gy=-(Buf[10]<<8 | Buf[11]);
  //  int16_t gz=Buf[12]<<8 | Buf[13];

  // Display values

  // Accelerometer
  //  Serial.print (ax,DEC);
  //  Serial.print ("\t");
  //  Serial.print (ay,DEC);
  //  Serial.print ("\t");
  //  Serial.print (az,DEC);
  //  Serial.print ("\t");

  // Gyroscope
  //  Serial.print (gx,DEC);
  //  Serial.print ("\t");
  //  Serial.print (gy,DEC);
  //  Serial.print ("\t");
  //  Serial.print (gz,DEC);
  //  Serial.print ("\t");

  // _____________________
  // :::  Magnetometer :::


  // Read register Status 1 and wait for the DRDY: Data Ready

  //  uint8_t ST1;
  //  do
  //  {
  //    I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  //  }
  //  while (!(ST1&0x01));

  // Read magnetometer data
  //  uint8_t Mag[7];
  //  I2Cread(MAG_ADDRESS,0x03,7,Mag);


  // Create 16 bits values from 8 bits data

  // Magnetometer
  //  int16_t mx=-(Mag[3]<<8 | Mag[2]);
  //  int16_t my=-(Mag[1]<<8 | Mag[0]);
  //  int16_t mz=-(Mag[5]<<8 | Mag[4]);


  // Magnetometer
  //  Serial.print (mx+200,DEC);
  //  Serial.print ("\t");
  //  Serial.print (my-70,DEC);
  //  Serial.print ("\t");
  //  Serial.print (mz-700,DEC);
  //  Serial.print ("\t");


  //send data
  radio.powerUp();
  radio.stopListening();

  payload.packet_id = packet_id;
  //  printf("Now sending payload %ul \r\n", ti);
  printf("Now sending payload %ld with time %ld \r\n", payload.packet_id, payload.core_time);
  radio.openWritingPipe(addresses[0]);       // Open different pipes when writing. Write on pipe 0, address 0
  radio.openReadingPipe(1, addresses[1]);    // Read on pipe 1, as address 1

  if (radio.write(&payload, sizeof(Payload))) {
    packet_id ++;
    if (!radio.available()) {
      printf("Radio unavailable \r\n");
    } else {
      while (radio.available()) {
        radio.read(&payback, sizeof(Payload));
        printf("Got response %ld \r\n", payback.core_time);
      }
    }
  } else {
    failed_packages++;
    printf("Failed packages %d \r\n", failed_packages);
  }
  radio.powerDown();



  // End of line
  //  Serial.println("");
  //debug delay
  delay(1000);
}










