/*
 * telemetryNode.cpp v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 */

 #include "telemetryNode.h"

 uint8_t _checksum(struct Packet *p){
   uint8_t *p8 = (uint8_t*)p;
   uint8_t s = 0;
   for (size_t i = 0; i < 15; i++)
     s += p8[i];
   return 0xff-s;
 }

 void TelemetryNode::begin(long baudrate){
   _serial->begin(baudrate);
   while(!connected){connect();}
 }

 void TelemetryNode::update(){
   if(connected){
     checkHeartbeat();
   }else{
     connect();
   }
 }

 void TelemetryNode::setPacketNum(uint8_t id){
   switch ((DeviceID)id) {
     case DEVICE_ALLTRAX:
     case DEVICE_VESC:
     case DEVICE_MOTOR_BOARD:
     case DEVICE_THROTTLE:
       numPackets = 1;
       break;
     case DEVICE_GPS_IMU:
       numPackets = 2;
       break;
     default:
       numPackets = 1;
       break;
   }
 }

 void TelemetryNode::checkHeartbeat(){
   while (_serial->available()!=0) {
     if(_serial->read()==CONN_HEARTBEAT){
       sendData();
       lastHeartbeat=millis();
     }
   }
   if(millis()-lastHeartbeat>1000){connected=false;}
 }

 void TelemetryNode::connect(){
   _serial->write(CONN_INIT);
   delay(100);
   while(_serial->available()!=0){
     if(_serial->read()==CONN_RESPONSE){
       _serial->write(deviceID);
       delay(100);
       while(_serial->available()!=0){
         if(_serial->read()==CONN_CONFIRM){
           connected=true;
           lastHeartbeat=millis();
         }
       }
     }
   }
 }

 void TelemetryNode::sendData(){
   pack((void*)(currentPack));
   for (uint8_t i = 0; i < numPackets; i++) {
     uint8_t *outBytes = (uint8_t*)(&currentPack[i]);
     for (uint16_t j = 0; j < 16; j++) {
       _serial->write(outBytes[j]);
     }
   }
 }

 void AlltraxNode::pack(void *p){
   Packet* packets = (Packet*)(p);
   packets[0].startByte=0xF0;
   uint16_t* p16 = (uint16_t*) (packets[0].data);
   p16[0] = diodeTemp;
   p16[1] = inVoltage;
   p16[2] = outCurrent;
   p16[3] = inCurrent;
   uint8_t *p8 = (uint8_t*)(&p16[4]);
   p8[0] = dutyCycle;
   p8[1] = errorCode;
   p8[2] = 0x00;
   p8[3] = 0x00;
   p8[4] = 0x00;
   packets[0].packetNum= 0x00;
   packets[0].checksum = 0x00;
   packets[0].checksum = _checksum(&packets[0]);
 }


 void VescNode::pack(void *p){
   Packet* packets = (Packet*)(p);
   packets[0].startByte=0xF0;
   uint16_t* p16 = (uint16_t*) (packets[0].data);
   p16[0] = fetTemp;
   p16[1] = inVoltage;
   p16[2] = outCurrent;
   p16[3] = inCurrent;
   uint8_t *p8 = (uint8_t*)(&p16[4]);
   p8[0] = dutyCycle;
   p8[1] = faultCode;
   p8[2] = 0x00;
   p8[3] = 0x00;
   p8[4] = 0x00;
   packets[0].packetNum=0x00;
   packets[0].checksum = 0x00;
   packets[0].checksum = _checksum(&packets[0]);
 }

 void MotorBoardNode::pack(void *p){
   Packet* packets = (Packet*)(p);
   packets[0].startByte=0xF0;
   uint32_t *p32 = (uint32_t*) (packets[0].data);
   uint32_t *temp = (uint32_t*) (&motorTemp);
   p32[0] = temp[0];
   p32[1] = motorRPM;
   p32[2] = propRPM;
   packets[0].packetNum=0x00;
   packets[0].checksum = 0x00;
   packets[0].checksum = _checksum(&packets[0]);
 }

 void GPSNode::pack(void *p){
   Packet* packets = (Packet*)(p);
   packets[0].startByte=0xF0;
   uint32_t *p32_1 = (uint32_t*) (packets[0].data);
   uint32_t *lat32 = (uint32_t*) (&lat);
   uint32_t *lng32 = (uint32_t*) (&lng);
   uint32_t *speed32 = (uint32_t*) (&speed);
   p32_1[0] = lat32[0];
   p32_1[1] = lng32[0];
   p32_1[2] = time;
   uint8_t *p8_1 = (uint8_t*) (&p32_1[3]);
   p8_1[0] = numSat;
   packets[0].packetNum=0x00;
   packets[0].checksum = _checksum(&packets[0]);
   packets[1].startByte=0xF0;
   uint32_t *p32_2 = (uint32_t*) (packets[1].data);
   p32_2[0] = speed32[0];
   uint8_t* p8_2 = (uint8_t*)(&p32_2[1]);
   p8_2[0] = heading;
   p8_2[1] = 0x00;
   p8_2[2] = 0x00;
   p8_2[3] = 0x00;
   p8_2[4] = 0x00;
   p8_2[5] = 0x00;
   p8_2[6] = 0x00;
   p8_2[7] = 0x00;
   p8_2[8] = 0x00;
   packets[1].packetNum=0x01;
   packets[1].checksum = _checksum(&packets[1]);
 }

 void ThrottleNode::pack(void *p){
   Packet* packets = (Packet*)(p);
   packets[0].startByte=0xF0;
   uint16_t* p16 = (uint16_t*) (packets[0].data);
   p16[0] = throt;
   p16[1] = 0x00;
   p16[2] = 0x00;
   p16[3] = 0x00;
   p16[4] = 0x00;
   p16[5] = 0x00;
   p16[6] = 0x00;
   packets[0].packetNum=0x00;
   packets[0].checksum = _checksum(&packets[0]);

 }
