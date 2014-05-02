#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <ArduTalk.h>

//     ___             _            _      
//    / __|___ _ _  __| |_ __ _ _ _| |_ ___
//   | (__/ _ \ ' \(_-<  _/ _` | ' \  _(_-<
//    \___\___/_||_/__/\__\__,_|_||_\__/__/
//                                         
#define YAW   0
#define PITCH 1 
#define ROLL  2

#define LAT 0
#define LON 1
#define ALT 2

#define ULTRA_SONIC 0

#define PI_POLL_DST2GND 0x01

//    ___ _               _      
//   / __| |_ _ _ _  _ __| |_ ___
//   \__ \  _| '_| || / _|  _(_-<
//   |___/\__|_|  \_,_\__|\__/__/
//                               
typedef struct{
  float orientation[3]; // yaw, pitch, roll
  float position[3];    // gps coordinates for location
} Location;

typedef struct{
  Location loc;
  int flags;
  unsigned char rotors[4]; // speed values for rotors
} fltCmpMsg;

typedef struct{
  unsigned char override;
  unsigned char rotors[4]; // speed values for rotors
} basStnMsg;

typedef struct{
  Location loc;
  unsigned char rotorVals[4];
  float distToGround;
} stateMsg;

//     ___ _     _          _    
//    / __| |___| |__  __ _| |___
//   | (_ | / _ \ '_ \/ _` | (_-<
//    \___|_\___/_.__/\__,_|_/__/
//                               
Servo rotors[4];
int rotorTarget[4];
int rotorCurrent[4];
int manualOverride     = 0;
float lastDistToGround = 0.0f;
unsigned long lastFltCmpContact, lastTime;
stateMsg relay = { 0 };

ArduTalk Radio(&Serial, 9600);

//    ___             _   _             
//   | __|  _ _ _  __| |_(_)___ _ _  ___
//   | _| || | ' \/ _|  _| / _ \ ' \(_-<
//   |_| \_,_|_||_\__|\__|_\___/_||_/__/
//                                      
//-------------------------------------------------------------------------------
void setup(){
  // hook up the servos, and set their speeds to 0
  for(int i = 4; i--;){
    rotors[i].attach(4 + i);
    rotors[i].write(0);
    
    rotorTarget[i] = rotorCurrent[i] = 0;
  }
  
  // setup I2C comms, join as slave
  Wire.begin(0x60);
  Wire.onReceive(I2Creceived);
  Wire.onRequest(I2Crequested);
  
  // so fast...
  Serial.begin(115200);
  
  
  lastTime = lastFltCmpContact = millis();
}
int cmToGround(){
  return analogRead(ULTRA_SONIC); 
}
//-------------------------------------------------------------------------------
void I2Creceived(int bytes){
  fltCmpMsg msg  = { 0 };
  static const stateMsg empty = { 0 };
  
  if(bytes >= sizeof(fltCmpMsg)){
    
    // TODO
    Wire.readBytes((char*)&msg, sizeof(fltCmpMsg));
 
    // clear out relay message
    relay = empty;       
 
    // see if the flight computer asked for the distance to ground
    if(msg.flags & PI_POLL_DST2GND){
       relay.distToGround = cmToGround() / 100.0;
    }
    
    // build up a relay message to be sent to the base station
    relay.loc = msg.loc;
    memcpy(relay.rotorVals, rotorCurrent, sizeof(unsigned char) * 4);
  }
}
//-------------------------------------------------------------------------------
void I2Crequested(){
    // fire away
    Radio.Write(&relay, sizeof(stateMsg)); 
}
//-------------------------------------------------------------------------------
unsigned long timeLanding = 0;
unsigned char lastRotorVals[4];
int emergencylastDistToGround;
char emergency = 0;

void emergencyLanding(unsigned long dt){
  float lRatio = timeLanding / 10000.0f;
  float tRatio = (timeLanding += dt) / 10000.0f;
  int distToGround = cmToGround();
  int delta = distToGround - emergencylastDistToGround;
  float vz = (delta / 100.0) * (dt / 1000.0);
  unsigned char doOffset = 0, offset = 0;
  
  // if we are decending faster than 0.25 m/s and we are within the measurement
  // range of the ultrasonic sensor, then make a modification to the throttle target
  if(vz < -0.25f && distToGround < 1000){
    doOffset = 1;
  }
  
  // power down slowly
  for(int i = 4; i--;){
    unsigned char lr = lastRotorVals[i];
    if(doOffset){
      // dx * y = -0.25
      // 
      offset = (unsigned char)((((1 - tRatio) * lr - (1 - lRatio) * lr)) / -0.25);
    }
    
    rotorTarget[i] = (unsigned char)((1 - tRatio) * lastRotorVals[i]) + offset; 
  }
  
  emergencylastDistToGround = distToGround;
}
//-------------------------------------------------------------------------------
void loop(){
  unsigned long now = millis();
  unsigned long lastContact = now - lastFltCmpContact;
  unsigned long dt = now - lastTime;
  
  if(lastContact > 1000){
    if(!emergency){
      memcpy(lastRotorVals, rotorCurrent, sizeof(unsigned char) << 2);
      emergency = 1;
    }
    emergencyLanding(dt);   
  }
  
  // for each rotor speed control...
  for(int i = 0; i < 4; i++){
    // approach target speeds by increments of 1
    int dx = rotorTarget[i] - rotorCurrent[i] < 0 ? -1 : 1;
    rotors[i].write((rotorCurrent[i] += dx));
  }
  
  lastTime = now;
}
//-------------------------------------------------------------------------------
void serialEvent(){
  basStnMsg received;
  if(Serial.available() > 0){
    if(Radio.Read(&received, sizeof(basStnMsg)) < 0) return;
    
    if(manualOverride = received.override){
      // if this message indicates manual control. Set the target
      // to the human indicated values.
      for(int i = 4; i--;){
       rotorTarget[i] = received.rotors[i];
      }
    }

    emergency = 0;    
    lastFltCmpContact = millis();
  }
}

