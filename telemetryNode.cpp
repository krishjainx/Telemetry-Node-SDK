/*
 * telemetryNode.cpp v1.0
 * Created: 7/10/18 by Andrew Gutierrez
 * Modified: 7/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the teleetry server that collects all the data.
 */

#include <telemetryNode.h>

TelemetryNode::TelemetryNode(byte deviceID){
  this->deviceID = deviceID;
  byte temp[]={deviceID,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  packet = temp;
  SERIAL_PORT.begin(BAUDRATE);
  lastPacket=millis();
}

/*
 * byte getDeviceID()
 * returns: current deviceID
 * Retrive node deviceID. ID's are used by the server
 * to determine the unpacking protocol. It is also used as
 * the leadig byte in the transmission.
 */
byte TelemetryNode::getDeviceID(){
  return deviceID;
}

/*
 * byte* getPacket()
 * returns: data packet to send to server
 * Get the current data in the packet
 */
byte* TelemetryNode::getPacket(){
  return packet;
}

/*
 * byte getByteAt(byte index)
 * index: index for byte to returns
 * returns: byte at given index in package
 * Get the data stored at the given index in the packet
 * sent to the server
 */
 byte TelemetryNode::getByteAt(byte index){
   return packet[index];
 }

/*
 * byte checkSum(byte* dataToSend)
 * dataToSend: the packet to be sent over serial
 * returns: last byte for packet
 * Generates the required last bit to set the 8-bit, overflow
 * packet sum to 0xFF. Used for serial error checking.
 */
byte TelemetryNode::generateLastByte(byte* dataToSend){
  byte sum=0;
  for (int i = 0; i < PACKET_SIZE-1; i++) {
    sum+=dataToSend[i];
  }
  return 0xFF-lowByte(sum);
}

/*
 * bool checkSum(byte* recivedData)
 * recivedData: serial packet recived by node
 * returns: whether any errors have been detected using the checksum
 * Serial error checking. The 8-bit overflow sum of all
 * serial transmissions should be 0xFF based on the setting
 * of the last bit.
 */
bool TelemetryNode::checkSum(byte* recivedData){
  byte sum=0;
  for (int i = 0; i < PACKET_SIZE; i++) {
    sum+=recivedData[i];
  }
  return sum==0xFF;
}

/*
 * void setByte(byte index,byte data)
 * index: index of byte to setByte
 * data: data to put in the packet
 * Set a byte of the packet at a given index
 */
void TelemetryNode::setByte(byte index,byte data){
  packet[index]=data;
}

/*
 * void TelemetryNode::setPacket(byte* inPacket)
 * inPacket: 12-byte packet to set the node package to.
 * Set all bytes in the package
 */
void TelemetryNode::setPacket(byte* inPacket){
  for (int i = 0; i < PACKET_SIZE; i++) {
    packet[i]=inPacket[i];
  }
}

/*
 * void sendPacket()
 * Set leading byte, setup checkSum byte, and send packet
 * over the appropriate serial port. This is a blocking write.
 * Will block for a few hundred microseconds
 */
void TelemetryNode::sendPacket(){
  packet[0]=deviceID;
  packet[PACKET_SIZE-1]=generateLastByte(packet);
  SERIAL_PORT.write(packet,PACKET_SIZE);
}

/*
 * intervalSend()
 * This function handles the timeing of sending seial packets
 * at regularly scheduled intervals. In order for this to happen, this function
 * should be called as frequently as possible and a minimum of once per interval.
 * This is also a blocking write, it will block for roughly a few hundred microseconds.
 * This code should protect against millis() rollover which occurs every ~49 days but it
 * has not been tested. To be safe, reset the arduino every couple weeks.
 */
void TelemetryNode::invervalSend(){
  long currentTime=millis();
  //Check if a new packet should be sent
  if(abs(currentTime-lastPacket)>=PACKET_INTERVAL){
    sendPacket();
    lastPacket=currentTime;
  }
}
