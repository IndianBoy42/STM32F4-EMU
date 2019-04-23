#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"

uint32_t frame_count = 0;

void nes_init(void) {
	cpu_initmem(romfile.data, romfile.romnum);
	cpu_reset();
	ppu_init(&romfile.data[romfile.romnum * 0x4000], (romfile.romfeature & 0x01));
}

void nes_frame(uint8_t render) { 
    frame_count++; 
    SpriteHitFlag = FALSE;         
    for(PPU_scanline=0; PPU_scanline<20; PPU_scanline++){    
        cpu_exec(CLOCKS_PER_SCANLINE);   
    }   
    /* scanline: 20*/   
    cpu_exec(CLOCKS_PER_SCANLINE);   
    PPU_scanline++;   //20++   
    PPU_Reg.R2 &= ~R2_SPR0_HIT;   
    /* scanline: 21~261*/      
    for(; PPU_scanline < SCAN_LINE_DISPLAY_END_NUM; PPU_scanline++){   
        if((SpriteHitFlag == TRUE) && ((PPU_Reg.R2 & R2_SPR0_HIT) == 0)){   
            int clocks = sprite[0].x * CLOCKS_PER_SCANLINE / NES_DISP_WIDTH;   
            cpu_exec(clocks);   
            PPU_Reg.R2 |= R2_SPR0_HIT;   
            cpu_exec(CLOCKS_PER_SCANLINE - clocks);   
        }else{   
            cpu_exec(CLOCKS_PER_SCANLINE);   
        }   

        if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE)){
            if(SpriteHitFlag == FALSE)   
                ppu_spr0_hit_flag(PPU_scanline - SCAN_LINE_DISPLAY_START_NUM);
        }   
        if(render) 
        // if(frame_count & 1) 
        // if((frame_count & 1) ^ (PPU_scanline & 1)) 
            ppu_render_line(PPU_scanline - SCAN_LINE_DISPLAY_START_NUM);
    }   
    /* scanline: 262*/   
    cpu_exec(CLOCKS_PER_SCANLINE);   
    PPU_Reg.R2 |= R2_VBlank_Flag;  
    if(PPU_Reg.R0 & R0_VB_NMI_EN){   
        NMI_Flag = SET1;
    } 

    // tft_clear();
    // tft_printi(0, 0, TIM6->CNT);
    // tft_pxbuf_write(DisplayBuffer);
}
