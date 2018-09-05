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
for example a device connected to an Alltrax AXE might have an ID of 0x01.
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

### Device packing
Each of the system boards/nodes will be discussed here with the types of data that will sent
and the packing used to send the data.
