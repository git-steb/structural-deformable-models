/* -*- C++ -*- */
/* obtained from: http://www.geocities.com/CapeCanaveral/Launchpad/3632/crc_c.htm */
/*****************************************
crc.c - straightforward 16 bit CRC
by Alberto Ricci Bitti
released to public domain

compatibility notes:
works on little-endian machines only,
assumes 32 bit long integers,
16 bit integers and 8 byte characters
*****************************************/

#ifndef _CRC_H_
#define _CRC_H_

/*crc-16 standard root*/

#define POLYNOMIAL 0x8005

/*place your own here*/
#define INITIAL_VALUE 0x0000 

namespace crc {

union {
    unsigned long Whole;
    struct
    {
	unsigned char Data;
	unsigned short  Remainder;
	unsigned char Head;
    } Part;
} CRC_buffer;
 
/*internal use only - puts a byte*/

inline void PutCRC(unsigned char b)
{  
    using namespace crc;
    unsigned char i;
    CRC_buffer.Part.Data = b;
    for (i=0; i<8; i++)
    {
	CRC_buffer.Whole = CRC_buffer.Whole << 1;
	if (CRC_buffer.Part.Head & 0x01)
	    CRC_buffer.Part.Remainder ^= POLYNOMIAL;
    }
}
}; // end of namespace crc


/*call this routine with your own data buffer*/
/* yes! it's really that simple!*/

inline unsigned short CRC (const unsigned char * Data,
			   unsigned int Length)
{
    using namespace crc;
    CRC_buffer.Part.Remainder = INITIAL_VALUE;
    while (Length-- > 0)
	PutCRC(*Data++);
    PutCRC(0);
    PutCRC(0);
    return CRC_buffer.Part.Remainder;
}

#endif
