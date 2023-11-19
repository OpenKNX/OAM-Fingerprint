#include "crc16.h"

u_int16_t crc16(u_int8_t* buf, int len)
{
    u_int16_t crc = CRC16_PRESET;

    for (int pos = 0; pos < len; pos++)
    {
        crc ^= (u_int16_t)buf[pos]; // XOR byte into least sig. byte of crc

        for (u_int8_t i = 8; i != 0; i--)
        { // Loop over each bit
            if ((crc & 0x0001) != 0)
            {              // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= CRC16_POLYNOMIAL;
            }
            else
            {              // Else LSB is not set
                crc >>= 1; // Just shift right
            }
        }
    }

    return crc;
}