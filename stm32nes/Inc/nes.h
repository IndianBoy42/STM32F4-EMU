#ifndef __NES_H
#define __NES_H

#include "stm32f4xx.h"

extern uint32_t frame_count;
void nes_init(void);
void nes_frame(uint8_t render);

#endif
