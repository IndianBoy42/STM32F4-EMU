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
/*  0x00 -> 0x75, 0x75, 0x75 */  0x73AE,    
/*  0x01 -> 0x27, 0x1B, 0x8F */  0x20D1,   
/*  0x02 -> 0x00, 0x00, 0xAB */  0x0015,   
/*  0x03 -> 0x47, 0x00, 0x9F */  0x4013,   
/*  0x04 -> 0x8F, 0x00, 0x77 */  0x880E,   
/*  0x05 -> 0xAB, 0x00, 0x13 */  0xA802,   
/*  0x06 -> 0xA7, 0x00, 0x00 */  0xA000,   
/*  0x07 -> 0x7F, 0x0B, 0x00 */  0x7840,   
/*  0x08 -> 0x43, 0x2F, 0x00 */  0x4160,   
/*  0x09 -> 0x00, 0x47, 0x00 */  0x0220,   
/*  0x0A -> 0x00, 0x51, 0x00 */  0x0280,   
/*  0x0B -> 0x00, 0x3F, 0x17 */  0x01E2,   
/*  0x0C -> 0x1B, 0x3F, 0x5F */  0x19EB,   
/*  0x0D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x0E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x0F -> 0x00, 0x00, 0x00 */  0x0000,   
   
/*  0x10 -> 0xBC, 0xBC, 0xBC */  0xBDF7,   
/*  0x11 -> 0x00, 0x73, 0xEF */  0x039D,   
/*  0x12 -> 0x23, 0x3B, 0xEF */  0x21DD,   
/*  0x13 -> 0x83, 0x00, 0xF3 */  0x801E,   
/*  0x14 -> 0xBF, 0x00, 0xBF */  0xB817,   
/*  0x15 -> 0xE7, 0x00, 0x5B */  0xE00B,   
/*  0x16 -> 0xDB, 0x2B, 0x00 */  0xD940,   
/*  0x17 -> 0xCB, 0x4F, 0x0F */  0xCA61,   
/*  0x18 -> 0x8B, 0x73, 0x00 */  0x8B80,   
/*  0x19 -> 0x00, 0x97, 0x00 */  0x04A0,   
/*  0x1A -> 0x00, 0xAB, 0x00 */  0x0540,   
/*  0x1B -> 0x00, 0x93, 0x3B */  0x0487,   
/*  0x1C -> 0x00, 0x83, 0x8B */  0x0411,   
/*  0x1D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x1E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x1F -> 0x00, 0x00, 00x0 */  0x0000,   
   
/*  0x20 -> 0xFF, 0xFF, 0xFF */  0xFFFF,   
/*  0x21 -> 0x3F, 0xBF, 0xFF */  0x3DFF,   
/*  0x22 -> 0x5F, 0x97, 0xFF */  0x5CBF,   
/*  0x23 -> 0xA7, 0x8B, 0xFD */  0xA45F,   
/*  0x24 -> 0xF7, 0x7B, 0xFF */  0xF3DF,   
/*  0x25 -> 0xFF, 0x77, 0xB7 */  0xFBB6,   
/*  0x26 -> 0xFF, 0x77, 0x63 */  0xFBAC,   
/*  0x27 -> 0xFF, 0x9B, 0x3B */  0xFCC7,   
/*  0x28 -> 0xF3, 0xBF, 0x3F */  0xF5E7,   
/*  0x29 -> 0x83, 0xD3, 0x13 */  0x8682,   
/*  0x2A -> 0x4F, 0xDF, 0x4B */  0x4EE9,   
/*  0x2B -> 0x58, 0xF8, 0x98 */  0x5FD3,   
/*  0x2C -> 0x00, 0xEB, 0xDB */  0x075B,   
/*  0x2D -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x2E -> 0x00, 0x00, 0x00 */  0x0000,   
/*  0x2F -> 0x00, 0x00, 0x00 */  0x0000,   
   
/*  0x30 -> 0xFF, 0xFF, 0xFF */  0xFFFF,   
/*  0x31 -> 0xAB, 0xE7, 0xFF */  0xAF3F,   
/*  0x32 -> 0xC7, 0xD7, 0xFF */  0xC6BF,   
/*  0x33 -> 0xD7, 0xCB, 0xFF */  0xD65F,   
/*  0x34 -> 0xFF, 0xC7, 0xFF */  0xFE3F,   
/*  0x35 -> 0xFF, 0xC7, 0xDB */  0xFE3B,   
/*  0x36 -> 0xFF, 0xBF, 0xB3 */  0xFDF6,   
/*  0x37 -> 0xFF, 0xDB, 0xAB */  0xFED5,   
/*  0x38 -> 0xFF, 0xE7, 0xA3 */  0xFF34,   
/*  0x39 -> 0xE3, 0xFF, 0xA3 */  0xE7F4,   
/*  0x3A -> 0xAB, 0xF3, 0xBF */  0xAF97,   
/*  0x3B -> 0xB3, 0xFF, 0xCF */  0xB7F9,   
/*  0x3C -> 0x9F, 0xFF, 0xF3 */  0x9FFE,   
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
void PPU_Init(  uint8_t* patterntableptr, /* Pattern table ��ַ*/   
                uint8_t  ScreenMirrorType /* ��Ļ��������*/   
                )   
{   
    memset(&PPU_Mem, 0, sizeof(PPU_Mem));//����洢��   
    memset(&Spr_Mem, 0, sizeof(Spr_Mem));   
    memset(&PPU_Reg, 0, sizeof(PPU_Reg));   
   
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
   
}   
   
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
 *  PPU �洢����Ĵ���������   
 */   
   
/*  
 * ��PPU name table ����  
 */   
uint8_t PPU_NameTablesRead(void)   
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
   
/*  
 * дPPU name table ����  
 */   
void PPU_NameTablesWrite(uint8_t value)   
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
   
/*  
 * дPPU�洢��  
 */   
void PPU_MemWrite(uint8_t value)   
{   
    switch(PPU_Mem.PPU_addrcnt & 0xF000){   
    case 0x0000: //$0000 ~ $0FFF  ֻ�� - �뿨���й�   
//       PPU_Mem.patterntable0[PPU_Mem.PPU_addrcnt] = value;   
         break;   
    case 0x1000: //$1000 ~ $1FFF  ֻ�� - �뿨���й�   
//       PPU_Mem.patterntable1[PPU_Mem.PPU_addrcnt & 0x0FFF] = value;   
         break;   
    case 0x2000: //$2000 ~ $2FFF   
         PPU_NameTablesWrite(value);   
         break;   
    case 0x3000:    
         //$3000 ~ $3EFF    -- $2000 ~ $2EFF�ľ���   
         //$3F00 ~ $3F0F    image  palette   
         //$3F10 ~ $3F1F    sprite palette   
         if((PPU_Mem.PPU_addrcnt & 0x1F) > 0x0F){   
            PPU_Mem.sprite_palette[(PPU_Mem.PPU_addrcnt & 0xF)] = value;    //������ɫ����ֵ��   
            if((PPU_Mem.PPU_addrcnt & 3) == 0){                         //��Ӧλ��Ϊ͸��ɫ�ľ���   
                PPU_Mem.sprite_palette[0] = PPU_Mem.image_palette[0] = value;   
                PPU_Mem.sprite_palette[4] = PPU_Mem.image_palette[4] = value;   
                PPU_Mem.sprite_palette[8] = PPU_Mem.image_palette[8] = value;   
                PPU_Mem.sprite_palette[12] =PPU_Mem.image_palette[12]= value;   
            }      
         }else{   
            PPU_Mem.image_palette[(PPU_Mem.PPU_addrcnt & 0xF)] = value;     //������ɫ����ֵ��   
         }   
//          PPU_NameTablesWrite(value);//name table����,һ�㲻ִ�е��˴�   
         break;   
    default: printf("д��PPU��ַ����$4000 %X", PPU_Mem.PPU_addrcnt);//��PPU_Mem.PPU_addrcnt & 0x3FFF   
    }   
    //��д�󣬵�ַ���������ӣ�����$2002 [bit2] 0��+1  1�� +32��   
    PPU_Reg.R0 & PPU_ADDRINCR ? PPU_Mem.PPU_addrcnt += 32 : PPU_Mem.PPU_addrcnt++ ;   
}   
   
/*  
 * ��PPU�洢��  
 */   
uint8_t PPU_MemRead(void)   
{   
    //����Ӳ��ԭ��NES PPUÿ�ζ�ȡ���ص��ǻ���ֵ��Ϊʱ����ȡ��ַ��1��   
    uint8_t temp;   
   
    temp = PPU_Mem.PPU_readtemp; //���滺��ֵ����Ϊ����ֵ   
           
    switch(PPU_Mem.PPU_addrcnt & 0xF000){   
    case 0x0000: //$0000 ~ $0FFF   
         PPU_Mem.PPU_readtemp = PPU_Mem.patterntable0[PPU_Mem.PPU_addrcnt];          //��ȡ��ַָ��ֵ������   
         break;   
    case 0x1000: //$1000 ~ $1FFF   
         PPU_Mem.PPU_readtemp = PPU_Mem.patterntable1[PPU_Mem.PPU_addrcnt & 0x0FFF]; //��ȡ��ַָ��ֵ������   
         break;   
    case 0x2000: //$2000 ~ $2FFF   
         PPU_Mem.PPU_readtemp = PPU_NameTablesRead();                                //��ȡ��ַָ��ֵ������   
         break;   
    case 0x3000:    
         //$3000 ~ $3EFF    -- $2000 ~ $2EFF�ľ���   
         //$3F00 ~ $3F0F image  palette   
         //$3F10 ~ $3F1F    sprite palette   
         if(PPU_Mem.PPU_addrcnt >= 0x3F10){   
            temp =  PPU_Mem.sprite_palette[(PPU_Mem.PPU_addrcnt & 0xF)];    //PPU ��ȡ���岻���� palette ��ɫ��,ֱ�ӷ���   
            break;     
         }   
         if(PPU_Mem.PPU_addrcnt >= 0x3F00){   
                temp = PPU_Mem.image_palette[(PPU_Mem.PPU_addrcnt & 0xF)];  //PPU ��ȡ���岻���� palette ��ɫ��,ֱ�ӷ���   
                break;   
         }   
//          temp = PPU_NameTablesRead();//name tables ����,һ�㲻ִ�е��˴�   
         break;   
    default: temp = 0;    
         printf("��ȡPPU��ַ����$4000 %X", PPU_Mem.PPU_addrcnt);   
    }   
    //��д�󣬵�ַ���������ӣ�����$2002 [bit2] 0��+1  1�� +32��   
    PPU_Reg.R0 & PPU_ADDRINCR ? PPU_Mem.PPU_addrcnt += 32 : PPU_Mem.PPU_addrcnt++ ;   
    return temp;   
}   
   
/*  
 * дPPU�Ĵ���  
 */   
void PPU_RegWrite(uint16_t RX, uint8_t value)   
{   
//#ifdef _NES_DEBUG_   
//  printf("\r\nPPU д�Ĵ��� %d %x", RX, value);   
//#endif   
    switch(RX){   
/*$2000*/   
    case 0: PPU_Reg.R0 = value;   
//          printf("\r\n PPU r0: %x", value);   
            // Account for Loopy's scrolling discoveries  �ο�InfoNes   
//          PPU_AddrTemp = ( PPU_AddrTemp & 0xF3FF ) | ( ( ( (uint16_t)value ) & 0x0003 ) << 10 );   
//          PPU_BG_NameTableNum = PPU_Reg.R0 & R0_NAME_TABLE;      
/*$2001*/   break;   
    case 1: PPU_Reg.R1 = value;     
/*$2003*/   break;   
    case 3: //Sprite Memory Address�� 8λ��ַ������   
            Spr_Mem.spr_addrcnt = value;   
/*$2004*/   break;   
    case 4: //Sprite Memory Data ,ÿ�δ�ȡ sprite ram ��ַ������spr_addrcnt�Զ���1   
            Spr_Mem.spr_ram[Spr_Mem.spr_addrcnt++] = value;   
/*$2005*/   break;   
    case 5: //PPU_Reg.R5 = value;   
            if(PPU_Latch_Flag){   //��1����ֱscroll����   
                PPU_BG_VScrlOrg = (value > 239) ? 0 : value;   
                //��ַ����ֵ�仯���ο�infones   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0xFC1F ) | ((((uint16_t)value) & 0xF8 ) << 2);   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0x8FFF ) | ((((uint16_t)value) & 0x07 ) << 12);   
            }else{                //��0��ˮƽscroll����   
                PPU_BG_HScrlOrg = value;   
                // Added : more Loopy Stuff  �ο�Infones   
//              PPU_AddrTemp = ( PPU_AddrTemp & 0xFFE0 ) | ((((uint16_t)value) & 0xF8 ) >> 3 );   
            }                       
            PPU_Latch_Flag ^= 1;   
/*$2006*/   break;   
    case 6:    
//          if(PPU_Latch_Flag){     //1   
////                PPU_Mem.PPU_addrcnt = (PPU_Mem.PPU_addrcnt << 8) + value; //PPU �洢����ַ����������д��8λ����д��8λ   
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
   
            PPU_Mem.PPU_addrcnt = (PPU_Mem.PPU_addrcnt << 8) + value; //PPU �洢����ַ����������д��8λ����д��8λ   
            PPU_Latch_Flag ^= 1;   
/*$2007*/   break;    
    case 7: /*д PPU Memory Data*/   
            PPU_MemWrite(value);   
            break;   
    default : printf("\r\nPPU д���ַ���� %d", RX);   
    }   
}   
   
/*  
 * ��PPU�Ĵ���  
 */   
uint8_t PPU_RegRead(uint16_t RX)   
{   
    uint8_t temp;   
   
    switch(RX){   
    case 0: temp = PPU_Reg.R0;  //$2000 RW   
            break;   
    case 1: temp = PPU_Reg.R1;  //$2001 RW   
            break;   
    case 2: temp = PPU_Reg.R2;   
            PPU_Reg.R2 &= ~(R0_VB_NMI_EN);   
            //��ȡ$2002����ԭPPU��ַ������д��ʱ��־   
            //ͬ������$2005 $2006д��״̬���Ʊ�־   
            PPU_Latch_Flag = 0;   
            // Make a Nametable 0 in V-Blank   
            if ((PPU_scanline > 20 && PPU_scanline < 262) && !(PPU_Reg.R0 & R0_VB_NMI_EN)){   
                PPU_Reg.R0 &= ~R0_NAME_TABLE;               //ѡ�� name table #0   
//              PPU_BG_NameTableNum = 0;                    //name table �� #0    
            }   
            break;; //$2002 R   
    case 4: /*�� Sprite Memory Data*/   
            temp = Spr_Mem.spr_ram[Spr_Mem.spr_addrcnt++];   
            break;   
    case 7: /*�� PPU Memory Data*/   
            temp = PPU_MemRead();   
            break;   
    default : printf("\r\nPPU ��ȡ��ַ���� %d", RX);   
            return RX;   
    }   
//#ifdef _NES_DEBUG_   
//  printf("\r\nPPU ���Ĵ��� %d %x", RX, temp);   
//#endif   
    return temp;       
}   
   
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
 *  PPU ��ʾ������   
 */   
   
/*  
 * ����sprite #0��ײ��־  
 */   
void NES_GetSpr0HitFlag(int y_axes)   
{   
    int   i,y_scroll, dy_axes, dx_axes;   
    uint8_t y_TitleLine, x_TitleLine;   
    uint8_t spr_size, Spr0_Data, temp;   
    uint8_t nNameTable, BG_TitlePatNum;   
    uint8_t BG_Data0, BG_Data1, BG_Data;   
    uint16_t title_addr;   
    uint8_t *BG_Patterntable;   
    uint8_t *Spr_Patterntable;   
   
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
void NES_RenderBGLine(int y_axes)   
{   
    int     i,y_scroll, /*x_scroll,*/ dy_axes, dx_axes;   
    int     Buffer_LineCnt, y_TitleLine, x_TitleLine;   
    uint8_t   H_byte, L_byte, BG_color_num, BG_attr_value;   
    uint8_t   nNameTable, BG_TitlePatNum;   
    uint8_t  *BG_Patterntable;   
   
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
void NES_RenderSprPattern(SpriteType *sprptr, uint8_t *Spr_Patterntable, uint16_t title_addr, uint8_t dy_axes)   
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
void NES_RenderSprite88(SpriteType *sprptr, int dy_axes)   
{   
    uint8_t  *Spr_Patterntable;      
    /*ȡ������title Pattern�׵�ַ*/   
    Spr_Patterntable = (PPU_Reg.R0 & SPR_PATTERN_ADDR)  ? PPU_Mem.patterntable1 : PPU_Mem.patterntable0;   
    NES_RenderSprPattern(sprptr, Spr_Patterntable, sprptr -> t_num << 4, (uint8_t)dy_axes);   
}   
   
/*  
 * sprite 8*16 ��ʾ����ɨ��  
 */            
void NES_RenderSprite16(SpriteType *sprptr, int dy_axes)   
{   
    if(sprptr -> t_num & 0x01){                                         
        if(dy_axes < 8)   //sprite  title ������   
            NES_RenderSprPattern(sprptr, PPU_Mem.patterntable1, (sprptr -> t_num & 0xFE) << 4, (uint8_t)dy_axes);       //��8*8   
        else   
            NES_RenderSprPattern(sprptr, PPU_Mem.patterntable1, sprptr -> t_num << 4, (uint8_t)dy_axes & 7);        //��8*8   
    }else{   
        if(dy_axes < 8)  //sprite  title ż����   
            NES_RenderSprPattern(sprptr, PPU_Mem.patterntable0, sprptr -> t_num << 4, (uint8_t)dy_axes);            //��8*8   
        else   
            NES_RenderSprPattern(sprptr, PPU_Mem.patterntable0, (sprptr -> t_num | 1) << 4, (uint8_t)dy_axes & 7);   //��8*8   
    }   
}   
   
/*  
 * PPU ��ʾһ��  
 */   
void NES_RenderLine(int y_axes)   
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
                    NES_RenderSprite16(&sprite[i], dy_axes);   
                }else{                                                  //��Ϊ�٣�sprite�Ĵ�С8*8   
                    NES_RenderSprite88(&sprite[i], dy_axes);   
                }   
            }      
        }   
       
        /* ɨ�豳�� background*/   
        if(PPU_Reg.R1 & R1_BG_VISIBLE){   
            NES_RenderBGLine(y_axes);                                   //ɨ�貢����Sprite #0��ײ��־   
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
                    NES_RenderSprite16(&sprite[i], dy_axes);   
                }else{                                                  //��Ϊ�٣�sprite�Ĵ�С8*8   
                    NES_RenderSprite88(&sprite[i], dy_axes);   
                }   
            }      
        }   
    }else{   
        for(i=8; i<264; i++){   
            //TODO: Buffer_scanline[i] = Black;                                 //�����ʾ����,����   
        }   
    }   
    /*���ɨ�裬������ʾ����д��LCD*/   
    //TODO: NES_LCD_DisplayLine(y_axes, Buffer_scanline);                                   //����LCD��ʾһ�У���ѯ��DMA����   
}   
   
/*  
 * PPU ���л��棬д��LCD  
 */   
void NES_LCD_DisplayLine(int y_axes, uint16_t *Disaplyline_buffer)   
{   
    uint32_t index;   
       
    //TODO: LCD_ConfigDispWindow(y_axes,y_axes,32,287);    
    //TODO: LCD_SetCursor(y_axes,287);   
    //TODO: LCD_GRAM_Prepare();   
    for(index = 8; index < 264; index++){   
        //TODO: LCD_RAM_Addr = Buffer_scanline[index];   
    }   
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

 