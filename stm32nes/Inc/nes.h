#ifndef __NES_H
#define __NES_H

#include "stm32f4xx.h"

extern uint32_t frame_count;
void nes_init(uint8_t game);
void nes_frame(uint8_t render);

#endif
