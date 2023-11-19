#include <string>

#define CRC16_PRESET 0xFFFF
#define CRC16_POLYNOMIAL 0xA001 // bit reverse of 0x8005

u_int16_t crc16(u_int8_t* buf, int len);