#ifndef CRC16_H
#define CRC16_H

class CRC16
{
    private:
    const unsigned int SHIFTER = 0x00FF;
    unsigned short crc16table[256];
    
    public:
    unsigned short calculateCRC16(char input[], int lenght);
    CRC16();
    ~CRC16();
};
#endif