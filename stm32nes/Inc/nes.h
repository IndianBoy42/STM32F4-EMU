#ifndef __NES_H
#define __NES_H

typedef struct {
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t P;
	uint8_t S;
	uint16_t PC;
} NESRegisters;

#endif