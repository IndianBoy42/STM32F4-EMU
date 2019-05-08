/*  memory map */   
uint8_t cpu_ram[0x800] = {0};  /* RAM*/   
// uint8_t erom[0x2000] = {0};
// uint8_t sram[0x2000] = {0};
// #define SRAM sram
// #define EROM erom
//uint8_t* ppu_regbase;       /* PPU IO reg*/   
//uint8_t* apu_regbase;       /* APU IO reg*/    
const uint8_t* prg_rombank0;    /* prg-rom lower bank*/   
const uint8_t* prg_rombank1;    /* prg-rom upper bank*/   

extern uint8_t SpriteHitFlag, PPU_Latch_Flag;
extern uint8_t PPU_BG_VScrlOrg, PPU_BG_HScrlOrg;

__forceinline uint8_t ppu_read_nametables(void)   
{   
    uint16_t addrtemp = PPU_Mem.PPU_addrcnt & 0xFFF;   
   
    if(addrtemp > 0xC00){   
        return  PPU_Mem.name_table[3][addrtemp - 0xC00];        //nametable3   
    }   
    if(addrtemp > 0x800){   
        return  PPU_Mem.name_table[2][addrtemp - 0x800];        //nametable2   
    }   
    if(addrtemp > 0x400){   
        return  PPU_Mem.name_table[1][addrtemp - 0x400];        //nametable1   
    }else{   
        return  PPU_Mem.name_table[0][addrtemp];                //nametable0   
    }   
}   
   
__forceinline void ppu_write_nametables(uint8_t value)   
{   
    uint16_t addrtemp = PPU_Mem.PPU_addrcnt & 0xFFF;   
   
    if(addrtemp > 0xC00){   
        PPU_Mem.name_table[3][addrtemp - 0xC00] = value;        //nametable3   
        return;   
    }   
    if(addrtemp > 0x800){   
        PPU_Mem.name_table[2][addrtemp - 0x800] = value;        //nametable2   
        return;   
    }   
    if(addrtemp > 0x400){   
        PPU_Mem.name_table[1][addrtemp - 0x400] = value;        //nametable1   
        return;   
    }else{   
        PPU_Mem.name_table[0][addrtemp] = value;                //nametable0   
        return;   
    }   
}   
   
__forceinline void ppu_putmemory(uint8_t value)   
{   
    switch(PPU_Mem.PPU_addrcnt & 0xF000){   
    case 0x0000: //$0000 ~ $0FFF  
//       PPU_Mem.patterntable0[PPU_Mem.PPU_addrcnt] = value;   
         break;   
    case 0x1000: //$1000 ~ $1FFF
//       PPU_Mem.patterntable1[PPU_Mem.PPU_addrcnt & 0x0FFF] = value;   
         break;   
    case 0x2000: //$2000 ~ $2FFF   
         ppu_write_nametables(value);   
         break;   
    case 0x3000:    
         //$3000 ~ $3EFF
         //$3F00 ~ $3F0F    image  palette   
         //$3F10 ~ $3F1F    sprite palette   
         if((PPU_Mem.PPU_addrcnt & 0x1F) > 0x0F){   
            PPU_Mem.sprite_palette[(PPU_Mem.PPU_addrcnt & 0xF)] = value; 
            if((PPU_Mem.PPU_addrcnt & 3) == 0){
                PPU_Mem.sprite_palette[0] = PPU_Mem.image_palette[0] = value;   
                PPU_Mem.sprite_palette[4] = PPU_Mem.image_palette[4] = value;   
                PPU_Mem.sprite_palette[8] = PPU_Mem.image_palette[8] = value;   
                PPU_Mem.sprite_palette[12] =PPU_Mem.image_palette[12]= value;   
            }      
         }else{   
            PPU_Mem.image_palette[(PPU_Mem.PPU_addrcnt & 0xF)] = value;
            premap_palette();
         }   
//          ppu_write_nametables(value); 
         break;
    }   
    PPU_Reg.R0 & PPU_ADDRINCR ? PPU_Mem.PPU_addrcnt += 32 : PPU_Mem.PPU_addrcnt++ ;   
}   
  
__forceinline uint8_t ppu_getmemory(void)   
{  
    uint8_t temp;   
   
    temp = PPU_Mem.PPU_readtemp;
           
    switch(PPU_Mem.PPU_addrcnt & 0xF000){   
    case 0x0000: //$0000 ~ $0FFF   
         PPU_Mem.PPU_readtemp = PPU_Mem.patterntable0[PPU_Mem.PPU_addrcnt];
         break;   
    case 0x1000: //$1000 ~ $1FFF   
         PPU_Mem.PPU_readtemp = PPU_Mem.patterntable1[PPU_Mem.PPU_addrcnt & 0x0FFF];
         break;   
    case 0x2000: //$2000 ~ $2FFF   
         PPU_Mem.PPU_readtemp = ppu_read_nametables(); 
         break;   
    case 0x3000:    
         //$3000 ~ $3EFF  
         //$3F00 ~ $3F0F image  palette   
         //$3F10 ~ $3F1F    sprite palette   
         if(PPU_Mem.PPU_addrcnt >= 0x3F10){   
            temp =  PPU_Mem.sprite_palette[(PPU_Mem.PPU_addrcnt & 0xF)];
            break;     
         }   
         if(PPU_Mem.PPU_addrcnt >= 0x3F00){   
                temp = PPU_Mem.image_palette[(PPU_Mem.PPU_addrcnt & 0xF)];
                break;   
         }   
//          temp = ppu_read_nametables();
         break;   
    default: temp = 0;
    }   
    PPU_Reg.R0 & PPU_ADDRINCR ? PPU_Mem.PPU_addrcnt += 32 : PPU_Mem.PPU_addrcnt++ ;   
    return temp;   
}   
  
__forceinline void ppu_write_register(uint16_t RX, uint8_t value)   
{   
    switch(RX){   
/*$2000*/   
    case 0: PPU_Reg.R0 = value;   
//          printf("\r\n PPU r0: %x", value);  
//          PPU_AddrTemp = ( PPU_AddrTemp & 0xF3FF ) | ( ( ( (uint16_t)value ) & 0x0003 ) << 10 );   
//          PPU_BG_NameTableNum = PPU_Reg.R0 & R0_NAME_TABLE;      
/*$2001*/   break;   
    case 1: PPU_Reg.R1 = value;     
/*$2003*/   break;   
    case 3:
            Spr_Mem.spr_addrcnt = value;   
/*$2004*/   break;   
    case 4:
            Spr_Mem.spr_ram[Spr_Mem.spr_addrcnt++] = value;   
/*$2005*/   break;   
    case 5: //PPU_Reg.R5 = value;   
            if(PPU_Latch_Flag){
                PPU_BG_VScrlOrg = (value > 239) ? 0 : value;   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0xFC1F ) | ((((uint16_t)value) & 0xF8 ) << 2);   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0x8FFF ) | ((((uint16_t)value) & 0x07 ) << 12);   
            }else{
                PPU_BG_HScrlOrg = value; 
//              PPU_AddrTemp = ( PPU_AddrTemp & 0xFFE0 ) | ((((uint16_t)value) & 0xF8 ) >> 3 );   
            }                       
            PPU_Latch_Flag ^= 1;   
/*$2006*/   break;   
    case 6:    
//          if(PPU_Latch_Flag){     //1   
////                PPU_Mem.PPU_addrcnt = (PPU_Mem.PPU_addrcnt << 8) + value;
//              /* Low */   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0xFF00 ) | (((uint16_t)value ) & 0x00FF);   
//              PPU_Mem.PPU_addrcnt = PPU_AddrTemp;   
//              PPU_BG_VScrlOrg = (uint8_t)(PPU_Mem.PPU_addrcnt & 0x001F );   
//              PPU_BG_HScrlOrg = (uint8_t)((PPU_Mem.PPU_addrcnt& 0x03E0 ) >> 5 );    
//          }else{                 //0   
//              /* High */   
//              PPU_AddrTemp = (PPU_AddrTemp & 0x00FF)|((((uint8_t)value) & 0x003F ) << 8 );    
//          }   
//          PPU_Latch_Flag ^= 1;   
   
            PPU_Mem.PPU_addrcnt = (PPU_Mem.PPU_addrcnt << 8) + value;
            PPU_Latch_Flag ^= 1;   
/*$2007*/   break;    
    case 7: /*д PPU Memory Data*/   
            ppu_putmemory(value);   
            break;
    }   
}   
 
__forceinline uint8_t ppu_read_register(uint16_t RX)   
{   
    uint8_t temp;   
   
    switch(RX){   
    case 0: temp = PPU_Reg.R0;  //$2000 RW   
        break;   
    case 1: temp = PPU_Reg.R1;  //$2001 RW   
        break;   
    case 2: temp = PPU_Reg.R2;   
        PPU_Reg.R2 &= ~(R0_VB_NMI_EN);
        PPU_Latch_Flag = 0;   
        // Make a Nametable 0 in V-Blank   
        if ((PPU_scanline > 20 && PPU_scanline < 262) && !(PPU_Reg.R0 & R0_VB_NMI_EN)){   
            PPU_Reg.R0 &= ~R0_NAME_TABLE;
    //              PPU_BG_NameTableNum = 0;   
        }   
        break;; //$2002 R   
    case 4: 
        temp = Spr_Mem.spr_ram[Spr_Mem.spr_addrcnt++];   
        break;   
    case 7:  
        temp = ppu_getmemory();   
        break;   
    default :   
        return RX;   
    } 
    return temp;       
}  

void sprite_dma(uint8_t scr_addr)  
{      
    extern int cpu_clockticks;
    cpu_clockticks -= 512;

    const uint8_t *scr_addrptr = 0;  
    int     i;   

    switch(scr_addr >> 4){  
    case 0x0: //RAM   
    case 0x1: 
        scr_addrptr = &cpu_ram[(scr_addr << 8)];   
        break;   
    case 0x6: //SRAM   
    case 0x7:    
    #ifdef SRAM 
        scr_addrptr = SRAM + ((scr_addr << 8) - 0x6000);   
        break;
    #else
        return;
    #endif   
    case 0x8:
    case 0x9:   
    case 0xA:   
    case 0xB:   
        scr_addrptr = &prg_rombank0[(scr_addr << 8) - 0x8000];   
        break;   
    case 0xC:   
    case 0xD:   
    case 0xE:   
    case 0xF:   
        scr_addrptr = &prg_rombank1[(scr_addr << 8) - 0xC000];   
        break;   
    }  

    for(i=0; i<256; i++){   
        Spr_Mem.spr_ram[i] = scr_addrptr[i];   
    }   
}   

__forceinline int cpu_getmemory(uint16_t addr)
{   
    switch(addr & 0xF000){   
    case 0x0000:
    case 0x1000:
        return cpu_ram[addr&0x7FF];    
    case 0x2000:
    case 0x3000:
         return ppu_read_register(addr&0x7);  
    case 0x4000:   
        if(addr < 0x4013){  
    //          return (apu_regbase[addr&0x1F]);
        }   
        if(addr == 0x4014){   
    //          //SpriteRAM_DMA();break; 
        }   
         if(addr == 0x4015){ //pAPU Sound / Vertical Clock Signal Register   
         }   
         if(addr == 0x4016){   
            return joypad_read(0);
         }   
         if(addr == 0x4017){   
            return joypad_read(1);
         }   
         break;   
    case 0x5000: /* expansion rom*/
    #ifdef EROM
        return EROM[addr - 0x5000];
    #else
        return 0;
    #endif
    case 0x6000:
    case 0x7000:
    #ifdef SRAM
        return SRAM[addr - 0x6000];
    #else
        return 0;
    #endif
    case 0x8000:
    case 0x9000:
    case 0xA000:   
    case 0xB000:   
        return prg_rombank0[addr - 0x8000];   
    case 0xC000:   
    case 0xD000:   
    case 0xE000:   
    case 0xF000:   
        return prg_rombank1[addr - 0xC000];    
    }   

    return (addr >> 8);  
}
__forceinline int cpu_getmemory16(uint16_t addr)
{   
    switch(addr & 0xF000){   
    case 0x0000:
    case 0x1000:
        return *(uint16_t*)&cpu_ram[addr&0x7FF];    
    case 0x2000:
    case 0x3000:
         return ppu_read_register(addr&0x7);  
    case 0x4000:   
        if(addr < 0x4013){  
    //          return (apu_regbase[addr&0x1F]);
        }   
        if(addr == 0x4014){   
    //          //SpriteRAM_DMA();break; 
        }   
         if(addr == 0x4015){ //pAPU Sound / Vertical Clock Signal Register   
         }   
         if(addr == 0x4016){   
            return joypad_read(0);
         }   
         if(addr == 0x4017){   
            return joypad_read(1);
         }   
         break;   
    case 0x5000: /* expansion rom*/
    #ifdef EROM
        return *(uint16_t*)&EROM[addr - 0x5000];
    #else
        return 0;
    #endif
    case 0x6000:
    case 0x7000:
    #ifdef SRAM
        return *(uint16_t*)&SRAM[addr - 0x6000];
    #else
        return 0;
    #endif
    case 0x8000:
    case 0x9000:
    case 0xA000:   
    case 0xB000:   
        return *(uint16_t*)&prg_rombank0[addr - 0x8000];   
    case 0xC000:   
    case 0xD000:   
    case 0xE000:   
    case 0xF000:   
        return *(uint16_t*)&prg_rombank1[addr - 0xC000];    
    }   

    return (addr >> 8);  
}   

__forceinline void cpu_putmemory(uint16_t addr, uint8_t value)   
{   
    switch(addr & 0xF000){   
    case 0x0000:   
    case 0x1000:   
        cpu_ram[addr&0x7FF] = value;   
        break;   
    case 0x2000:
    case 0x3000:
        ppu_write_register(addr&0x7, value);   
        break;   
    case 0x4000:    
        if(addr == 0x4014){               
            sprite_dma(value);    
        }   
        if(addr == 0x4016){      
            joypad_write(0, value);             
        }   
        if(addr == 0x4017){   
            //joypad_write(1, value);   
        }   
//       if(addr >= 0x4020){   
//          EROM[addr - 0x6000] = value;   
//       }    
        break;

    case 0x5000: //Exp Rom
    #ifdef EROM
        EROM[addr - 0x5000] = value;
    #endif
        // gpio_toggle(LED2);
        break;

    case 0x6000:
    case 0x7000:  
    #ifdef SRAM
        SRAM[addr - 0x6000] = value;  
    #endif
        break; 

    case 0x8000:
    case 0x9000:
    case 0xA000:
    case 0xB000:
    case 0xC000:
    case 0xD000:
    case 0xE000:
    case 0xF000:
//       // Write to Mapper   
//           MapperWrite( wAddr, byData );   
        break;   
      }   
  }   
