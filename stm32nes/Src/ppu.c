/* Includes ------------------------------------------------------------------*/    
// #include "nes_main.h"   
#include "lcd_main.h"   
void tft_print_nes_line(int y, uint16_t* buf);
#include "ppu.h"

uint8_t NameTable[2048];   
   
PPU_RegType PPU_Reg;   
PPU_MemType PPU_Mem;   
   
Spr_MemType Spr_Mem;   
SpriteType  * const sprite = (SpriteType  *)&Spr_Mem.spr_ram[0];
   

uint8_t   SpriteHitFlag, PPU_Latch_Flag; 
int     PPU_scanline;     
uint16_t  DisplayBuffer[256*240] = {0};
static uint16_t  Buffer_scanline[8 + 256 + 8];    
   
uint8_t PPU_BG_VScrlOrg, PPU_BG_HScrlOrg;   
//uint8_t PPU_BG_VScrlOrg_Pre, PPU_BG_HScrlOrg_Pre;   
//uint8_t PPU_BG_NameTableNum;     
//uint16_t PPU_AddrTemp;   
static const uint16_t NES_Color_Palette[64] ={   
/*��ɫ������ַ->RGBֵ -> RGB565(16bit)*/   
/*  0x00 -> 0x75, 0x75, 0x75 */  0xAE73,    
/*  0x01 -> 0x27, 0x1B, 0x8F */  0xD120,   
/*  0x02 -> 0x00, 0x00, 0xAB */  0x1500,   
/*  0x03 -> 0x47, 0x00, 0x9F */  0x1340,   
/*  0x04 -> 0x8F, 0x00, 0x77 */  0x0E88,   
/*  0x05 -> 0xAB, 0x00, 0x13 */  0x02A8,   
/*  0x06 -> 0xA7, 0x00, 0x00 */  0x00A0,   
/*  0x07 -> 0x7F, 0x0B, 0x00 */  0x4078,   
/*  0x08 -> 0x43, 0x2F, 0x00 */  0x6041,   
/*  0x09 -> 0x00, 0x47, 0x00 */  0x2002,   
/*  0x0A -> 0x00, 0x51, 0x00 */  0x8002,   
/*  0x0B -> 0x00, 0x3F, 0x17 */  0xE201,   
/*  0x0C -> 0x1B, 0x3F, 0x5F */  0xEB19,   
/*  0x0D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x0E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x0F -> 0x00, 0x00, 0x00 */  0x0000,   
   
/*  0x10 -> 0xBC, 0xBC, 0xBC */  0xF7BD,   
/*  0x11 -> 0x00, 0x73, 0xEF */  0x9D03,   
/*  0x12 -> 0x23, 0x3B, 0xEF */  0xDD21,   
/*  0x13 -> 0x83, 0x00, 0xF3 */  0x1E80,   
/*  0x14 -> 0xBF, 0x00, 0xBF */  0x17B8,   
/*  0x15 -> 0xE7, 0x00, 0x5B */  0x0BE0,   
/*  0x16 -> 0xDB, 0x2B, 0x00 */  0x40D9,   
/*  0x17 -> 0xCB, 0x4F, 0x0F */  0x61CA,   
/*  0x18 -> 0x8B, 0x73, 0x00 */  0x808B,   
/*  0x19 -> 0x00, 0x97, 0x00 */  0xA004,   
/*  0x1A -> 0x00, 0xAB, 0x00 */  0x4005,   
/*  0x1B -> 0x00, 0x93, 0x3B */  0x8704,   
/*  0x1C -> 0x00, 0x83, 0x8B */  0x1104,   
/*  0x1D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x1E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x1F -> 0x00, 0x00, 00x0 */  0x0000,   
   
/*  0x20 -> 0xFF, 0xFF, 0xFF */  0xFFFF,   
/*  0x21 -> 0x3F, 0xBF, 0xFF */  0xFF3D,   
/*  0x22 -> 0x5F, 0x97, 0xFF */  0xBF5C,   
/*  0x23 -> 0xA7, 0x8B, 0xFD */  0x5FA4,   
/*  0x24 -> 0xF7, 0x7B, 0xFF */  0xDFF3,   
/*  0x25 -> 0xFF, 0x77, 0xB7 */  0xB6FB,   
/*  0x26 -> 0xFF, 0x77, 0x63 */  0xACFB,   
/*  0x27 -> 0xFF, 0x9B, 0x3B */  0xC7FC,   
/*  0x28 -> 0xF3, 0xBF, 0x3F */  0xE7F5,   
/*  0x29 -> 0x83, 0xD3, 0x13 */  0x8286,   
/*  0x2A -> 0x4F, 0xDF, 0x4B */  0xE94E,   
/*  0x2B -> 0x58, 0xF8, 0x98 */  0xD35F,   
/*  0x2C -> 0x00, 0xEB, 0xDB */  0x5B07,   
/*  0x2D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x2E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x2F -> 0x00, 0x00, 0x00 */  0x0000,   
   
/*  0x30 -> 0xFF, 0xFF, 0xFF */  0xFFFF,   
/*  0x31 -> 0xAB, 0xE7, 0xFF */  0x3FAF,   
/*  0x32 -> 0xC7, 0xD7, 0xFF */  0xBFC6,   
/*  0x33 -> 0xD7, 0xCB, 0xFF */  0x5FD6,   
/*  0x34 -> 0xFF, 0xC7, 0xFF */  0x3FFE,   
/*  0x35 -> 0xFF, 0xC7, 0xDB */  0x3BFE,   
/*  0x36 -> 0xFF, 0xBF, 0xB3 */  0xF6FD,   
/*  0x37 -> 0xFF, 0xDB, 0xAB */  0xD5FE,   
/*  0x38 -> 0xFF, 0xE7, 0xA3 */  0x34FF,   
/*  0x39 -> 0xE3, 0xFF, 0xA3 */  0xF4E7,   
/*  0x3A -> 0xAB, 0xF3, 0xBF */  0x97AF,   
/*  0x3B -> 0xB3, 0xFF, 0xCF */  0xF9B7,   
/*  0x3C -> 0x9F, 0xFF, 0xF3 */  0xFE9F,   
/*  0x3D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x3E -> 0x00, 0x00,0x 00 */  0x0000,   
/*  0x3F -> 0x00, 0x00, 0x00 */  0x0000   
};   

void ppu_init(const uint8_t* patterntableptr, uint8_t  ScreenMirrorType )   {   
    // memset(&PPU_Mem, 0, sizeof(PPU_Mem));   
    // memset(&Spr_Mem, 0, sizeof(Spr_Mem));   
    // memset(&PPU_Reg, 0, sizeof(PPU_Reg));   
   
    PPU_Mem.patterntable0 =  patterntableptr;   
    PPU_Mem.patterntable1 =  patterntableptr + 0x1000;   
       
    if(ScreenMirrorType == 0){   
        PPU_Mem.name_table[0] = &NameTable[0];   
        PPU_Mem.name_table[1] = &NameTable[0];   
        PPU_Mem.name_table[2] = &NameTable[1024];   
        PPU_Mem.name_table[3] = &NameTable[1024];   
    }else{                      
        PPU_Mem.name_table[0] = &NameTable[0];   
        PPU_Mem.name_table[1] = &NameTable[1024];   
        PPU_Mem.name_table[2] = &NameTable[0];   
        PPU_Mem.name_table[3] = &NameTable[1024];   
    }   
   
    SpriteHitFlag = PPU_Latch_Flag = FALSE;   
//  PPU_BG_VScrlOrg = PPU_BG_VScrlOrg_Pre = 0;   
//  PPU_BG_HScrlOrg = PPU_BG_HScrlOrg_Pre = 0;   
    PPU_BG_VScrlOrg = 0;   
    PPU_BG_HScrlOrg = 0;   
//  PPU_BG_NameTableNum = 0;   
//  PPU_AddrTemp = 0;   
    PPU_scanline = 0;   

    tft_double_push_pxbuf(DisplayBuffer, 32, 0, 256, 240);
} 
   
void ppu_spr0_hit_flag(int y_axes)   
{   
    int   i,y_scroll, dy_axes, dx_axes;   
    uint8_t y_TitleLine, x_TitleLine;   
    uint8_t spr_size, Spr0_Data, temp;   
    uint8_t nNameTable, BG_TitlePatNum;   
    uint8_t BG_Data0, BG_Data1, BG_Data;   
    uint16_t title_addr;   
    const uint8_t *BG_Patterntable;   
    const uint8_t *Spr_Patterntable;   
   
    spr_size = PPU_Reg.R0 & R0_SPR_SIZE ? 0x0F : 0x07;       
    dy_axes = y_axes - (uint8_t)(sprite[0].y + 1);            
    if(dy_axes != (dy_axes & spr_size))            
        return;   
   
    nNameTable = PPU_Reg.R0 & R0_NAME_TABLE;   
    BG_Patterntable = PPU_Reg.R0 & BG_PATTERN_ADDR ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0; 
    y_scroll = y_axes + PPU_BG_VScrlOrg;    
    if(y_scroll > 239){   
        y_scroll -= 240;   
        nNameTable ^= NAME_TABLE_V_MASK;        
    }   
    y_TitleLine = y_scroll >> 3;             
    x_TitleLine = (PPU_BG_HScrlOrg + sprite[0].x) >> 3;
    dy_axes = y_scroll & 0x07; 
    dx_axes = PPU_BG_HScrlOrg & 0x07;
    if(x_TitleLine > 31)
        nNameTable ^= NAME_TABLE_H_MASK;   
    BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine]; 
    BG_Data0  = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];
    BG_Data0 |= BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
    if((x_TitleLine + 1) > 31)
        nNameTable ^= NAME_TABLE_H_MASK;   
    BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine + 1];
    BG_Data1  = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];
    BG_Data1 |= BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
    BG_Data = (BG_Data0 << dx_axes) | (BG_Data1 >> dx_axes); 

    if(sprite[0].attr & SPR_VFLIP)
        dy_axes = spr_size - dy_axes;                          
    if(PPU_Reg.R2 & R0_SPR_SIZE){//8*16
        Spr_Patterntable = (sprite[0].t_num & 0x01) ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
        title_addr = (sprite[0].t_num & 0XFE) << 4;
        Spr0_Data  = Spr_Patterntable[title_addr + dy_axes];   
        Spr0_Data |= Spr_Patterntable[title_addr + dy_axes + 8];   
    }else{                      //8*8   
        Spr_Patterntable = (PPU_Reg.R0 & SPR_PATTERN_ADDR)  ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
        title_addr = sprite[0].t_num  << 4; 
        Spr0_Data  = Spr_Patterntable[title_addr + dy_axes];   
        Spr0_Data |= Spr_Patterntable[title_addr + dy_axes + 8];   
    }   
   
    if(sprite[0].attr & SPR_HFLIP){  
        temp = 0;   
        for(i=0; i<8; i++){   
            temp |= (Spr0_Data >> i) & 1;   
            temp <<= i;   
        }   
        Spr0_Data = temp;   
    }   
    if(Spr0_Data & BG_Data){   
        SpriteHitFlag = TRUE;   
    }    
} 
static __forceinline void render_bg_line(int y_axes)   
{   
    int     i,y_scroll, /*x_scroll,*/ dy_axes, dx_axes;   
    int     Buffer_LineCnt, y_TitleLine, x_TitleLine;   
    uint8_t   H_byte, L_byte, BG_color_num, BG_attr_value;   
    uint8_t   nNameTable, BG_TitlePatNum;   
    const uint8_t  *BG_Patterntable;   
   
//  nNameTable = PPU_BG_NameTableNum; 
    nNameTable = PPU_Reg.R0 & R0_NAME_TABLE;   
    BG_Patterntable = PPU_Reg.R0 & BG_PATTERN_ADDR ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;
    y_scroll = y_axes + PPU_BG_VScrlOrg; 
    if(y_scroll > 239){   
        y_scroll -= 240;   
        nNameTable ^= NAME_TABLE_V_MASK;
    }   
    y_TitleLine = y_scroll >> 3;
    dy_axes = y_scroll & 0x07;  
//  x_scroll =  PPU_BG_HScrlOrg_Pre;           
    dx_axes = PPU_BG_HScrlOrg & 0x07;
    Buffer_LineCnt = 8 - dx_axes;
    for(x_TitleLine = PPU_BG_HScrlOrg >> 3; x_TitleLine < 32; x_TitleLine++){  
        BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine];
        L_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];
        H_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];         
        BG_attr_value = PPU_Mem.name_table[nNameTable][960 + ((y_TitleLine >> 2) << 3) + (x_TitleLine >> 2)]; 
        BG_attr_value = ((BG_attr_value >> (((y_TitleLine & 2) << 1) | (x_TitleLine & 2))) & 3) << 2;  
        for(i=7; i>=0; i--){ 
            BG_color_num = BG_attr_value;   
            BG_color_num |=(L_byte >> i) & 1;                
            BG_color_num |=((H_byte >> i) & 1) << 1;   
            if(BG_color_num & 3){
                Buffer_scanline[Buffer_LineCnt] =  NES_Color_Palette[PPU_Mem.image_palette[BG_color_num]];   
                Buffer_LineCnt++;   
            }else{   
                Buffer_LineCnt++;   
            }   
        }   
    } 
    nNameTable ^= NAME_TABLE_H_MASK;   
//  Buffer_LineCnt -= dx_axes;  
    for (x_TitleLine = 0; x_TitleLine <= (PPU_BG_HScrlOrg >> 3); x_TitleLine++ ){   
        BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine];
        L_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];                   
        H_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
        BG_attr_value = PPU_Mem.name_table[nNameTable][960 + ((y_TitleLine >> 2) << 3) + (x_TitleLine >> 2)];
        BG_attr_value = ((BG_attr_value >> (((y_TitleLine & 2) << 1) | (x_TitleLine & 2))) & 3) << 2; 
        for(i=7; i>=0; i--){   
            BG_color_num = BG_attr_value;                                 
            BG_color_num |=(L_byte >> i) & 1;                
            BG_color_num |=((H_byte >> i) & 1) << 1;   
            if(BG_color_num & 3){               
                Buffer_scanline[Buffer_LineCnt] = NES_Color_Palette[PPU_Mem.image_palette[BG_color_num]];   
                Buffer_LineCnt++;   
            }else{   
                Buffer_LineCnt++;   
            }   
        }   
    }   
}   

static __forceinline void render_sprite_pattern(SpriteType *sprptr, const uint8_t *Spr_Patterntable, uint16_t title_addr, uint8_t dy_axes)   
{   
    int   i, dx_axes;   
    uint8_t Spr_color_num, H_byte, L_byte;   
       
    if((PPU_Reg.R1 & R1_SPR_LEFT8 == 0) && sprptr -> x < 8){  
        dx_axes =  8 - sprptr -> x;   
        if(dx_axes == 0)    return;        
    }else{   
        dx_axes = 0;   
    }   
    if(sprptr -> attr & SPR_VFLIP){ 
        dy_axes = 7 - dy_axes;
    }   
    L_byte = Spr_Patterntable[title_addr + dy_axes];   
    H_byte = Spr_Patterntable[title_addr + dy_axes + 8];   
    if(sprptr -> attr & SPR_HFLIP){ 
        for(i=7; i>=dx_axes; i--){
            Spr_color_num  = (L_byte >> i) & 1;              //bit0   
            Spr_color_num |= ((H_byte >> i) & 1) << 1;       //bit1   
            if(Spr_color_num == 0)  continue;   
            Spr_color_num |= (sprptr -> attr & 0x03) << 2;   //bit23   
            Buffer_scanline[sprptr -> x + i + 8] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];
//          if(Spr_color_num & 0x03 && (PPU_Reg.R1 & R1_SPR_LEFT8 || (sprptr -> x + 8 - i) > 8)){
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];
//          }   
//          else{   
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[0]];       
//          }   
        }   
    }else{   
        for(i=7; i>=dx_axes; i--){ 
            Spr_color_num  = (L_byte >> (7-i)) & 1;               //bit0 
            Spr_color_num |= ((H_byte >> (7-i)) & 1) << 1;      //bit1   
            if(Spr_color_num == 0)  continue;   
            Spr_color_num |= (sprptr -> attr & 0x03) << 2;         //bit23   
            Buffer_scanline[sprptr -> x + i + 8] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];
//          if(Spr_color_num & 0x03 && (PPU_Reg.R1 & R1_SPR_LEFT8 || (sprptr -> x + 8 - i) > 8)){
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];
//          }   
//          else{   
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[0]];       
//          }      
        }    
    }   
}   
   
static __forceinline void render_sprite_88(SpriteType *sprptr, int dy_axes)   
{   
    const uint8_t  *Spr_Patterntable;     
    Spr_Patterntable = (PPU_Reg.R0 & SPR_PATTERN_ADDR)  ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
    render_sprite_pattern(sprptr, Spr_Patterntable, sprptr -> t_num << 4, (uint8_t)dy_axes);   
}   
   
static __forceinline void render_sprite_16(SpriteType *sprptr, int dy_axes)   
{   
    if(sprptr -> t_num & 0x01){                                         
        if(dy_axes < 8) 
            render_sprite_pattern(sprptr, PPU_Mem.patterntable1, (sprptr -> t_num & 0xFE) << 4, (uint8_t)dy_axes);       //��8*8   
        else   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable1, sprptr -> t_num << 4, (uint8_t)dy_axes & 7);        //��8*8   
    }else{   
        if(dy_axes < 8)  
            render_sprite_pattern(sprptr, PPU_Mem.patterntable0, sprptr -> t_num << 4, (uint8_t)dy_axes);            //��8*8   
        else   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable0, (sprptr -> t_num | 1) << 4, (uint8_t)dy_axes & 7);   //��8*8   
    }   
}   
   
void ppu_render_line(int y_axes)   
{   
    int i, render_spr_num, spr_size, dy_axes;   
//  MapperRenderScreen( 1 );   
   
    PPU_Reg.R2 &= ~R2_LOST_SPR;  
//  PPU_BG_VScrlOrg_Pre = PPU_BG_VScrlOrg;
//  PPU_BG_HScrlOrg_Pre = PPU_BG_HScrlOrg; 
   
    if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE)){ 
        for(i=7; i<256 ; i++){  
            Buffer_scanline[i] =  NES_Color_Palette[PPU_Mem.image_palette[0]];   
        }   
        spr_size = PPU_Reg.R0 & R0_SPR_SIZE ? 0x0F : 0x07;
        if(PPU_Reg.R1 & R1_SPR_VISIBLE){ 
            render_spr_num=0; 
            for(i=63; i>=0; i--){   
                if(!(sprite[i].attr & SPR_BG_PRIO)) continue;            //(0=Sprite In front of BG, 1=Sprite Behind BG)   
                dy_axes = y_axes - (uint8_t)(sprite[i].y + 1);  
                if(dy_axes != (dy_axes & spr_size)) continue; 
                render_spr_num++; 
                if(render_spr_num > 8 ){ 
                    PPU_Reg.R2 |= R2_LOST_SPR; 
                    break;   
                }   
                if(PPU_Reg.R0 & R0_SPR_SIZE){ 
                    render_sprite_16(&sprite[i], dy_axes);   
                }else{
                    render_sprite_88(&sprite[i], dy_axes);   
                }   
            }      
        }   

        if(PPU_Reg.R1 & R1_BG_VISIBLE){   
            render_bg_line(y_axes);    
        }     
        if(PPU_Reg.R1 & R1_SPR_VISIBLE){  
            render_spr_num=0; 
          for(i=63; i>=0; i--){ 
                if(sprite[i].attr & SPR_BG_PRIO)    continue;            //(0=Sprite In front of BG, 1=Sprite Behind BG)   
                dy_axes = y_axes - ((int)sprite[i].y + 1);    
                if(dy_axes != (dy_axes & spr_size)) continue; 
                render_spr_num++;          
                if(render_spr_num > 8 ){     
                    PPU_Reg.R2 |= R2_LOST_SPR;
                    break;   
                }   
                if(PPU_Reg.R0 & R0_SPR_SIZE){ 
                    render_sprite_16(&sprite[i], dy_axes);   
                }else{ 
                    render_sprite_88(&sprite[i], dy_axes);   
                }   
            }      
        }   
    }else{   
        for(i=8; i<264; i++){   
            Buffer_scanline[i] = 0;        
        }   
    }   
    // tft_print_nes_line(y_axes, Buffer_scanline);  
    // uint64_t* ptr = (void*)&Buffer_scanline[8];
    // uint64_t* end = (void*)&Buffer_scanline[262];
    // uint64_t* out = (void*)&DisplayBuffer[y_axes*256];
    // while (ptr != end) {
        // *out++ = *ptr++;
    // }
    for (int i=0; i<256; i++) { \
        DisplayBuffer[(y_axes)*256 + i] = Buffer_scanline[8+i]; \
    } 
}
   
/*******************************END OF FILE***********************************/   
