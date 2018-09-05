# Telemetry-Node-SDK
Arudino Library for telemetry communication with Raspberry Pi server

## Protocol
Data is sent in 16-byte packets at 115200 baud. Streaming data should be sent
at 4hz. Data can be decimated on the server side so improve performance if
needed but the devices should still send data regularly.

### Byte Breakdown
- Byte 0: Device ID
- Byte 1-4: Data (1)
- Byte 5-8: Data (2)
- Byte 9-12: Data (3)
- Byte 13: Data (misc)
- Byte 14: packet# (typically 0x00)
- Byte 15: 8-bit Checksum

The device id is unique to the type of device but not to the device itself,
for example a device connected to an Alltrax AXE might have an ID of 0x00.
The last byte of the transmission should be assigned at before it is sent
so that the overflow sum of the previous 15-bytes is 0x00.
(i.e. 0xFF-sumOfPrevFifteen).

### Device IDs
Device IDs are unique to the type of board and are used as the lead byte for
serial transmission. More device IDs will be assigned in future versions as necessary.
Device IDs are enumerated and defined in telemetryNode.h. Current device ID's are listed below:
- DEVICE_ALLTRAX = 0x00 (Alltrax Motor Controller)
- DEVICE_VESC = 0x01 (VESC controller)
- DEVICE_MOTOR_BOARD = 0x02 (Motor board)
- DEVICE_BATTERY_BOARD = 0x03 (Battery board)
- DEVICE_GPS_IMU = 0x04 (GPS/accelerometer board)
- DEVICE_THROTTLE = 0x05 (Throttle board)

### Packet Number
For the most part, only one packet of data is required to transmit all the necessary
data from each board and the packet number will be 0x00 but in some circumstances,
more than one transmission may be required. In these cases, the packet number will
be used to identify which data packet is being sent to help with serial parsing.

## Device packing
Each of the system boards/nodes will be discussed here with the types of data that will sent
and the packing used to send the data.

### DEVICE_ALLTRAX
The following data points will be sent from the Alltrax controller:
#### Controller temp (diodeTemp)
- Description: Internal temperature of the controller in deg C
- Size: 2 bytes _diodeTemp=(diodeTempH<<8)&diodeTempL_
- Encoding: tempC = (diodeTemp-0x0C)(.48828125)

#### Battery Voltage (inVoltage)
- Description: Input voltage to the motor controller
- Size: 2 bytes _inVoltage=(inVoltageH<<8)&inVoltageL_
- Encoding: voltage = inVoltage(.1025)

#### Motor Current (outCurrent)
- Description: Output current to the motor from controller (A)
- Size: 2 bytes _outCurrent=(outCurrentH<<8)&outCurrentL_
- Encoding: 1A/bit

#### Battery Current (inCurrent)
- Description: Current input to the motor controller (A). Calculated from dutyCycle and outCurrent
- Size: 2 bytes _inCurrent=(inCurrentH<<8)&inCurrentL_
- Encoding: 1A/bit

#### Duty Cycle (dutyCycle)
- Description: duty cycle(%) for driving the motor mapped from 0-255. Technically called throttle %
- Size: 1 byte
- Encoding: percent mapped 0-255

#### Error Code (errorCode)
- Description: each bit is a flag for an Alltrax error code.
- Size: 1 byte
- Encoding:
  - Bit 0: Over-throttle
  - Bit 1: Under-temp
  - Bit 2: HPD
  - Bit 3: Over-temp
  - Bit 4: unused
  - Bit 5: Under-voltage
  - Bit 6: Over-voltage
  - Bit 7: controller in boot sequence

#### Alltrax packing
- Byte 0: DeviceID (0x00)
- Byte 1: diodeTempH
- Byte 2: diodeTempL
- Byte 3: inVoltageH
- Byte 4: inVoltageL
- Byte 5: outCurrentH
- Byte 6: outCurrentL
- Byte 7: inCurrentH
- Byte 8: inCurrentL
- Byte 9: dutyCycle
- Byte 10: errorCode
- Byte 11-13: unused (0x00)
- Byte 14: 0x00  (packet #)
- Byte 15: 8-bit checksum
