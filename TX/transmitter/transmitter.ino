#include <Wire.h>
#include <TimerOne.h>
#include "MPU9250.h"

//basic config
#define Debug             true //general debug
#define SerialDebug       true //serial debug messaging
#define RadioEnabled      true //enable nRF24l01+ radio communication module

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

byte rssi;
int transmitted_packages = 1, failed_packages;
unsigned long RSSI_timer;

// Topology
byte addresses[][6] = {"1Node", "2Node"};   // Radio pipe addresses for the 2 nodes to communicate.

//IMU setup
#define    I2Cclock 400000
#define    I2Cport Wire


// Use either this line or the next to select which I2C address your device 
//is using
#define    MPU9250_ADDRESS            MPU9250_ADDRESS_AD0
//#define    MPU9250_ADDRESS            MPU9250_ADDRESS_AD1

MPU9250 IMU(MPU9250_ADDRESS, I2Cport, I2Cclock);

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
  Serial.begin(115200);
  
  //Wait for serial
  while(!Serial){};

  //Set up the interrupt pin. It's set as active high, push-pull
  
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
//  radio.printDetails();                       // Dump the configuration of the rf unit for debugging
  radio.setPALevel (RF24_PA_HIGH);             // Can be RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS);

  int dataSize = sizeof(Payload);

  if (dataSize > 32) {
    printf("!!!*** === Payload size exceeds 32 Bytes === ***!!! \n");
  }

  //Read the WHO_AM_I register. Test communication with sensor
  byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print(F("MPU9250 I AM 0x"));
  Serial.print(c, HEX);
  Serial.print(F(" I should be 0x"));
  Serial.println(0x71, HEX);

  if(c == 0x71)// WHO_AM_I register should always be 0x71
  {
    Serial.println(F("MPU9250 is online..."));

    //Start self testing procedures
    IMU.MPU9250SelfTest(IMU.selfTest);
    if(SerialDebug)
    {
      //Accelerator report
      Serial.print(F("X-axis self test: acceleration trim within: "));
      Serial.print(IMU.selfTest[0],1);
      Serial.println("% of factory value");
      Serial.print(F("Y-axis self test: acceleration trim within: "));
      Serial.print(IMU.selfTest[1],1);
      Serial.println("% of factory value");
      Serial.print(F("Z-axis self test: acceleration trim within: "));
      Serial.print(IMU.selfTest[2],1);
      Serial.println("% of factory value");
      //Gyroscope report
      Serial.print(F("X-axis self test: gyration trim within: "));
      Serial.print(IMU.selfTest[3],1);
      Serial.println("% of factory value");
      Serial.print(F("X-axis self test: gyration trim within: "));
      Serial.print(IMU.selfTest[4],1);
      Serial.println("% of factory value");
      Serial.print(F("X-axis self test: gyration trim within: "));
      Serial.print(IMU.selfTest[5],1);
      Serial.println("% of factory value");
    }

    //Calibrate accelerometer and gyro, load biases in bias registers
    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);

    //Initialize active mode and read accel, gyro and temperature
    IMU.initMPU9250();
    if(SerialDebug){
      Serial.println("MPU925 initialized for active mode");
    }

    //For test communication with magnetometer read the WHO_AM_I register
    byte d = IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    if(SerialDebug){
      Serial.print("AK8963 ");
      Serial.print("I AM 0x");
      Serial.print(d, HEX);
      Serial.print(" I should be 0x");
      Serial.println(0x48, HEX);
    }    

    if(d != 0x48){
      //Magnetometer communication failed. Abort
      if(SerialDebug){
        Serial.print("Could not connect to AK8963: 0x");
        Serial.print(d, HEX);        
        Serial.println(F("Magnetometer communication failed"));
        Serial.flush();
      }
      abort();
    }

    //Get magnetometer calibration from ROM
    IMU.initAK8963(IMU.factoryMagCalibration);
    if(SerialDebug){
      Serial.println("AK8963 is online");
      Serial.print(F("X-axis factory sensitivity adjusment value: "));
      Serial.println(IMU.factoryMagCalibration[0],2);
      Serial.print(F("Y-axis factory sensitivity adjusment value: "));
      Serial.println(IMU.factoryMagCalibration[1],2);
      Serial.print(F("Z-axis factory sensitivity adjusment value: "));
      Serial.println(IMU.factoryMagCalibration[2],2);
    }

    //get sensor resolutions
    IMU.getAres();
    IMU.getGres();
    IMU.getMres();

    IMU.magCalMPU9250(IMU.magBias, IMU.magScale);

    if(SerialDebug){
      Serial.println("AK8963 mag biases (mG):");
      Serial.println(IMU.magBias[0]);
      Serial.println(IMU.magBias[1]);
      Serial.println(IMU.magBias[2]);

      Serial.println("AK8963 mag scale (mG):");
      Serial.println(IMU.magScale[0]);
      Serial.println(IMU.magScale[1]);
      Serial.println(IMU.magScale[2]);

      Serial.println("Magnetometer:");
      Serial.print("X-axis sensitivity adjustment value: ");
      Serial.println(IMU.factoryMagCalibration[0], 2);
      Serial.print("Y-axis sensitivity adjustment value: ");
      Serial.println(IMU.factoryMagCalibration[1], 2);
      Serial.print("Z-axis sensitivity adjustment value: ");
      Serial.println(IMU.factoryMagCalibration[2], 2);
    }    
  }else{
    if(SerialDebug){
      //communication failed
      Serial.print("Could not connect to MPU9250: 0x");
      Serial.println(c, HEX);
      Serial.println(F("Accelerometer communication failed. Abort!"));
      Serial.flush();
    }
    abort();
  }
  

//  // Set accelerometers low pass filter at 5Hz
//  I2CwriteByte(MPU9250_ADDRESS, 29, 0x06);
//  // Set gyroscope low pass filter at 5Hz
//  I2CwriteByte(MPU9250_ADDRESS, 26, 0x06);
//
//
//  // Configure gyroscope range
//  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);
//  // Configure accelerometers range
//  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);
//  // Set by pass mode for the magnetometers
//  I2CwriteByte(MPU9250_ADDRESS, 0x37, 0x02);
//
//  // Request continuous magnetometer measurements in 16 bits
//  I2CwriteByte(MAG_ADDRESS, 0x0A, 0x16);

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
    // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes - IMU.accelBias[0];
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes - IMU.accelBias[1];
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes - IMU.accelBias[2];

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

    IMU.readMagData(IMU.magCount);  // Read the x/y/z adc values

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU.mx = (float)IMU.magCount[0] * IMU.mRes
               * IMU.factoryMagCalibration[0] - IMU.magBias[0];
    IMU.my = (float)IMU.magCount[1] * IMU.mRes
               * IMU.factoryMagCalibration[1] - IMU.magBias[1];
    IMU.mz = (float)IMU.magCount[2] * IMU.mRes
               * IMU.factoryMagCalibration[2] - IMU.magBias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  IMU.updateTime();

  IMU.tempCount = IMU.readTempData();  // Read the adc values
  // Temperature in degrees Centigrade
  IMU.temperature = ((float) IMU.tempCount) / 333.87 + 21.0;
  if(SerialDebug){
    // Print temperature in degrees Centigrade
    Serial.print("Temperature is ");  
    Serial.print(IMU.temperature, 1);
    Serial.println(" degrees C");
  }

  // _______________
  // ::: Counter :::
  cpt++;
  // Display data counter
//    Serial.print (payload.counter,DEC);
//    Serial.print ("\t");


  // Display time
    payload.core_time = millis() - ti;
    payload.packet_id = packet_id;
  //  Serial.print (millis()-ti,DEC);
//    Serial.print(payload.packet_id);
//    Serial.print("\t");
//    Serial.print(payload.core_time);
//    Serial.print ("\t");

  // ____________________________________
  // :::  accelerometer and gyroscope :::

  // Read accelerometer and gyroscope
    uint8_t Buf[14];
    I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);

  // Create 16 bits values from 8 bits data

  // Accelerometer
    int16_t ax=-(Buf[0]<<8 | Buf[1]);
    payload.ax = ax;
    int16_t ay=-(Buf[2]<<8 | Buf[3]);
    payload.ay = ay;
    int16_t az=Buf[4]<<8 | Buf[5];
    payload.az = az;

  // Gyroscope
    int16_t gx=-(Buf[8]<<8 | Buf[9]);
    payload.gx = gx;
    int16_t gy=-(Buf[10]<<8 | Buf[11]);
    payload.gy = gy;
    int16_t gz=Buf[12]<<8 | Buf[13];
    payload.gz = gz;
  // Display values

  // Accelerometer
//    Serial.print (payload.ax,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.ay,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.az,DEC);
//    Serial.print ("\t");

  // Gyroscope
//    Serial.print (payload.gx,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.gy,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.gz,DEC);
//    Serial.print ("\t");    

  // _____________________
  // :::  Magnetometer :::


  // Read register Status 1 and wait for the DRDY: Data Ready

    uint8_t ST1;
    do
    {
      I2Cread(MAG_ADDRESS,0x02,1,&ST1);
    }
    while (!(ST1&0x01));

  // Read magnetometer data
    uint8_t Mag[7];
    I2Cread(MAG_ADDRESS,0x03,7,Mag);


  // Create 16 bits values from 8 bits data

  // Magnetometer
    int16_t mx=-(Mag[1]<<8 | Mag[0]);
    payload.mx = mx;
    int16_t my=-(Mag[3]<<8 | Mag[2]);
    payload.my = my;
    int16_t mz=-(Mag[5]<<8 | Mag[4]);
    payload.mz = mz;

  // Magnetometer
//    Serial.print (payload.mx+200,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.my-70,DEC);
//    Serial.print ("\t");
//    Serial.print (payload.mz-700,DEC);
//    Serial.print ("\t");

  //send data
//  radio.powerUp();
//  radio.stopListening();

//  printf("\r\n");
  
//  //  printf("Now sending payload %ul \r\n", ti);
////  printf("Now sending payload %ld with time %ld \r\n", payload.packet_id, payload.core_time);
//  radio.openWritingPipe(addresses[0]);       // Open different pipes when writing. Write on pipe 0, address 0
//  radio.openReadingPipe(1, addresses[1]);    // Read on pipe 1, as address 1
//
//  if (radio.write(&payload, sizeof(Payload))) {
//    packet_id ++;
//    if (!radio.available()) {
//      printf("Radio unavailable \r\n");
//    } else {
//      while (radio.available()) {
//        radio.read(&payback, sizeof(Payload));
////        printf("Got response %ld \r\n", payback.core_time);
//      }
//    }
//  } else {
//    failed_packages++;
////    printf("Failed packages %d \r\n", failed_packages);
//  }
////  radio.powerDown();



  // End of line
//    Serial.println("");
  //debug delay
//  delay(500);
}










