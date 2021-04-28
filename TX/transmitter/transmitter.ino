#include "MPU9250.h"
#include <TimerOne.h>
#include "quaternionFilters.h"

//basic config
#define Debug             false //general debug
#define SerialDebug       true //serial debug messaging
#define RadioEnabled      true //enable nRF24l01+ radio communication module
#define AHRS              true //calculate AHRS on board and send Euler angles  
                               //or send sensor values only
#ifdef AHRS
  #define MAHONY          false //select filter engine
  #define MADGWICK        true  //
#endif
#define I2Cclock 400000
#define I2Cport Wire
#define MPU9250_ADDRESS MPU9250_ADDRESS_AD0   // Use either this line or the next to select which I2C address your device is using
//#define MPU9250_ADDRESS MPU9250_ADDRESS_AD1

//calculation setup
#ifdef AHRS
  struct Payload {
    long int      counter     = 0;  //system counter
    unsigned long packet_id   = 0;  //packet ID
    unsigned long core_time   = 0;  //sensor unit core time  
//    float         temperature = 0;  //core temp
    float         yaw         = 0;  //
    float         pitch       = 0;  //
    float         roll        = 0;  //
  };
#else    
  struct Payload {
    long int      counter     = 0; //system counter
    unsigned long packet_id   = 0; //packet ID
    unsigned long core_time   = 0; //sensor unit core time
//    float         temperature = 0; //core temperature  
    int16_t       ax          = 0; //acceleration by X axis
    int16_t       ay          = 0; //acceleration by Y axis
    int16_t       az          = 0; //acceleration by Z axis  
    int16_t       gx          = 0; //orientation by X axis
    int16_t       gy          = 0; //orientation by Y axis
    int16_t       gz          = 0; //orientation by Z axis
    int16_t       mx          = 0; //magnetic inclination by X axis
    int16_t       my          = 0; //magnetic inclination by Y axis
    int16_t       mz          = 0; //magnetic inclination by Z axis
  };
#endif

//radio routine setup
#ifdef RadioEnabled           //Libs required for radio. SPI.h already included
  #include "nRF24L01.h"       //inside MPU9250.h
  #include "RF24.h"
  #include "printf.h"         //nRF24 debug helper lib

  //set nRF24l01 SPI bus pins 9 and 10
  #define CE_PIN         9
  #define CSN_PIN        10
  
  RF24 radio(CE_PIN, CSN_PIN);  
  byte rssi;
  int transmitted_packages = 1, failed_packages;
  unsigned long RSSI_timer;
  // Topology
  byte addresses[][6] = {"1Node", "2Node"};   // Radio pipe addresses for the 2 nodes to communicate.
#endif

//Pin definitions
int intPin              = 2;  //Arduino ext int pins
int myLed               = 13; //Assign led pin for indicate work

MPU9250 myIMU(MPU9250_ADDRESS, I2Cport, I2Cclock);

// Initial time
unsigned long ti;
volatile bool intFlag = false;

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

// Counter
long int cpt = 0;

void callback()
{
  intFlag = true;
  digitalWrite(intPin, digitalRead(intPin) ^ 1);
}

unsigned long packet_id = 0;
// Main loop, read and display data

//Initialization
void setup(){
  // Arduino initializations
  Wire.begin();
  TWBR = 12;  // 400 kbit/sec I2C speed
  Serial.begin(9600);

  //Radio setup
  if(RadioEnabled){
    if(SerialDebug){
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
      radio.setPALevel (RF24_PA_HIGH);             // Can be RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
      radio.setDataRate (RF24_1MBPS);
    }
    int dataSize = sizeof(Payload);

    if (dataSize > 32) {
      printf("!!!*** === Payload size exceeds 32 Bytes === ***!!! \n");
    }
  }
  
  //Wait for serial if debug enabled
  if(SerialDebug){
    while(!Serial){};
  }

  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH);

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print(F("MPU9250 I AM 0x"));
  Serial.print(c, HEX);
  Serial.print(F(" I should be 0x"));
  Serial.println(0x71, HEX);

  if (c == 0x71)
  {
    Serial.println(F("MPU9250 is online..."));

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.selfTest);
    
    if(SerialDebug){
      //Accelerometer report
      Serial.print(F("x-axis self test: acceleration trim within : "));
      Serial.print(myIMU.selfTest[0],1); 
      Serial.println("% of factory value");
      Serial.print(F("y-axis self test: acceleration trim within : "));
      Serial.print(myIMU.selfTest[1],1); 
      Serial.println("% of factory value");
      Serial.print(F("z-axis self test: acceleration trim within : "));
      Serial.print(myIMU.selfTest[2],1); 
      Serial.println("% of factory value");
      //Gyroscope report
      Serial.print(F("x-axis self test: gyration trim within : "));
      Serial.print(myIMU.selfTest[3],1); 
      Serial.println("% of factory value");
      Serial.print(F("y-axis self test: gyration trim within : "));
      Serial.print(myIMU.selfTest[4],1); 
      Serial.println("% of factory value");
      Serial.print(F("z-axis self test: gyration trim within : "));
      Serial.print(myIMU.selfTest[5],1); 
      Serial.println("% of factory value");
    }
    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    Serial.println("MPU9250 initialized for active data mode....");

    // Set accelerometers low pass filter at 5Hz
    I2CwriteByte(MPU9250_ADDRESS, 29, 0x06);
    // Set gyroscope low pass filter at 5Hz
    I2CwriteByte(MPU9250_ADDRESS, 26, 0x06);
  
    // Configure gyroscope range
//    I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);
    // Configure accelerometers range
//    I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);
    // Set by pass mode for the magnetometers
    I2CwriteByte(MPU9250_ADDRESS, 0x37, 0x02);

    // Request continuous magnetometer measurements in 16 bits
//    I2CwriteByte(MAG_ADDRESS, 0x0A, 0x16);

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("AK8963 ");
    Serial.print("I AM 0x");
    Serial.print(d, HEX);
    Serial.print(" I should be 0x");
    Serial.println(0x48, HEX);

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

    // Get magnetometer calibration from AK8963 ROM
    myIMU.initAK8963(myIMU.factoryMagCalibration);
    // Initialize device for active mode read of magnetometer
    Serial.println("AK8963 initialized for active data mode....");

    if (SerialDebug)
    {
      //  Serial.println("Calibration values: ");
      Serial.print("X-Axis factory sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[0], 2);
      Serial.print("Y-Axis factory sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[1], 2);
      Serial.print("Z-Axis factory sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[2], 2);
    }

    // Get sensor resolutions, only need to do this once
    myIMU.getAres();
    myIMU.getGres();
    myIMU.getMres();

    //Manual sensor-specific calibration settings
    myIMU.magBias[0] = 50.675003;
    myIMU.magBias[1] = -279.615;
    myIMU.magBias[2] = -493.85;

    myIMU.magScale[0] = 1.0603297;
    myIMU.magScale[1] = 1.0646136;
    myIMU.magScale[2] = 0.894783;

    // The next call delays for 4 seconds, and then records about 15 seconds of
    // data to calculate bias and scale.
//    myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
    //    stand IMU

  pinMode(intPin, OUTPUT);
  Timer1.initialize(10000);         // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt

  // Store initial time
  ti = millis();

    if(SerialDebug){
      Serial.println("AK8963 mag biases (mG)");
      Serial.println(myIMU.magBias[0]);
      Serial.println(myIMU.magBias[1]);
      Serial.println(myIMU.magBias[2]);
  
      Serial.println("AK8963 mag scale (mG)");
      Serial.println(myIMU.magScale[0]);
      Serial.println(myIMU.magScale[1]);
      Serial.println(myIMU.magScale[2]);
      
      Serial.println("Magnetometer:");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(myIMU.factoryMagCalibration[2], 2);      
    }    
  }else{
    Serial.print("Could not connect to MPU9250: 0x");
    Serial.println(c, HEX);

    //Communication to MPU9250 failed
    Serial.println(F("Communication failed. Abort!"));
    Serial.flush();
    abort();
  }  
}


void loop()
{
  Payload payload; //initialize structure
  Payload payback;

  while (!intFlag);
  intFlag = false;
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes - myIMU.accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes - myIMU.accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes - myIMU.accelBias[2];

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  myIMU.updateTime();

#ifdef AHRS
    // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
#ifdef MAHONY
    MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, -myIMU.mz, myIMU.deltat);
#endif
#ifdef MADGWICK    
    MadgwickQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, -myIMU.mz, myIMU.deltat);
#endif

    myIMU.tempCount = myIMU.readTempData();  // Read the adc values
    // Temperature in degrees Centigrade
    myIMU.temperature = (myIMU.tempCount) / 333.87 + 21.0;

    //payload pre-assembly
    payload.counter     = cpt;
    payload.packet_id   = packet_id;
    payload.core_time   = millis() - ti;
//    payload.temperature = myIMU.temperature;
  

      myIMU.delt_t = millis() - myIMU.count;
// Define output variables from updated quaternion---these are Tait-Bryan
// angles, commonly used in aircraft orientation. In this coordinate system,
// the positive z-axis is down toward Earth. Yaw is the angle between Sensor
// x-axis and Earth magnetic North (or true North if corrected for local
// declination, looking down on the sensor positive yaw is counterclockwise.
// Pitch is angle between sensor x-axis and Earth ground plane, toward the
// Earth is positive, up toward the sky is negative. Roll is angle between
// sensor y-axis and Earth ground plane, y-axis up is positive roll. These
// arise from the definition of the homogeneous rotation matrix constructed
// from quaternions. Tait-Bryan angles as well as Euler angles are
// non-commutative; that is, the get the correct orientation the rotations
// must be applied in the correct order which for this configuration is yaw,
// pitch, and then roll.
// For more see
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// which has additional links.
      myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ()
                    * *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ()
                    * *(getQ()+2)));
      myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2)
                    * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw   *= RAD_TO_DEG;

      // Model Used: WMM-2020  Dnipro
      // Latitude: 48° 28' 30" N
      // Longitude:  35° 0' 57" E
      // Date  Declination
      // 2020-11-16  8° 13' E  ± 0° 23'  changing by  0° 6' E per year
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      myIMU.yaw  -= 8.13;
      myIMU.roll *= RAD_TO_DEG;

      //payload assembly
      payload.yaw         = myIMU.yaw;
      payload.pitch       = myIMU.pitch;
      payload.roll        = myIMU.roll;
            
      if(SerialDebug){
        Serial.print(payload.counter);
        Serial.print("\t");
        Serial.print(payload.packet_id);
        Serial.print("\t");
        Serial.print(payload.core_time);
//        Serial.print("\t");
//        Serial.print(payload.temperature);
        Serial.print("\t");
        Serial.print(payload.yaw, 2);
        Serial.print("\t");
        Serial.print(payload.pitch, 2);
        Serial.print("\t");
        Serial.println(payload.roll, 2);
      }
      
      myIMU.sumCount = 0;
      myIMU.sum = 0;
#else
      //set accelerometer values in milligs per second!
      payload.ax = 1000 * myIMU.ax;
      payload.ay = 1000 * myIMU.ay;
      payload.az = 1000 * myIMU.az;
      //set gyroscope values in degrees per second
      payload.gx = 1000 * myIMU.gx;
      payload.gy = 1000 * myIMU.gy;
      payload.gz = 1000 * myIMU.gz;
      //set magnetometer values in degrees per second
      payload.mx = myIMU.mx;
      payload.my = myIMU.my;
      payload.mz = myIMU.mz;

      if(SerialDebug){
        Serial.print(payload.counter);
        Serial.print("\t");
        Serial.print(payload.packet_id);
        Serial.print("\t");
        Serial.print(payload.core_time);
        Serial.print("\t");
//        Serial.print(payload.temperature);
//        Serial.print("\t");
        Serial.print(payload.ax);
        Serial.print("\t");
        Serial.print(payload.ay);
        Serial.print("\t");
        Serial.print(payload.az);
        Serial.print("\t");
        Serial.print(payload.gx);
        Serial.print("\t");
        Serial.print(payload.gy);
        Serial.print("\t");
        Serial.print(payload.gz);
        Serial.print("\t");
        Serial.print(payload.mx);
        Serial.print("\t");
        Serial.print(payload.my);
        Serial.print("\t");
        Serial.println(payload.mz);
      }
#endif

      myIMU.count = millis();
      digitalWrite(myLed, !digitalRead(myLed));  // toggle led

  //Counter
  cpt++;

  if(RadioEnabled){
    //send data
    radio.powerUp();
    radio.stopListening();
  
//    printf("Now sending payload %ul \r\n", ti);
//  printf("Now sending payload %ld with time %ld \r\n", payload.packet_id, payload.core_time);
    radio.openWritingPipe(addresses[0]);       // Open different pipes when writing. Write on pipe 0, address 0
    radio.openReadingPipe(1, addresses[1]);    // Read on pipe 1, as address 1

    if (radio.write(&payload, sizeof(Payload))) {
      packet_id ++;
      if (!radio.available()) {
        printf("Radio unavailable \r\n");
      } else {
        while (radio.available()) {
          radio.read(&payback, sizeof(Payload));
  //        printf("Got response %ld \r\n", payback.core_time);
        }
      }
    } else {
      failed_packages++;
  //    printf("Failed packages %d \r\n", failed_packages);
    }
    radio.powerDown();
  }
//  Debug delay
  if(SerialDebug && Debug){
    delay(500);
  }
}










