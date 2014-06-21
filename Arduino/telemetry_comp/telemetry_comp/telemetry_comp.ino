
#include <Servo.h>
#include <Wire.h>
#include <ArduTalk.h>

//#define DEBUG

#define YAW   0
#define PITCH 1 
#define ROLL  2

#define LAT 0
#define LON 1
#define ALT 2

#define ULTRA_SONIC 0
#define PI_POLL_DST2GND 0x01

#define MSG_TYPE_ROTORS 0
#define MSG_TYPE_ORI    1
#define MSG_TYPE_LOC    2

typedef byte  OrientationStates[3];
typedef byte  RotorStates[4];
typedef float LocationStates[3];

typedef struct{
  OrientationStates orientation; // yaw, pitch, roll
  LocationStates position;    // gps coordinates for location
} Location; // 24 bytes

typedef struct{
  unsigned char override;
  RotorStates rotors; // speed values for rotors
} BasStnMsg;

typedef struct{
  Location loc;
  RotorStates rotorVals;
  float distToGround;
} StateMsg;

Servo rotors[4];
RotorStates rotorTarget;
RotorStates rotorCurrent;
int manualOverride     = 0;
float lastDistToGround = 0.0f;
unsigned long lastFltCmpContact, lastTime;
StateMsg relay = { 0 };

ArduTalk Radio(&Serial, 115200);
//-------------------------------------------------------------------------------
void setup(){
  // hook up the servos, and set their speeds to 0
  for(int i = 4; i--;){
    rotors[i].attach(4 + i);
    rotors[i].write(0);
    
    rotorTarget[i] = rotorCurrent[i] = 0;
  }
  
  // setup I2C comms, join as slave
  Wire.begin(0x03);
  Wire.onReceive(I2Creceived);
  Wire.onRequest(I2Crequested);
  
  // so fast...
  Serial.begin(115200);
#ifdef DEBUG
  Serial.println("Oh hey");
#endif

  lastTime = lastFltCmpContact = millis();
}
int cmToGround(){
  return analogRead(ULTRA_SONIC); 
}
//-------------------------------------------------------------------------------
byte __chksum(byte* data, int size){
  byte sum = 0;
  for(;size--;){
    sum ^= data[size];
  }
  
  return sum;
}
//-------------------------------------------------------------------------------
int I2C_TRANSMIT(void* data, int size){
  byte buf[size + 1];
  memcpy(buf, data, size);
  buf[size] = __chksum((byte*)data, size);
  /*Serial.println*/(Wire.write(buf, size + 1));

  return 0;
}

int I2C_RECEIVE(void* data, int size){
  Wire.readBytes((char*)data, size);
  if(Wire.read() != __chksum((byte*)data, size)){
    Serial.println("BC");
    Wire.flush();
    return -1;
  }
  
  return 0;
}

byte MsgSizes[3] = { sizeof(RotorStates), sizeof(OrientationStates), sizeof(LocationStates) };
byte ReadMode = 0xFF;

void I2Creceived(int bytes){
  //FltCmpMsg msg  = { 0 };
  static const StateMsg empty = { 0 };
//Serial.print("Bytes "); Serial.println(bytes, DEC);
    LocationStates    posStates = {0};
    OrientationStates oriStates = {0};
    RotorStates    targusTargus = {0};
#ifdef DEBUG 
Serial.println("vvvvvv");
#endif

  if(ReadMode == 0xFF){
    // read the message type to determine how many bytes will be read next
    ReadMode = Wire.read();
#ifdef DEBUG
    Serial.print("Read "); Serial.println(ReadMode, DEC);
#endif
  }
  // if the correct amount of data is available
  // read the message type
  if(Wire.available() >= MsgSizes[ReadMode]){
    byte msgSize = MsgSizes[ReadMode];
    
    switch(ReadMode){
      case MSG_TYPE_ROTORS:
#ifdef DEBUG
          Serial.println("Rotor msg");
#endif
          if(I2C_RECEIVE(targusTargus, msgSize) >= 0){
            memcpy(rotorTarget, (char*)targusTargus, msgSize);
            memcpy(relay.rotorVals, (char*)targusTargus,  msgSize);
          }
        break;
      case MSG_TYPE_ORI:
#ifdef DEBUG
          Serial.println("Orientation msg");
#endif
          I2C_RECEIVE(oriStates, msgSize);
          memcpy(relay.loc.orientation, oriStates, msgSize);
        break;
      case MSG_TYPE_LOC:
#ifdef DEBUG
          Serial.println("Location msg");
#endif
          I2C_RECEIVE(posStates, msgSize);
          memcpy(relay.loc.position, posStates, msgSize);
        break;
    }
    
    // done reading clear the read mode
#ifdef DEBUG
    Serial.println("Reset");
#endif
    ReadMode = 0xFF;
  }      
 
  // see if the flight computer asked for the distance to ground
  relay.distToGround = cmToGround() / 100.0;
#ifdef DEBUG
Serial.println("Done!");
#endif
}
//-------------------------------------------------------------------------------
void I2Crequested(){
#ifdef DEBUG
  Serial.println("^^^^^");
  Serial.println("Req");
  Serial.println(ReadMode, DEC);
#endif
  if(ReadMode != 0xFF){
    switch(ReadMode){
      case MSG_TYPE_ROTORS:
          I2C_TRANSMIT(relay.rotorVals, sizeof(RotorStates));
          //Wire.write((byte*)relay.rotorVals, sizeof(RotorStates));
          //Wire.write(__chksum((byte*)relay.rotorVals, sizeof(RotorStates)));
#ifdef DEBUG
          for(int i = 0; i < sizeof(RotorStates); i++){
            Serial.print(relay.rotorVals[i], HEX); Serial.print(" "); 
          }Serial.println();
          Serial.print("{{");
          Serial.write(relay.rotorVals, sizeof(RotorStates));
          Serial.print("}}");
          Serial.println();
#endif
        break;
      case MSG_TYPE_ORI:
          I2C_TRANSMIT(relay.loc.orientation, sizeof(OrientationStates));
        break;
      case MSG_TYPE_LOC:
          I2C_TRANSMIT((byte*)relay.loc.position, sizeof(LocationStates));
        break;
    }
    
    ReadMode = 0xFF;
  }
}
//-------------------------------------------------------------------------------
unsigned long timeLanding = 0;
unsigned char lastRotorVals[4];
int emergencylastDistToGround;

void emergencyLanding(unsigned long dt){
  float lRatio = timeLanding / 10000.0f;
  float tRatio = (timeLanding += dt) / 10000.0f;
  int distToGround = cmToGround();
  int delta = distToGround - emergencylastDistToGround;
  float vz = (delta / 100.0) * (dt / 1000.0);
  
  // if we are decending faster than 0.25 m/s and we are within the measurement
  // range of the ultrasonic sensor, then make a modification to the throttle target
  if(vz < -0.25f && distToGround < 1000){
    
  }
  
  // power down slowly
  for(int i = 4; i--;){
    rotorTarget[i] = (unsigned char)((1 - tRatio) * lastRotorVals[4]); 
  }
  
  emergencylastDistToGround = distToGround;
}
//-------------------------------------------------------------------------------
void loop(){
  unsigned long now = millis();
  unsigned long lastContact = now - lastFltCmpContact;
  unsigned long dt = now - lastTime;
  
  //if(lastContact > 1000){
    //memcpy(lastRotorVals, rotorCurrent, sizeof(unsigned char) << 2);
    //emergencyLanding(dt);   
  //}
  
  // for each rotor speed control...
  for(int i = 0; i < 4; i++){
    // approach target speeds by increments of 1
    int dx = rotorTarget[i] - rotorCurrent[i] < 0 ? -1 : 1;
    rotors[i].write(rotorTarget[i]);
  }
  
  lastTime = now;
}
//-------------------------------------------------------------------------------
void serialEvent(){
  BasStnMsg received;
  if(Serial.available() > 0){
    if(Radio.Read(&received, sizeof(BasStnMsg)) < 0) return;
    
    if(manualOverride = received.override){
      // if this message indicates manual control. Set the target
      // to the human indicated values.
      for(int i = 4; i--;){
       rotorTarget[i] = received.rotors[i];
      }
    }
    
    lastFltCmpContact = millis();
  }
}

