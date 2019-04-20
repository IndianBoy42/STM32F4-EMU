/**  
  ******************************************************************************  
  * @file      
  * @author    
  * @version   
  * @date      
  * @brief    
  ******************************************************************************  
  * @attention  
  *  
  *   
  ******************************************************************************  
  */     
   
/* Includes ------------------------------------------------------------------*/    
// #include "nes_main.h"   
#include "lcd_main.h"   
void tft_print_nes_line(int y, uint16_t* buf);
#include "ppu.h"

/*�������� ------------------------------------------------------------------*/   
/*�洢�����*/   
uint8_t NameTable[2048];   
   
PPU_RegType PPU_Reg;   
PPU_MemType PPU_Mem;   
   
Spr_MemType Spr_Mem;   
SpriteType  * const sprite = (SpriteType  *)&Spr_Mem.spr_ram[0]; //ָ���һ��sprite 0 ��λ��   
   
/*��ʾ���*/   
uint8_t   SpriteHitFlag, PPU_Latch_Flag; //sprite #0 ��ʾ��ײ������ɨ���к�, ����λ�ƣ�2005д���־    
int     PPU_scanline;                   //��ǰɨ����   
// uint16_t  DisplayBuffer[256*240] = {0};
uint16_t  Buffer_scanline[8 + 256 + 8];   //����ʾ����,���±�Խ�����Ϊ7����ʾ�� 7 ~ 263  0~7 263~270 Ϊ��ֹ�����   
   
uint8_t PPU_BG_VScrlOrg, PPU_BG_HScrlOrg;   
//uint8_t PPU_BG_VScrlOrg_Pre, PPU_BG_HScrlOrg_Pre;   
//uint8_t PPU_BG_NameTableNum;            //��ǰ������������   
//uint16_t PPU_AddrTemp;   
/********************************************************************************  
 **NES ��ɫ�� ��ɫ����RGB565�� 64  
 */   
const uint16_t NES_Color_Palette[64] ={   
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
/*  
 **  
 *********************************************************************************/   
   
   
/*�������� ----------------------------------------------------------------------*/   
   
/*  
 * PPU ��ʼ��  
 */   
void ppu_init(const uint8_t* patterntableptr, /* Pattern table ��ַ*/   
                uint8_t  ScreenMirrorType /* ��Ļ��������*/   
                )   
{   
    // memset(&PPU_Mem, 0, sizeof(PPU_Mem));//����洢��   
    // memset(&Spr_Mem, 0, sizeof(Spr_Mem));   
    // memset(&PPU_Reg, 0, sizeof(PPU_Reg));   
   
    PPU_Mem.patterntable0 =  patterntableptr;   
    PPU_Mem.patterntable1 =  patterntableptr + 0x1000;   
       
    if(ScreenMirrorType == 0){  //ˮƽ����   
        PPU_Mem.name_table[0] = &NameTable[0];   
        PPU_Mem.name_table[1] = &NameTable[0];   
        PPU_Mem.name_table[2] = &NameTable[1024];   
        PPU_Mem.name_table[3] = &NameTable[1024];   
    }else{                      //��ֱ����   
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

    // tft_circ_push_pxbuf(DisplayBuffer, 32, 0, 256, 240);
} 
   
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
 *  PPU ��ʾ������   
 */   
   
/*  
 * ����sprite #0��ײ��־  
 */   
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
   
    /*�ж�sprite #0 ��ʾ�����Ƿ��ڵ�ǰ��*/   
    spr_size = PPU_Reg.R0 & R0_SPR_SIZE ? 0x0F : 0x07;      //spr_size 8��0~7��16: 0~15   
    dy_axes = y_axes - (uint8_t)(sprite[0].y + 1);            //�ж�sprite#0 �Ƿ��ڵ�ǰ����ʾ��Χ��,0����ʵ��ֵΪFF   
    if(dy_axes != (dy_axes & spr_size))            
        return;   
   
    /*ȡ��sprite��ʾλ�õı�����ʾ����*/   
//  nNameTable = PPU_BG_NameTableNum;           //ȡ�õ�ǰ��Ļ��name table ��   
    nNameTable = PPU_Reg.R0 & R0_NAME_TABLE;   
    BG_Patterntable = PPU_Reg.R0 & BG_PATTERN_ADDR ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;//����pattern�׵�ַ   
    y_scroll = y_axes + PPU_BG_VScrlOrg;    //Scorll λ�ƺ󱳾���ʾ�е�Y���꣬����[00]��[01]��[10]��[11]��ÿ����ʾ(0+x_scroll_org,y_scroll_org)   
    if(y_scroll > 239){   
        y_scroll -= 240;   
        nNameTable ^= NAME_TABLE_V_MASK;        //��ֱ������Ļ���л�name table��   
    }   
    y_TitleLine = y_scroll >> 3;              //title �к� 0~29��y�������8��   
    x_TitleLine = (PPU_BG_HScrlOrg + sprite[0].x) >> 3;//title �к� 0~31��y�������8��    
    dy_axes = y_scroll & 0x07;                  //title��ʾy��ƫ������ ��8������   
    dx_axes = PPU_BG_HScrlOrg & 0x07;       //title��ʾx��ƫ������ ��8������   
    if(x_TitleLine > 31)                     //x�������ʾ   
        nNameTable ^= NAME_TABLE_H_MASK;   
    BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine];    //y_TitleLine * 32 +��x_TitleLine,��name����ȡ��sprite��ʾλ�õı�����title�ŵ�    
    BG_Data0  = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];                     //������ʾ����0   
    BG_Data0 |= BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
    if((x_TitleLine + 1) > 31)                   //x�������ʾ   
        nNameTable ^= NAME_TABLE_H_MASK;   
    BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine + 1]; //��name������һ��������ʾ��title�ŵ�   
    BG_Data1  = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];                     //������ʾ����1   
    BG_Data1 |= BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
    BG_Data = (BG_Data0 << dx_axes) | (BG_Data1 >> dx_axes);                            //������Sprite #0 λ����ͬ�ĵ�ǰ��ʾ�е���ʾ����   
       
    /*ȡ��sprite #0 ��ʾ����*/   
    if(sprite[0].attr & SPR_VFLIP)                          //����ֱ��ת   
        dy_axes = spr_size - dy_axes;                          
    if(PPU_Reg.R2 & R0_SPR_SIZE){//8*16                     //��Ϊ�棬sprite�Ĵ�С8*16   
        /*ȡ������title Pattern�׵�ַ*/   
        Spr_Patterntable = (sprite[0].t_num & 0x01) ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
        title_addr = (sprite[0].t_num & 0XFE) << 4;       //*16,ԭ��ַ��*2   
        Spr0_Data  = Spr_Patterntable[title_addr + dy_axes];   
        Spr0_Data |= Spr_Patterntable[title_addr + dy_axes + 8];   
    }else{                      //8*8   
        /*ȡ��sprite #0 ����title Pattern�׵�ַ*/   
        Spr_Patterntable = (PPU_Reg.R0 & SPR_PATTERN_ADDR)  ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
        title_addr = sprite[0].t_num  << 4;               //*16   
        Spr0_Data  = Spr_Patterntable[title_addr + dy_axes];   
        Spr0_Data |= Spr_Patterntable[title_addr + dy_axes + 8];   
    }   
   
    if(sprite[0].attr & SPR_HFLIP){             /*��ˮƽ��ת, ��ת�ߵ�λ����*/   
        temp = 0;   
        for(i=0; i<8; i++){   
            temp |= (Spr0_Data >> i) & 1;   
            temp <<= i;   
        }   
        Spr0_Data = temp;   
    }   
    if(Spr0_Data & BG_Data){   
//      printf("\r\nSprite #0 Hit!");   
        SpriteHitFlag = TRUE;   
    }    
}   
   
/*  
 * ��ʾһ�б���������sprite��ײ��������ײ��־  
 */   
__forceinline void render_bg_line(int y_axes)   
{   
    int     i,y_scroll, /*x_scroll,*/ dy_axes, dx_axes;   
    int     Buffer_LineCnt, y_TitleLine, x_TitleLine;   
    uint8_t   H_byte, L_byte, BG_color_num, BG_attr_value;   
    uint8_t   nNameTable, BG_TitlePatNum;   
    const uint8_t  *BG_Patterntable;   
   
//  nNameTable = PPU_BG_NameTableNum;           //ȡ�õ�ǰ��Ļ��name table ��   
    nNameTable = PPU_Reg.R0 & R0_NAME_TABLE;   
//  printf("\r\n name table num: %x", nNameTable);   
    BG_Patterntable = PPU_Reg.R0 & BG_PATTERN_ADDR ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;//����pattern�׵�ַ   
    y_scroll = y_axes + PPU_BG_VScrlOrg;    //Scorll λ�ƺ���ʾ�е�Y����   
    if(y_scroll > 239){   
        y_scroll -= 240;   
        nNameTable ^= NAME_TABLE_V_MASK;        //��ֱ������Ļ���л�name table��   
    }   
    y_TitleLine = y_scroll >> 3;              //title �к� 0~29��y�������8��   
    dy_axes = y_scroll & 0x07;                  //��8������   
//  x_scroll =  PPU_BG_HScrlOrg_Pre;           
    dx_axes = PPU_BG_HScrlOrg & 0x07;           //x��ƫ������   
       
    /*����ʾһ�е���߲���,�ӵ�һ�����ؿ�ʼɨ��*/   
    Buffer_LineCnt = 8 - dx_axes;               //����д��λ��(0~ 255)��8����ʾ��ʼ��   
    /*x_TitleLine ~ 31 ��������ʾ��8bitһ�У�*/   
    for(x_TitleLine = PPU_BG_HScrlOrg >> 3; x_TitleLine < 32; x_TitleLine++){         //��������һ����ʾtitle��Ԫ��ʼ   
//      printf("\r\n%d %d %d",y_axes, y_TitleLine, x_TitleLine);   
        BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine];    //y_TitleLine * 32,��ǰ��ʾ��title�ŵ�    
        L_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];                            //BG_TitlePatNum * 16 + dy_xaes   
        H_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
        //���Ա� �в��� ����λ��ɫ����ֵ                     ��4ȥ�����ٳ�8                ��4            
        BG_attr_value = PPU_Mem.name_table[nNameTable][960 + ((y_TitleLine >> 2) << 3) + (x_TitleLine >> 2)];//title��Ӧ�����Ա�8bitֵ   
         //��title��Ӧ�ĸ���λ����y title bit2  ���� 1λ��(����)�� ��x title bit2������ֵ [000][010][100][110] 0 2 4 6Ϊ��Ӧ��attr 8bit[0:1][2:3][4:5][6:7] �еĸ���λ��ɫֵ   
        BG_attr_value = ((BG_attr_value >> (((y_TitleLine & 2) << 1) | (x_TitleLine & 2))) & 3) << 2;            
        /*x��ÿ��ɨ��8������ʾ*/   
        for(i=7; i>=0; i--){                            //��д������ص���ɫ   
            //[1:0]����λ��ɫ����ֵ   
            BG_color_num = BG_attr_value;   
            BG_color_num |=(L_byte >> i) & 1;                
            BG_color_num |=((H_byte >> i) & 1) << 1;   
            if(BG_color_num & 3){             //�������λΪ0����Ϊ͸��ɫ,��д��   
                Buffer_scanline[Buffer_LineCnt] =  NES_Color_Palette[PPU_Mem.image_palette[BG_color_num]];   
                Buffer_LineCnt++;   
            }else{   
                Buffer_LineCnt++;   
            }   
        }   
    }   
    /*��ʾһ���ұ߲���, �л�name table��*/   
    nNameTable ^= NAME_TABLE_H_MASK;   
//  Buffer_LineCnt -= dx_axes;   
    /*�ұ�0 ~ PPU_BG_HScrlOrg_Pre >> 3*/   
    for (x_TitleLine = 0; x_TitleLine <= (PPU_BG_HScrlOrg >> 3); x_TitleLine++ ){   
        BG_TitlePatNum = PPU_Mem.name_table[nNameTable][(y_TitleLine << 5) + x_TitleLine]; //y_TitleLine * 32,��ǰ��ʾ��title�ŵ�    
        L_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes];                   
        H_byte = BG_Patterntable[(BG_TitlePatNum << 4) + dy_axes + 8];   
        BG_attr_value = PPU_Mem.name_table[nNameTable][960 + ((y_TitleLine >> 2) << 3) + (x_TitleLine >> 2)];//title��Ӧ�����Ա�8bitֵ   
        BG_attr_value = ((BG_attr_value >> (((y_TitleLine & 2) << 1) | (x_TitleLine & 2))) & 3) << 2;           //������ɫ[4:3]   
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
   
/*  
 * ��ʾһ��sprite��title 88  
 */   
__forceinline void render_sprite_pattern(SpriteType *sprptr, const uint8_t *Spr_Patterntable, uint16_t title_addr, uint8_t dy_axes)   
{   
    int   i, dx_axes;   
    uint8_t Spr_color_num, H_byte, L_byte;   
       
    if((PPU_Reg.R1 & R1_SPR_LEFT8 == 0) && sprptr -> x < 8){         //��ֹ��8��������ʾ   
        dx_axes =  8 - sprptr -> x;   
        if(dx_axes == 0)    return;        
    }else{   
        dx_axes = 0;   
    }   
    if(sprptr -> attr & SPR_VFLIP){      //����ֱ��ת   
        dy_axes = 7 - dy_axes;          //sprite 8*8��ʾdy_axes��   
    }   
    L_byte = Spr_Patterntable[title_addr + dy_axes];   
    H_byte = Spr_Patterntable[title_addr + dy_axes + 8];   
    if(sprptr -> attr & SPR_HFLIP){                                  //��ˮƽ��ת   
        for(i=7; i>=dx_axes; i--){                                   //��д�ұ� ��ɫ����   
            Spr_color_num  = (L_byte >> i) & 1;                       //bit0   
            Spr_color_num |= ((H_byte >> i) & 1) << 1;              //bit1   
            if(Spr_color_num == 0)  continue;   
            Spr_color_num |= (sprptr -> attr & 0x03) << 2;         //bit23   
            Buffer_scanline[sprptr -> x + i + 8] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]]; //ƫ��8   
//          if(Spr_color_num & 0x03 && (PPU_Reg.R1 & R1_SPR_LEFT8 || (sprptr -> x + 8 - i) > 8)){             //��ɫ�������2λΪ0��ʾ͸��,��ʾ�ױ�����$3F00��&&  �������룬0:������8����ʾ����                     
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];//д����ɫֵ������   
//          }   
//          else{   
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[0]];       
//          }   
        }   
    }else{   
        for(i=7; i>=dx_axes; i--){                               //��д�ұ� ��ɫ����   
            Spr_color_num  = (L_byte >> (7-i)) & 1;               //bit0   
            Spr_color_num |= ((H_byte >> (7-i)) & 1) << 1;      //bit1   
            if(Spr_color_num == 0)  continue;   
            Spr_color_num |= (sprptr -> attr & 0x03) << 2;         //bit23   
            Buffer_scanline[sprptr -> x + i + 8] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];//д����ɫֵ������   
//          if(Spr_color_num & 0x03 && (PPU_Reg.R1 & R1_SPR_LEFT8 || (sprptr -> x + 8 - i) > 8)){             //��ɫ�������2λΪ0��ʾ͸��,��ʾ�ױ�����$3F00��&&  �������룬0:������8����ʾ����                     
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[Spr_color_num]];//д����ɫֵ������   
//          }   
//          else{   
//              Buffer_scanline[sprptr -> x + 8 - i] =  NES_Color_Palette[PPU_Mem.sprite_palette[0]];       
//          }      
        }    
    }   
}   
   
/*  
 * sprite 8*8 ��ʾ����ɨ��  
 */   
__forceinline void render_sprite_88(SpriteType *sprptr, int dy_axes)   
{   
    const uint8_t  *Spr_Patterntable;      
    /*ȡ������title Pattern�׵�ַ*/   
    Spr_Patterntable = (PPU_Reg.R0 & SPR_PATTERN_ADDR)  ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
    render_sprite_pattern(sprptr, Spr_Patterntable, sprptr -> t_num << 4, (uint8_t)dy_axes);   
}   
   
/*  
 * sprite 8*16 ��ʾ����ɨ��  
 */            
__forceinline void render_sprite_16(SpriteType *sprptr, int dy_axes)   
{   
    if(sprptr -> t_num & 0x01){                                         
        if(dy_axes < 8)   //sprite  title ������   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable1, (sprptr -> t_num & 0xFE) << 4, (uint8_t)dy_axes);       //��8*8   
        else   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable1, sprptr -> t_num << 4, (uint8_t)dy_axes & 7);        //��8*8   
    }else{   
        if(dy_axes < 8)  //sprite  title ż����   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable0, sprptr -> t_num << 4, (uint8_t)dy_axes);            //��8*8   
        else   
            render_sprite_pattern(sprptr, PPU_Mem.patterntable0, (sprptr -> t_num | 1) << 4, (uint8_t)dy_axes & 7);   //��8*8   
    }   
}   
   
/*  
 * PPU ��ʾһ��  
 */   
void ppu_render_line(int y_axes)   
{   
    int i, render_spr_num, spr_size, dy_axes;   
    /* MMC5 VROM switch -- VROM�洢���л� */   
//  MapperRenderScreen( 1 );   
   
    PPU_Reg.R2 &= ~R2_LOST_SPR;                                         //����PPU״̬�Ĵ���R2 SPR LOST�ı�־λ   
//  PPU_BG_VScrlOrg_Pre = PPU_BG_VScrlOrg;                              //���� ��ֱ scroll    
//  PPU_BG_HScrlOrg_Pre = PPU_BG_HScrlOrg;                              //���� ˮƽ scroll   
   
    if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE)){                  //��Ϊ�٣��ر���ʾ����0��   
        /*�����ʾ���棬�ڴ����õױ���ɫ����ȷ����*/   
        for(i=7; i<256 ; i++){                       //��ʾ�� 7 ~ 263  0~7 263~270 Ϊ��ֹ�����   
            Buffer_scanline[i] =  NES_Color_Palette[PPU_Mem.image_palette[0]];   
        }   
        spr_size = PPU_Reg.R0 & R0_SPR_SIZE ? 0x0F : 0x07;              //spr_size 8��0~7��16: 0~15   
        /* ɨ�豳��sprite��ת������ʾ����д�뵽����,ÿһ�����ֻ����ʾ8��Sprite*/   
        if(PPU_Reg.R1 & R1_SPR_VISIBLE){                                //������sprite��ʾ   
            render_spr_num=0;                                           //������ʾ������   
            for(i=63; i>=0; i--){                                        //���ص�sprites 0 ������ʾ������ȼ����������ȼ�˳���֮������������ʾ������ȼ�   
                /*�ж���ʾ�㣨�ǣ� ����*/   
                if(!(sprite[i].attr & SPR_BG_PRIO)) continue;            //(0=Sprite In front of BG, 1=Sprite Behind BG)   
                /*�ж���ʾλ��*/   
                dy_axes = y_axes - (uint8_t)(sprite[i].y + 1);            //�ж�sprite�Ƿ��ڵ�ǰ����ʾ��Χ��,sprite y (FF,00,01,...EE)(0~239)   
                if(dy_axes != (dy_axes & spr_size)) continue;           //�������򷵻ؼ���ѭ��������һ��   
                /*������sprite�ڵ�ǰ��ʾ��,��ת��������ʾ�׶�*/   
                render_spr_num++;                                       //����ʾ��sprite����Ŀ+1   
                if(render_spr_num > 8 ){                             //һ�г���8��spreite������ѭ��   
                    PPU_Reg.R2 |= R2_LOST_SPR;                          //����PPU״̬�Ĵ���R2�ı�־λ   
                    break;   
                }   
                if(PPU_Reg.R0 & R0_SPR_SIZE){                           //��Ϊ�棬sprite�Ĵ�С8*16   
                    render_sprite_16(&sprite[i], dy_axes);   
                }else{                                                  //��Ϊ�٣�sprite�Ĵ�С8*8   
                    render_sprite_88(&sprite[i], dy_axes);   
                }   
            }      
        }   
       
        /* ɨ�豳�� background*/   
        if(PPU_Reg.R1 & R1_BG_VISIBLE){   
            render_bg_line(y_axes);                                   //ɨ�貢����Sprite #0��ײ��־   
        }   
       
        /* ɨ��ǰ��sprite��ת������ʾ����д�뵽����,ÿһ�����ֻ����ʾ8��Sprite*/   
        if(PPU_Reg.R1 & R1_SPR_VISIBLE){                                //������sprite��ʾ   
            render_spr_num=0;                                           //������ʾ������   
        /* ���ص�sprites 0 ������ʾ������ȼ����������ȼ�˳���֮������������ʾ������ȼ�  
         * ��ע����ǰ��sprites ���ȼ����ڱ������ȼ����ص�����ɫ��ǰ�����ȼ����ڱ������ȼ��Ļ���ǰ����������ʾ(��δ����)*/   
            for(i=63; i>=0; i--){                                           
                /*�ж���ʾ�� ǰ��*/   
                if(sprite[i].attr & SPR_BG_PRIO)    continue;            //(0=Sprite In front of BG, 1=Sprite Behind BG)   
                /*�ж���ʾλ��*/   
                dy_axes = y_axes - ((int)sprite[i].y + 1);              //�ж�sprite�Ƿ��ڵ�ǰ����ʾ��Χ��,sprite y (FF,00,01,...EE)(0~239)   
                if(dy_axes != (dy_axes & spr_size)) continue;           //�������򷵻ؼ���ѭ��������һ��   
                /*������sprite�ڵ�ǰ��ʾ��,��ת��������ʾ�׶�*/   
                render_spr_num++;                                       //����ʾ��sprite����Ŀ+1   
                if(render_spr_num > 8 ){                             //һ�г���8��spreite������ѭ��   
                    PPU_Reg.R2 |= R2_LOST_SPR;                          //����PPU״̬�Ĵ���R2�ı�־λ   
                    break;   
                }   
                if(PPU_Reg.R0 & R0_SPR_SIZE){                           //��Ϊ�棬sprite�Ĵ�С8*16   
                    render_sprite_16(&sprite[i], dy_axes);   
                }else{                                                  //��Ϊ�٣�sprite�Ĵ�С8*8   
                    render_sprite_88(&sprite[i], dy_axes);   
                }   
            }      
        }   
    }else{   
        for(i=8; i<264; i++){   
            Buffer_scanline[i] = 0;                                 //�����ʾ����,����   
        }   
    }   
    /*���ɨ�裬������ʾ����д��LCD*/   

    tft_print_nes_line(y_axes, Buffer_scanline);                                   //����LCD��ʾһ�У���ѯ��DMA����   
    // for (int i=0; i<256; i++) {
        // DisplayBuffer[(y_axes)*256 + i] = Buffer_scanline[i];
    // } 
}
   
///*   
// * PPU��ʾ�رգ����ױ���д��LCD       
// */   
//void NES_LCD_BG_DisplayLine(uint16_t color)   
//{   
//   
//}   
  /**  
  * @}  
  */   
   
   
/*******************************END OF FILE***********************************/   
