#ifndef __NES_H
#define __NES_H

#include "stm32f4xx.h"

#pragma anon_unions
typedef struct {
	uint8_t Constant_NES[4];
	uint8_t romnum, vromnum, romfeature, rommappernum;
	uint8_t dontcare[8];
} NesRomHeader;

typedef struct {
	NesRomHeader;
	uint8_t data[40960];
} NesRom;

extern uint32_t frame_count;
const NesRom* rom_select(int sel);
void nes_init(const NesRom* game);
void nes_frame(uint8_t render);

#endif
