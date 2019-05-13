/*
 * telemetryNode.h v1.1
 * Created: 7/10/18 by Andrew Gutierrez and Chris Kjellqvist
 * Modified: 12/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 *
 * v1.0
 * Fully functional library for telemetry streaming to
 * Telemtry-Server-SDK. Support for the following boards:
 *  - URSS Alltrax Controller
 *  - URSS VESC Controller
 *  - URSS Motor board
 *  - URSS Battery Board
 *  - URSS GPS/IMU Board
 *  - URSS Throttle Board
 *
 * v1.1
 * Added support for full duplex communication with heartbeat packets.
 * Heartbeats can now send data back to the nodes with data requests.
 * Added unpacking functions for:
 * - URSS Alltrax Controller
 * - URSS VESC Controller
 */
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

 uint8_t validateChecksum(uint8_t* p){
   uint8_t s=0;
   for (size_t i = 0; i < 16; i++)
     s+=p[i];
   return s;
 }

 void TelemetryNode::begin(long baudrate){
   _serial->begin(baudrate);
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
   switch (rState) {
     case HEARTBEAT_WAITING:
       if(_serial->available()>0){
         uint8_t inByte = _serial->read();
         if(inByte==CONN_HEARTBEAT){
           hbPacket[0]=inByte;
           lastHeartbeat=millis();
           if(_serial->available()>=15){
             sendData();
             for (size_t i = 1; i < 16; i++)
               hbPacket[i]=_serial->read();
             if(validateChecksum(hbPacket)==0xFF){
               unpack();
             }
           }else{
             rState=HEARTBEAT_RECEIVING;
             hbPacketTimeout=millis();
           }
         }
       }
       break;
     case HEARTBEAT_RECEIVING:
       if(_serial->available()>=15){
         sendData();
         for (size_t i = 1; i < 16; i++)
           hbPacket[i]=_serial->read();
         if(validateChecksum(hbPacket)==0xFF){
           unpack();
         }
         rState=HEARTBEAT_WAITING;
       }else{
         if(millis()-hbPacketTimeout>10){
           rState=HEARTBEAT_WAITING;
         }
       }
       break;
   }
   if(millis()-lastHeartbeat>1000){connected=false;}
 }

 void TelemetryNode::connect(){
   _serial->write(CONN_INIT);
   delay(100);
   if(_serial->available()>0){
     if(_serial->read()==CONN_RESPONSE){
       _serial->write(deviceID);
       delay(100);
       if(_serial->available()>0){
         if(_serial->read()==CONN_CONFIRM){
           connected=true;
           lastHeartbeat=millis();
           rState = HEARTBEAT_WAITING;
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

 void AlltraxNode::unpack(){
   throt = hbPacket[2]<<8 | hbPacket[1];
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

 void VescNode::unpack(){
   throt = hbPacket[2]<<8|hbPacket[1];
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

 void MotorBoardNode::unpack(){

 }

 void GPSIMUNode::pack(void *p){
   Packet* packets = (Packet*)(p);

   // Assemble packet 1/2
   // imu pitch, imu yaw, imu roll, fix, num gps satellites, heading
   packets[0].startByte=0xF0;
   uint32_t *p32_1 = (uint32_t*) (packets[0].data);
   uint32_t *imuPitch32 = (uint32_t*) (&imuPitch);
   uint32_t *imuRoll32 = (uint32_t*) (&imuRoll);
   p32_1[0] = imuPitch32[0];
   p32_1[1] = imuRoll32[0];
   uint8_t *p8_1 = (uint8_t*) (&p32_1[2]);
   p8_1[0] = fix;
   p8_1[1] = numSatellites;
   p8_1[2] = heading;
   packets[0].packetNum=0x00;
   packets[0].checksum = _checksum(&packets[0]);

   // Assemble packet 2/2
   // latitude, longitude, speed (knots), gps fix status
   // Sends gps fix twice to let server decide when to throw out gps data
   packets[1].startByte=0xF0;
   uint32_t *p32_2 = (uint32_t*) (packets[1].data);
   uint32_t *latitude32 = (uint32_t*) (&latitude);
   uint32_t *longitude32 = (uint32_t*) (&longitude);
   uint32_t *speedKnots32 = (uint32_t*) (&speedKnots);
   p32_2[0] = latitude32[0];
   p32_2[1] = longitude32[0];
   p32_2[2] = speedKnots32[0];
   uint8_t* p8_2 = (uint8_t*)(&p32_2[3]);
   p8_2[0] = fix;
   packets[1].packetNum=0x01;
   packets[1].checksum = _checksum(&packets[1]);
 }

 void GPSIMUNode::unpack(){

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

 void ThrottleNode::unpack(){

 }
