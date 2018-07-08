# Telemetry-Node-SDK
Arudino Library for telemetry communication with Raspberry Pi server

## Protocol
Data is sent in 12-byte packets at 115200 baud. Streaming data should be sent
at 4hz. Data can be decimated on the server side so improve performance if
needed but the devices should still send data regularly.

### Byte Breakdown
- Device ID
- Data byte0
- Data byte1
- Data byte2
- Data byte3
- Data byte4
- Data byte5
- Data byte6
- Data byte7
- Data byte8
- Data byte9
- 8-bit Checksum

The device id is unique to the type of device but not to the device itself,
for example a device connected to an Alltrax AXE might have an ID of 0x01.
The last byte of the transmission should be assigned at before it is sent
so that the overflow sum of the previous 11-bytes is 0x00.
(i.e. 0xFF-sumOfPrevEleven).

### Device IDs
Device IDs are unique to the type of board and are used as the lead byte for
serial transmission. More device IDs will be assigned in future versions as necessary.
- Alltrax Motor Controller: 0x01
- VESC data: 0x02
- Motor board: 0x03
- Battery Board: 0x04
- Throttle board: 0x05
