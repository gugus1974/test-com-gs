/*=============================================================================================*/
/* CRC calculation routines                                                                    */
/* Header file (crc.h)           													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*																					           */
/* Using the routines defined in this file you can calculate the CRC-16 and the CRC-32 of a    */
/* memory buffer.                                                                              */
/*                                                                                             */
/* A CRC calculation is identified by the following parameters:                                */
/* WIDTH : the number of bits of the calculated CRC                                            */
/* POLY  : the poly must be expressed like x^WIDTH + ... + x^0 with missing addends            */
/* INIT  : the init value for the CRC calculation                                              */
/* SWAP  : if TRUE the bit values are reversed from the memory                                 */
/* XOROUT: the value that must be XORed with the CRC result                                    */
/*                                                                                             */
/* The POLY can be expressed in a numeric form removing the X^WIDTH addend and specifying a 1  */
/* in the bit position corresponding to the exponent of the present addends. Note that if SWAP */
/* is TRUE, also the bit positions of the POLY must be reversed.                               */
/*                                                                                             */
/*                                                                                             */
/* XMODEM                     1111 1100 0000 0000                                              */
/* ------                     5432 1098 7654 3210                                              */
/* WIDTH : 16                 ---- ---- ---- ----                                              */
/* POLY  : x^16+x^12+x^5+1    0001 0000 0010 0001 = 0x1021                                     */
/* INIT  : 0                                                                                   */
/* SWAP  : FALSE              init_crc_table_16(0x1021, crc_table);                            */
/* XOROUT: 0                  crc = crc_16(crc_table, 0, p, len);                              */
/*                                                                                             */
/*                                                                                             */
/* CCITT                      1111 1100 0000 0000                                              */
/* ------                     5432 1098 7654 3210                                              */
/* WIDTH : 16                 ---- ---- ---- ----                                              */
/* POLY  : x^16+x^12+x^5+1    0001 0000 0010 0001 = 0x1021                                     */
/* INIT  : 0xFFFF (?)                                                                          */
/* SWAP  : undefined          init_crc_table_16(0x1021, crc_table);                            */
/* XOROUT: 0                  crc = crc_16(crc_table, 0xFFFF, p, len);                         */
/*                                                                                             */
/*                            0000 0000 0011 1111                                              */
/*                            0123 4567 8901 2345                                              */
/*                            ---- ---- ---- ----                                              */
/*                            1000 0100 0000 1000 = 0x8408                                     */
/*                                                                                             */
/*                            init_crc_table_16_swap(0x8408, crc_table);                       */
/*                            crc = crc_16_swap(crc_table, 0xFFFF, p, len);                    */
/*                                                                                             */
/*                                                                                             */
/* CRC-32                                                                                      */
/* ------                                                                                      */
/* WIDTH : 32                                                                                  */
/* POLY  : x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0                 */
/* INIT  : 0xFFFFFFFF                                                                          */
/* SWAP  : TRUE               0000 0000 0011 1111 1111 2222 2222 2233                          */
/* XOROUT: 0xFFFFFFFF         0123 4567 8901 2345 6789 0123 4567 8901                          */
/*                            ---- ---- ---- ---- ---- ---- ---- ----                          */
/*                            1110 1101 1011 1000 1000 0011 0010 0000 = 0xEDB88320             */
/*                                                                                             */
/*                            init_crc_table_32_swap(0xEDB88320, crc_table);                   */
/*                            crc = crc_32_swap(crc_table, 0xFFFFFFFF, p, len) ^ 0xFFFFFFFF;   */
/*                                                                                             */
/* The *_swap() routines could be used to calculate CRCs with any bit length (up to 32)        */
/*																					           */
/*=============================================================================================*/

#ifndef _CRC_
#define _CRC_


/*=============================================================================================*/
/* Typedefs */

typedef unsigned short CrcTable16[256];		/* CRC table type for 16 bit CRC calculation */
typedef unsigned long  CrcTable32[256];		/* CRC table type for 32 bit CRC calculation */

/*=============================================================================================*/
/* Globals */

/* Precalculated CRC tables */
extern const CrcTable16 crc_table_0x1021;		/* init_crc_table_16(0x1021, crc_table_0x1021);           */
extern const CrcTable16 crc_table_0x8408_swap;	/* init_crc_table_16_swap(0x8408, crc_table_0x8408_swap); */


/*=============================================================================================*/
/* Prototypes */

void init_crc_table_16(unsigned short poly, CrcTable16 crc_table);
void init_crc_table_16_swap(unsigned short poly, CrcTable16 crc_table);
void init_crc_table_32(unsigned short poly, CrcTable32 crc_table);
void init_crc_table_32_swap(unsigned short poly, CrcTable32 crc_table);

unsigned short crc_16(CrcTable16 crc_table, unsigned short crc, unsigned char *p, unsigned long len);
unsigned short crc_16_swap(CrcTable16 crc_table, unsigned short crc, unsigned char *p, unsigned long len);
unsigned long crc_32(CrcTable32 crc_table, unsigned long crc, unsigned char *p, unsigned long len);
unsigned long crc_32_swap(CrcTable32 crc_table, unsigned long crc, unsigned char *p, unsigned long len);


/*=============================================================================================*/

#endif


