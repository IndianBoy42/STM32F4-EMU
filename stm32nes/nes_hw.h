#ifndef __NES_HARDWARE_H
#define __NES_HARDWARE_H

typedef u8  NESRegister8;
typedef u16 NESRegister16;

typedef struct {
	u8 zeropage[256];
	u8 stack[256];
	u8 ram[1536];

	u8 io1[8];
	u8 io2[32];

	u8 rom[8160];
	
	u8 sram[8192];

	u8 prgrom[32768];
} NESMemory;

typedef struct {
	NESRegister16 pc;
	NESRegister8 sp;
	NESRegister8 acc;
	NESRegister8 idx;
	NESRegister8 idy;
	NESProcStatus p;
} NESRegisters;

typedef struct {
	u8 carry;
	u8 zero;
	u8 intdisable;
	u8 decimal;
	u8 brk;
	u8 overflow;
	u8 negative;
} NESProcStatus

#endif
