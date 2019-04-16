#include "cpu.h" 
#include "nes.h"
// #include "PPU.h" 

   
//flags = NVRBDIZC   
#define C_FLAG      0x01        // 1: Carry   
#define Z_FLAG      0x02        // 1: Zero   
#define I_FLAG      0x04        // 1: Irq disabled   
#define D_FLAG      0x08        // 1: Decimal mode flag (NES unused)   
#define B_FLAG      0x10        // 1: Break   
#define R_FLAG      0x20        // 1: Reserved (Always 1)   
#define V_FLAG      0x40        // 1: Overflow   
#define N_FLAG      0x80        // 1: Negative   
   
// /* flags = NVRBDIZC */ 
uint32_t pc_reg = 0;   
   
/*�жϱ�־*/   
CPU_InitFlag NMI_Flag;   
CPU_InitFlag IRQ_Flag;   

NESRegisters reg;

/* Macros for convenience */   
#define A  reg.A   
#define X  reg.X 
#define Y  reg.Y 
#define P  reg.P 
#define S  reg.S 
#define PC reg.PC 
   
/* internal registers */   
uint8_t opcode;   
int  cpu_clockticks;   
   
/* help variables */   
uint16_t savepc;   
uint8_t value;   
int  sum, saveflags;   
uint16_t help;   
   
/*  memory map */   
uint8_t  cpu_ram[0x800];  /* RAM*/   
//uint8_t* ppu_regbase;       /* PPU IO reg*/   
//uint8_t* apu_regbase;       /* APU IO reg*/   
uint8_t* exp_rom;         /* expansion rom*/   
uint8_t* sram;            /* sram*/   
uint8_t* prg_rombank0;    /* prg-rom lower bank*/   
uint8_t* prg_rombank1;    /* prg-rom upper bank*/   
   
/* arrays */   
typedef struct{   
    unsigned short int  ticks;   
    void (*instruction)(void);   
    void (*adrmode)(void);   
}OPCODE;   
    
const OPCODE opcodetable[]={   
/*0x00*/    {7, cpu_brk,    cpu_implied},   
/*0x01*/    {6, cpu_ora,    cpu_indx},   
/*0x02*/    {2, cpu_nop,    cpu_implied},   
/*0x03*/    {2, cpu_nop,    cpu_implied},   
/*0x04*/    {3, cpu_tsb,    cpu_zp},   
/*0x05*/    {3, cpu_ora,    cpu_zp},   
/*0x06*/    {5, cpu_asl,    cpu_zp},   
/*0x07*/    {2, cpu_nop,    cpu_implied},   
/*0x08*/    {3, cpu_php,    cpu_implied},   
/*0x09*/    {3, cpu_ora,    cpu_immediate},   
/*0x0A*/    {2, cpu_asla,   cpu_implied},   
/*0x0B*/    {2, cpu_ora,    cpu_implied},   
/*0x0C*/    {4, cpu_tsb,    cpu_abs},   
/*0x0D*/    {4, cpu_ora,    cpu_abs},   
/*0x0E*/    {6, cpu_asl,    cpu_abs},   
/*0x0F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x10*/    {2, cpu_bpl,    cpu_relative},   
/*0x11*/    {5, cpu_ora,    cpu_indy},   
/*0x12*/    {3, cpu_ora,    cpu_indzp},   
/*0x13*/    {2, cpu_nop,    cpu_implied},   
/*0x14*/    {3, cpu_trb,    cpu_zp},   
/*0x15*/    {4, cpu_ora,    cpu_zpx},   
/*0x16*/    {6, cpu_asl,    cpu_zpx},   
/*0x17*/    {2, cpu_nop,    cpu_implied},   
/*0x18*/    {2, cpu_clc,    cpu_implied},   
/*0x19*/    {4, cpu_ora,    cpu_absy},   
/*0x1A*/    {2, cpu_ina,    cpu_implied},   
/*0x1B*/    {2, cpu_nop,    cpu_implied},   
/*0x1C*/    {4, cpu_trb,    cpu_abs},   
/*0x1D*/    {4, cpu_ora,    cpu_absx},   
/*0x1E*/    {7, cpu_asl,    cpu_absx},   
/*0x1F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x20*/    {6, cpu_jsr,    cpu_abs},   
/*0x21*/    {6, cpu_and,    cpu_indx},   
/*0x22*/    {2, cpu_nop,    cpu_implied},   
/*0x23*/    {2, cpu_nop,    cpu_implied},   
/*0x24*/    {3, cpu_bit,    cpu_zp},   
/*0x25*/    {3, cpu_and,    cpu_zp},   
/*0x26*/    {5, cpu_rol,    cpu_zp},   
/*0x27*/    {2, cpu_nop,    cpu_implied},   
/*0x28*/    {4, cpu_plp,    cpu_implied},   
/*0x29*/    {3, cpu_and,    cpu_immediate},   
/*0x2A*/    {2, cpu_rola,   cpu_implied},   
/*0x2B*/    {2, cpu_nop,    cpu_implied},   
/*0x2C*/    {4, cpu_bit,    cpu_abs},   
/*0x2D*/    {4, cpu_and,    cpu_abs},   
/*0x2E*/    {6, cpu_rol,    cpu_abs},   
/*0x2F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x30*/    {2, cpu_bmi,    cpu_relative},   
/*0x31*/    {5, cpu_and,    cpu_indy},   
/*0x32*/    {3, cpu_and,    cpu_indzp},   
/*0x33*/    {2, cpu_nop,    cpu_implied},   
/*0x34*/    {4, cpu_bit,        cpu_zpx},   
/*0x35*/    {4, cpu_and,    cpu_zpx},   
/*0x36*/    {6, cpu_rol,    cpu_zpx},   
/*0x37*/    {2, cpu_nop,    cpu_implied},   
/*0x38*/    {2, cpu_sec,    cpu_implied},   
/*0x39*/    {4, cpu_and,    cpu_absy},   
/*0x3A*/    {2, cpu_dea,    cpu_implied},   
/*0x3B*/    {2, cpu_nop,    cpu_implied},   
/*0x3C*/    {4, cpu_bit,    cpu_absx},   
/*0x3D*/    {4, cpu_and,    cpu_absx},   
/*0x3E*/    {7, cpu_rol,    cpu_absx},   
/*0x3F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x40*/    {6, cpu_rti,    cpu_implied},   
/*0x41*/    {6, cpu_eor,    cpu_indx},   
/*0x42*/    {2, cpu_nop,    cpu_implied},   
/*0x43*/    {2, cpu_nop,    cpu_implied},   
/*0x44*/    {2, cpu_nop,    cpu_implied},   
/*0x45*/    {3, cpu_eor,    cpu_zp},   
/*0x46*/    {5, cpu_lsr,    cpu_zp},   
/*0x47*/    {2, cpu_nop,    cpu_implied},   
/*0x48*/    {3, cpu_pha,    cpu_implied},   
/*0x49*/    {3, cpu_eor,    cpu_immediate},   
/*0x4A*/    {2, cpu_lsra,   cpu_implied},   
/*0x4B*/    {2, cpu_nop,    cpu_implied},   
/*0x4C*/    {3, cpu_jmp,    cpu_abs},   
/*0x4D*/    {4, cpu_eor,    cpu_abs},   
/*0x4E*/    {6, cpu_lsr,    cpu_abs},   
/*0x4F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x50*/    {2, cpu_bvc,    cpu_relative},   
/*0x51*/    {5, cpu_eor,    cpu_indy},   
/*0x52*/    {3, cpu_eor,    cpu_indzp},   
/*0x53*/    {2, cpu_nop,    cpu_implied},   
/*0x54*/    {2, cpu_nop,    cpu_implied},   
/*0x55*/    {4, cpu_eor,    cpu_zpx},   
/*0x56*/    {6, cpu_lsr,    cpu_zpx},   
/*0x57*/    {2, cpu_nop,    cpu_implied},   
/*0x58*/    {2, cpu_cli,    cpu_implied},   
/*0x59*/    {4, cpu_eor,    cpu_absy},   
/*0x5A*/    {3, cpu_phy,    cpu_implied},   
/*0x5B*/    {2, cpu_nop,    cpu_implied},   
/*0x5C*/    {2, cpu_nop,    cpu_implied},   
/*0x5D*/    {4, cpu_eor,    cpu_absx},   
/*0x5E*/    {7, cpu_lsr,    cpu_absx},   
/*0x5F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x60*/    {6, cpu_rts,    cpu_implied},   
/*0x61*/    {6, cpu_adc,    cpu_indx},   
/*0x62*/    {2, cpu_nop,    cpu_implied},   
/*0x63*/    {2, cpu_nop,    cpu_implied},   
/*0x64*/    {3, cpu_stz,    cpu_zp},   
/*0x65*/    {3, cpu_adc,    cpu_zp},   
/*0x66*/    {5, cpu_ror,    cpu_zp},   
/*0x67*/    {2, cpu_nop,    cpu_implied},   
/*0x68*/    {4, cpu_pla,    cpu_implied},   
/*0x69*/    {3, cpu_adc,    cpu_immediate},   
/*0x6A*/    {2, cpu_rora,   cpu_implied},   
/*0x6B*/    {2, cpu_nop,    cpu_implied},   
/*0x6C*/    {5, cpu_jmp,    cpu_indirect},   
/*0x6D*/    {4, cpu_adc,    cpu_abs},   
/*0x6E*/    {6, cpu_ror,    cpu_abs},   
/*0x6F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x70*/    {2, cpu_bvs,    cpu_implied},   
/*0x71*/    {5, cpu_adc,    cpu_indy},   
/*0x72*/    {3, cpu_adc,    cpu_indzp},   
/*0x73*/    {2, cpu_nop,    cpu_implied},   
/*0x74*/    {4, cpu_stz,    cpu_zpx},   
/*0x75*/    {4, cpu_adc,    cpu_zpx},   
/*0x76*/    {6, cpu_ror,    cpu_zpx},   
/*0x77*/    {2, cpu_nop,    cpu_implied},   
/*0x78*/    {2, cpu_sei,    cpu_implied},   
/*0x79*/    {4, cpu_adc,    cpu_absy},   
/*0x7A*/    {4, cpu_ply,    cpu_implied},   
/*0x7B*/    {2, cpu_nop,    cpu_implied},   
/*0x7C*/    {6, cpu_jmp,    cpu_indabsx},   
/*0x7D*/    {4, cpu_adc,    cpu_absx},   
/*0x7E*/    {7, cpu_ror,    cpu_absx},   
/*0x7F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x80*/    {2, cpu_bra,    cpu_relative},   
/*0x81*/    {6, cpu_sta,    cpu_indx},   
/*0x82*/    {2, cpu_nop,    cpu_implied},   
/*0x83*/    {2, cpu_nop,    cpu_implied},   
/*0x84*/    {2, cpu_sty,    cpu_zp},   
/*0x85*/    {2, cpu_sta,    cpu_zp},   
/*0x86*/    {2, cpu_stx,    cpu_zp},   
/*0x87*/    {2, cpu_nop,    cpu_implied},   
/*0x88*/    {2, cpu_dey,    cpu_implied},   
/*0x89*/    {2, cpu_bit,    cpu_immediate},   
/*0x8A*/    {2, cpu_txa,    cpu_implied},   
/*0x8B*/    {2, cpu_nop,    cpu_implied},   
/*0x8C*/    {4, cpu_sty,    cpu_abs},   
/*0x8D*/    {4, cpu_sta,    cpu_abs},   
/*0x8E*/    {4, cpu_stx,    cpu_abs},   
/*0x8F*/    {2, cpu_nop,    cpu_implied},   
   
/*0x90*/    {2, cpu_bcc,    cpu_relative},   
/*0x91*/    {6, cpu_sta,    cpu_indy},   
/*0x92*/    {3, cpu_sta,    cpu_indzp},   
/*0x93*/    {2, cpu_nop,    cpu_implied},   
/*0x94*/    {4, cpu_sty,    cpu_zpx},   
/*0x95*/    {4, cpu_sta,    cpu_zpx},   
/*0x96*/    {4, cpu_stx,    cpu_zpy},   
/*0x97*/    {2, cpu_nop,    cpu_implied},   
/*0x98*/    {2, cpu_tya,    cpu_implied},   
/*0x99*/    {5, cpu_sta,    cpu_absy},   
/*0x9A*/    {2, cpu_txs,    cpu_implied},   
/*0x9B*/    {2, cpu_nop,    cpu_implied},   
/*0x9C*/    {4, cpu_stz,    cpu_abs},   
/*0x9D*/    {5, cpu_sta,    cpu_absx},   
/*0x9E*/    {5, cpu_stz,    cpu_absx},   
/*0x9F*/    {2, cpu_nop,    cpu_implied},   
   
/*0xA0*/    {3, cpu_ldy,    cpu_immediate},   
/*0xA1*/    {6, cpu_lda,    cpu_indx},   
/*0xA2*/    {3, cpu_ldx,    cpu_immediate},   
/*0xA3*/    {2, cpu_nop,    cpu_implied},   
/*0xA4*/    {3, cpu_ldy,    cpu_zp},   
/*0xA5*/    {3, cpu_lda,    cpu_zp},   
/*0xA6*/    {3, cpu_ldx,    cpu_zp},   
/*0xA7*/    {2, cpu_nop,    cpu_implied},   
/*0xA8*/    {2, cpu_tay,    cpu_implied},   
/*0xA9*/    {3, cpu_lda,    cpu_immediate},   
/*0xAA*/    {2, cpu_tax,    cpu_implied},   
/*0xAB*/    {2, cpu_nop,    cpu_implied},   
/*0xAC*/    {4, cpu_ldy,    cpu_abs},   
/*0xAD*/    {4, cpu_lda,    cpu_abs},   
/*0xAE*/    {4, cpu_ldx,    cpu_abs},   
/*0xAF*/    {2, cpu_nop,    cpu_implied},   
   
/*0xB0*/    {2, cpu_bcs,    cpu_relative},   
/*0xB1*/    {5, cpu_lda,    cpu_indy},   
/*0xB2*/    {3, cpu_lda,    cpu_indzp},   
/*0xB3*/    {2, cpu_nop,    cpu_implied},   
/*0xB4*/    {4, cpu_ldy,    cpu_zpx},   
/*0xB5*/    {4, cpu_lda,    cpu_zpx},   
/*0xB6*/    {4, cpu_ldx,    cpu_zpy},   
/*0xB7*/    {2, cpu_nop,    cpu_implied},   
/*0xB8*/    {2, cpu_clv,    cpu_implied},   
/*0xB9*/    {4, cpu_lda,    cpu_absy},   
/*0xBA*/    {2, cpu_tsx,    cpu_implied},   
/*0xBB*/    {2, cpu_nop,    cpu_implied},   
/*0xBC*/    {4, cpu_ldy,    cpu_absx},   
/*0xBD*/    {4, cpu_lda,    cpu_absx},   
/*0xBE*/    {4, cpu_ldx,    cpu_absy},   
/*0xBF*/    {2, cpu_nop,    cpu_implied},   
   
/*0xC0*/    {3, cpu_cpy,    cpu_immediate},   
/*0xC1*/    {6, cpu_cmp,    cpu_indx},   
/*0xC2*/    {2, cpu_nop,    cpu_implied},   
/*0xC3*/    {2, cpu_nop,    cpu_implied},   
/*0xC4*/    {3, cpu_cpy,    cpu_zp},   
/*0xC5*/    {3, cpu_cmp,    cpu_zp},   
/*0xC6*/    {5, cpu_dec,    cpu_zp},   
/*0xC7*/    {2, cpu_nop,    cpu_implied},   
/*0xC8*/    {2, cpu_iny,    cpu_implied},   
/*0xC9*/    {3, cpu_cmp,    cpu_immediate},   
/*0xCA*/    {2, cpu_dex,    cpu_implied},   
/*0xCB*/    {2, cpu_nop,    cpu_implied},   
/*0xCC*/    {4, cpu_cpy,    cpu_abs},   
/*0xCD*/    {4, cpu_cmp,    cpu_abs},   
/*0xCE*/    {6, cpu_dec,    cpu_abs},   
/*0xCF*/    {2, cpu_nop,    cpu_implied},   
   
/*0xD0*/    {2, cpu_bne,    cpu_relative},   
/*0xD1*/    {5, cpu_cmp,    cpu_indy},   
/*0xD2*/    {3, cpu_cmp,    cpu_indzp},   
/*0xD3*/    {2, cpu_nop,    cpu_implied},   
/*0xD4*/    {2, cpu_nop,    cpu_implied},   
/*0xD5*/    {4, cpu_cmp,    cpu_zpx},   
/*0xD6*/    {6, cpu_dec,    cpu_zpx},   
/*0xD7*/    {2, cpu_nop,    cpu_implied},   
/*0xD8*/    {2, cpu_cld,    cpu_implied},   
/*0xD9*/    {4, cpu_cmp,    cpu_absy},   
/*0xDA*/    {3, cpu_phx,    cpu_implied},   
/*0xDB*/    {2, cpu_nop,    cpu_implied},   
/*0xDC*/    {2, cpu_nop,    cpu_implied},   
/*0xDD*/    {4, cpu_cmp,    cpu_absx},   
/*0xDE*/    {7, cpu_dec,    cpu_absx},   
/*0xDF*/    {2, cpu_nop,    cpu_implied},   
   
/*0xE0*/    {3, cpu_cpx,    cpu_immediate},   
/*0xE1*/    {6, cpu_sbc,    cpu_indx},   
/*0xE2*/    {2, cpu_nop,    cpu_implied},   
/*0xE3*/    {2, cpu_nop,    cpu_implied},   
/*0xE4*/    {3, cpu_cpx,    cpu_zp},   
/*0xE5*/    {3, cpu_sbc,    cpu_zp},   
/*0xE6*/    {5, cpu_inc,    cpu_zp},   
/*0xE7*/    {2, cpu_nop,    cpu_implied},   
/*0xE8*/    {2, cpu_inx,    cpu_implied},   
/*0xE9*/    {3, cpu_sbc,    cpu_immediate},   
/*0xEA*/    {2, cpu_nop,    cpu_implied},   
/*0xEB*/    {2, cpu_nop,    cpu_implied},   
/*0xEC*/    {4, cpu_cpx,    cpu_abs},   
/*0xED*/    {4, cpu_sbc,    cpu_abs},   
/*0xEE*/    {6, cpu_inc,    cpu_abs},   
/*0xEF*/    {2, cpu_nop,    cpu_implied},   
   
/*0xF0*/    {2, cpu_beq,    cpu_relative},   
/*0xF1*/    {5, cpu_sbc,    cpu_indy},   
/*0xF2*/    {3, cpu_sbc,    cpu_indzp},   
/*0xF3*/    {2, cpu_nop,    cpu_implied},   
/*0xF4*/    {2, cpu_nop,    cpu_implied},   
/*0xF5*/    {4, cpu_sbc,    cpu_zpx},   
/*0xF6*/    {6, cpu_inc,    cpu_zpx},   
/*0xF7*/    {2, cpu_nop,    cpu_implied},   
/*0xF8*/    {2, cpu_sed,    cpu_implied},   
/*0xF9*/    {4, cpu_sbc,    cpu_absy},   
/*0xFA*/    {4, cpu_plx,    cpu_implied},   
/*0xFB*/    {2, cpu_nop,    cpu_implied},   
/*0xFC*/    {2, cpu_nop,    cpu_implied},   
/*0xFD*/    {4, cpu_sbc,    cpu_absx},   
/*0xFE*/    {7, cpu_inc,    cpu_absx},   
/*0xFF*/    {2, cpu_nop,    cpu_implied}   
};   
   
/******************************************************************************  
 ******************************************************************************  
 *  
 cpu_*                               �洢�������д  
 *  
 ******************************************************************************  
 ******************************************************************************  
 */   
   
/*  
 * sprite DMA (д��ַ��$4014, ����8λ��ԭ���ݵ�ַ)  
 */   
void SprDMA(uint8_t scr_addr)  
{   
    uint8_t *scr_addrptr = 0;  
    int     i;   
   
    switch(scr_addr >> 4){  
    case 0x0: //RAM   
    case 0x1: scr_addrptr = &cpu_ram[(scr_addr << 8)];   
              break;   
    case 0x6: //SRAM   
    case 0x7:    
#ifdef CHECK_POINTER   
              check_pointer(sram);    
#endif   
              scr_addrptr = sram + ((scr_addr << 8) - 0x6000);   
              break;   
    case 0x8:
    case 0x9:   
    case 0xA:   
    case 0xB:   
#ifdef CHECK_POINTER   
              check_pointer(prg_rombank0);    
#endif   
              scr_addrptr = &prg_rombank0[(scr_addr << 8) - 0x8000];   
              break;   
    case 0xC:   
    case 0xD:   
    case 0xE:   
    case 0xF:   
#ifdef CHECK_POINTER   
              check_pointer(prg_rombank1);    
#endif   
              scr_addrptr = &prg_rombank1[(scr_addr << 8) - 0xC000];   
              break;   
    }   
   
#ifdef CKECK_POINTER   
    if(scr_addrptr == NULL){   
        printf("\r\nSprite ram DMA ��ַ��������");   
        return;   
    }   
#endif   
    for(i=0; i<256; i++){   
        //TODO: Spr_Mem.spr_ram[i] = scr_addrptr[i];   
    }     
   
    cpu_clockticks -= 512;
}   
   
/*  
 * cpu_��ȡ�洢��  
 */   
int cpu_getmemory(uint16_t addr)//û0x2000����   
{   
    switch(addr & 0xE000){   
    case 0x0000:    //$0000 ~ $0FFF    
         return (cpu_ram[addr&0x7FF]);   
//  case 0x1000:    //$1000 ~ $1FFF ����   
//           return (cpu_ram[addr&0x7FF]);   
////         break;   
    case 0x2000:    //$2000 ~ $2FFF   
         //TODO: return PPU_RegRead(addr&0x7);  /*PPU IO�ӿ�*/      
//  case 0x3000:    //$3000 ~ $3FFF     ����   
////         break;   
    case 0x4000:    //$4000 ~ $4FFF  //�ݲ�����   
         /*APU �������� IO�ӿ� ��չROM*/   
         if(addr < 0x4013){  
//          return (apu_regbase[addr&0x1F]);  //�ݲ�����   
         }   
         if(addr == 0x4014){   
//          //SpriteRAM_DMA();break; ֻ֧��д   
         }   
         if(addr == 0x4015){ //pAPU Sound / Vertical Clock Signal Register   
         }   
         if(addr == 0x4016){   
            //TODO: return NES_GetJoyPadVlaue(JOYPAD_0);//��Ϊ3��8bit���ݣ���һ��8bitΪ������1,�ڶ��ο�����3��������ID code   
         }   
         if(addr == 0x4017){   
            //TODO: return NES_GetJoyPadVlaue(JOYPAD_1);//��Ϊ3��8bit���ݣ���һ��8bitΪ������2,�ڶ��ο�����4��������ID code   
         }   
         break;   
//  case 0x5000:    /* expansion rom*/   
//#ifdef     CKECK_POINTER   
//       check_pointer(exp_rom);    
//#endif   
//       return (exp_rom[addr - 0x4020]);   
    case 0x6000:    /*����SRAM��һ�������洢��Ϸ����*/   
#ifdef   CKECK_POINTER   
         check_pointer(cpu_ram);    
#endif   
         //�ݲ�����      
         break;   
    case 0x8000:    /*����洢�� ֻ�� */   
    case 0xA000:   
#ifdef   CHECK_POINTER   
         check_pointer(prg_rombank0);    
#endif   
         return prg_rombank0[addr-0x8000];   
    case 0xC000:   
    case 0xE000:   
#ifdef   CHECK_POINTER   
         check_pointer(prg_rombank1);    
#endif   
         return prg_rombank1[addr-0xC000];   
//       break;   
    }   
      
    return (addr >> 8);/*ͨ������ִ�е��˴�,���ص�ַ�߰�λ  
                         APU��SRAMδ��ɣ�����ִ�е��˴�*/    
}   
   
/*  
 * cpu_д�洢��  
 */   
void cpu_putmemory(uint16_t addr, uint8_t value)   
{   
    switch(addr & 0xE000){   
    case 0x0000:   
         cpu_ram[addr&0x7FF] = value;   
         break;   
    case 0x2000:    /*PPU IO�ӿ�*/   
         //TODO: PPU_RegWrite(addr&0x7, value);   
         break;   
    case 0x4000:    
         if(addr == 0x4014){               
            SprDMA(value);    
         }   
         if(addr == 0x4016){      
            /*bit0 JoyPad ����*/   
            if(value & 1){     //��дbit0:1��λ����״̬����дbit0:0��ֹ   
                //TODO: NES_JoyPadReset();   
            }else{   
//              NES_JoyPadDisable();   //��д1��д0����λ������   
            }   
            /*����λ��δʹ��*/                 
         }   
//       if(addr == 0x4017){   
//          NES_SetJoyPad(value);   
//       }   
//       if(addr >= 0x4020){   
//          exp_rom[addr - 0x6000] = value;   
//       }   
         //����Ĵ�����δ����   
         break;   
    case 0x6000:    /*����SRAM��һ�������洢��Ϸ����*/   
//#ifdef     CHECK_POINTER   
//       check_pointer(sram);    
//#endif   
//       sram[addr - 0x6000] = value; //��洢���л��й�     
         break;   
    case 0x8000:    /*����洢�� ֻ��(д��ʱ��洢���л�(mapper)�йأ�����)*/   
    case 0xA000:   
    case 0xC000:   
    case 0xE000:   
//       // Write to Mapper   
//           MapperWrite( wAddr, byData );   
         break;   
   }   
}   
   
/******************************************************************************  
 ******************************************************************************  
 *  
 *                              Ѱַģʽ  
 *  
 ******************************************************************************  
 ******************************************************************************  
 */   
   
/* Adressing modes */   
/* Implied */   
void cpu_implied(void)   
{   
}   
   
/* #Immediate */   
void cpu_immediate(void)   
{   
      savepc=PC++;   
}   
   
/* ABS */   
void cpu_abs(void)   
{   
//      savepc = gameImage[PC] + (gameImage[PC + 1] << 8);   
    savepc  = cpu_getmemory(PC);   
    savepc += (cpu_getmemory(PC+1) << 8);   
   
    PC++;   
    PC++;   
}   
   
/* Branch */   
void cpu_relative(void)   
{   
//      savepc = gameImage[PC++];   
    savepc  = cpu_getmemory(PC);   
    PC++;   
       
    if (savepc & 0x80) savepc -= 0x100;   
    if ((savepc>>8) != (PC>>8))   
        cpu_clockticks++;   
}   
   
/* (ABS) */   
void cpu_indirect(void)   
{   
//      help = gameImage[PC] + (gameImage[PC + 1] << 8);   
//      savepc = gameImage[help] + (gameImage[help + 1] << 8);   
    help    =  cpu_getmemory(PC);   
    help   += (cpu_getmemory(PC+1) << 8);   
    savepc  =  cpu_getmemory(help);   
    savepc += (cpu_getmemory(help+1) << 8);   
   
    PC++;   
    PC++;   
}   
   
/* ABS,X */   
void cpu_absx(void)   
{   
//      savepc = gameImage[PC] + (gameImage[PC + 1] << 8);   
    savepc  = cpu_getmemory(PC);   
    savepc += (cpu_getmemory(PC+1) << 8);   
   
    PC++;   
    PC++;   
    if (opcodetable[opcode].ticks==4)   
        if ((savepc>>8) != ((savepc+X)>>8))   
            cpu_clockticks++;   
    savepc += X;   
}   
   
/* ABS,Y */   
void cpu_absy(void)   
{   
//      savepc = gameImage[PC] + (gameImage[PC + 1] << 8);   
    savepc  = cpu_getmemory(PC);   
    savepc += (cpu_getmemory(PC+1) << 8);   
   
    PC++;   
    PC++;   
       
    if (opcodetable[opcode].ticks==4)   
          if ((savepc>>8) != ((savepc+Y)>>8))   
                  cpu_clockticks++;   
    savepc += Y;   
}   
   
/* ZP */   
void cpu_zp(void)   
{   
//  savepc=gameImage[PC++];   
    savepc  = cpu_getmemory(PC);   
    PC++;   
}   
   
/* ZP,X */   
void cpu_zpx(void)   
{   
//  savepc=gameImage[PC++]+X;   
    savepc  = cpu_getmemory(PC) + X;   
    PC++;   
   
    savepc &= 0x00ff;   
}   
   
/* ZP,Y */   
void cpu_zpy(void)   
{   
//      savepc=gameImage[PC++]+Y;   
    savepc  = cpu_getmemory(PC) + Y;   
    PC++;   
   
    savepc &= 0x00ff;   
}   
   
/* (ZP,X) */   
void cpu_indx(void)   
{   
//      value = gameImage[PC++]+X;   
//      savepc = gameImage[value] + (gameImage[value+1] << 8);   
    value = cpu_getmemory(PC) + Y;   
    PC++;      
    savepc  = cpu_getmemory(value);     
    savepc += cpu_getmemory(value + 1) << 8;    
}   
   
/* (ZP),Y */   
void cpu_indy(void)   
{   
//  value = gameImage[PC++];   
//  savepc = gameImage[value] + (gameImage[value+1] << 8);   
    value = cpu_getmemory(PC);   
    PC++;   
    savepc  = cpu_getmemory(value);     
    savepc += cpu_getmemory(value + 1) << 8;     
   
    if (opcodetable[opcode].ticks==5)   
          if ((savepc>>8) != ((savepc+Y)>>8))   
                  cpu_clockticks++;   
    savepc += Y;   
}   
   
/* (ABS,X) */   
void cpu_indabsx(void)   
{   
//      help = gameImage[PC] + (gameImage[PC + 1] << 8) + X;   
//      savepc = gameImage[help] + (gameImage[help + 1] << 8);   
    help = cpu_getmemory(PC);   
    help = (cpu_getmemory(PC+1) << 8) +X;   
    savepc  = cpu_getmemory(help);     
    savepc += cpu_getmemory(help + 1) << 8;   
}   
   
/* (ZP) */   
void cpu_indzp(void)   
{   
//      value = gameImage[PC++];   
//      savepc = gameImage[value] + (gameImage[value + 1] << 8);   
    value = cpu_getmemory(PC);   
    PC++;   
    savepc  = cpu_getmemory(value);     
    savepc += cpu_getmemory(value + 1) << 8;   
}   
   
/******************************************************************************  
 ******************************************************************************  
 *  
 *                              ָ��  
 *  
 ******************************************************************************  
 ******************************************************************************  
 */   
/* Instructions */   
void cpu_adc(void)   
{   
    opcodetable[opcode].adrmode();   
//      value = gameImage[savepc];   
    value = cpu_getmemory(savepc);   
       
    saveflags=(P & 0x01);   
    sum= ((char) A) + ((char) value) + saveflags;   
    if ((sum>0x7f) || (sum<-0x80)) P |= 0x40; else P &= 0xbf;   
    sum= A + value + saveflags;   
    if (sum>0xff) P |= 0x01; else P &= 0xfe;   
    A=sum;   
    if (P & 0x08)   
    {   
          P &= 0xfe;   
          if ((A & 0x0f)>0x09)   
                  A += 0x06;   
          if ((A & 0xf0)>0x90)   
          {   
                  A += 0x60;   
                  P |= 0x01;   
          }   
    }   
    else   
    {   
          cpu_clockticks++;   
    }   
    if (A) P &= 0xfd; else P |= 0x02;   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_and(void)   
{   
    opcodetable[opcode].adrmode();   
//      value = gameImage[savepc];   
    value = cpu_getmemory(savepc);   
       
    A &= value;   
    if (A) P &= 0xfd; else P |= 0x02;   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_asl(void)   
{   
      opcodetable[opcode].adrmode();   
      value = cpu_getmemory(savepc);   
      P= (P & 0xfe) | ((value >>7) & 0x01);   
      value = value << 1;   
      cpu_putmemory(savepc,value);   
      if (value) P &= 0xfd; else P |= 0x02;   
      if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_asla(void)   
{   
      P= (P & 0xfe) | ((A >>7) & 0x01);   
      A = A << 1;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_bcc(void)   
{   
    if ((P & 0x01)==0)   
    {   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_bcs(void)   
{   
    if (P & 0x01){   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_beq(void)   
{   
    if (P & 0x02){   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_bit(void)   
{   
    opcodetable[opcode].adrmode();   
//  value=gameImage[savepc];   
    value = cpu_getmemory(savepc);   
       
    /* non-destrucive logically And between value and the accumulator  
    * and set zero flag */   
    if (value & A) P &= 0xfd; else P |= 0x02;   
       
    /* set negative and overflow flags from value */   
    P = (P & 0x3f) | (value & 0xc0);   
}   
   
void cpu_bmi(void)   
{   
    if (P & 0x80){   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_bne(void)   
{   
    if ((P & 0x02)==0){   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_bpl(void)   
{   
    if ((P & 0x80)==0)  {   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_brk(void)   
{   
    PC++;   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
    cpu_putmemory(0x0100+S--,P);   
      P |= 0x14;   
//      PC = gameImage[0xfffe & addrmask] + (gameImage[0xffff & addrmask] << 8);   
    PC = cpu_getmemory(0xFFFE) + (cpu_getmemory(0xFFFF) << 8);   
}   
   
void cpu_bvc(void)   
{   
    if ((P & 0x40)==0){   
        opcodetable[opcode].adrmode();   
        PC += savepc;   
        cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_bvs(void)   
{   
    if (P & 0x40){   
          opcodetable[opcode].adrmode();   
          PC += savepc;   
          cpu_clockticks++;   
    }else{   
//      value=gameImage[PC++];   
        value = cpu_getmemory(PC);   
        PC++;   
    }   
}   
   
void cpu_clc(void)   
{   
    P &= 0xfe;   
}   
   
void cpu_cld(void)   
{   
    P &= 0xf7;   
}   
   
void cpu_cli(void)   
{   
    P &= 0xfb;   
}   
   
void cpu_clv(void)   
{   
    P &= 0xbf;   
}   
   
void cpu_cmp(void)   
{   
    opcodetable[opcode].adrmode();   
    value = cpu_getmemory(savepc);   
    if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
    value=A+0x100-value;   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_cpx(void)   
{   
    opcodetable[opcode].adrmode();   
    value = cpu_getmemory(savepc);   
    if (X+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
    value=X+0x100-value;   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_cpy(void)   
{   
    opcodetable[opcode].adrmode();   
    value = cpu_getmemory(savepc);   
    if (Y+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
    value=Y+0x100-value;   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_dec(void)   
{   
    uint8_t temp;    
   
    opcodetable[opcode].adrmode();   
//  gameImage[savepc]--;   
    temp = cpu_getmemory(savepc);   
    temp--;   
    cpu_putmemory(savepc, temp);   
   
    value = cpu_getmemory(savepc);   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_dex(void)   
{   
      X--;   
      if (X) P &= 0xfd; else P |= 0x02;   
      if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_dey(void)   
{   
      Y--;   
      if (Y) P &= 0xfd; else P |= 0x02;   
      if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_eor(void)   
{   
    opcodetable[opcode].adrmode();   
//  A ^= gameImage[savepc];   
    A ^= cpu_getmemory(savepc);   
    if (A) P &= 0xfd; else P |= 0x02;   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_inc(void)   
{   
    uint8_t temp;     
       
    opcodetable[opcode].adrmode();   
//      gameImage[savepc]++;   
    temp = cpu_getmemory(savepc);   
    temp++;   
    cpu_putmemory(savepc,temp);   
   
    value = cpu_getmemory(savepc);   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_inx(void)   
{   
      X++;   
      if (X) P &= 0xfd; else P |= 0x02;   
      if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_iny(void)   
{   
      Y++;   
      if (Y) P &= 0xfd; else P |= 0x02;   
      if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_jmp(void)   
{   
      opcodetable[opcode].adrmode();   
      PC=savepc;   
}   
   
void cpu_jsr(void)   
{   
      PC++;   
      cpu_putmemory(0x0100+S--,(uint8_t)(PC >> 8));   
      cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
      PC--;   
      opcodetable[opcode].adrmode();   
      PC=savepc;   
}   
   
void cpu_lda(void)   
{   
    opcodetable[opcode].adrmode();   
//  A=gameImage[savepc];   
    A = cpu_getmemory(savepc);   
    // set the zero flag   
    if (A) P &= 0xfd; else P |= 0x02;   
    // set the negative flag   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_ldx(void)   
{   
    opcodetable[opcode].adrmode();   
//  X=gameImage[savepc];   
    X = cpu_getmemory(savepc);   
    if (X) P &= 0xfd; else P |= 0x02;   
    if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_ldy(void)   
{   
    opcodetable[opcode].adrmode();   
//      Y=gameImage[savepc];   
    Y = cpu_getmemory(savepc);   
    if (Y) P &= 0xfd; else P |= 0x02;   
    if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_lsr(void)   
{   
    opcodetable[opcode].adrmode();   
//      value=gameImage[savepc];   
    value = cpu_getmemory(savepc);   
       
    /* set carry flag if shifting right causes a bit to be lost */   
    P= (P & 0xfe) | (value & 0x01);   
       
    value = value >>1;   
    cpu_putmemory(savepc,value);   
       
    /* set zero flag if value is zero */   
    if (value != 0) P &= 0xfd; else P |= 0x02;   
       
    /* set negative flag if bit 8 set??? can this happen on an LSR? */   
    if ((value & 0x80) == 0x80)   
     P |= 0x80;   
    else   
     P &= 0x7f;   
}   
   
void cpu_lsra(void)   
{   
      P= (P & 0xfe) | (A & 0x01);   
      A = A >>1;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_nop(void)   
{   
}   
   
void cpu_ora(void)   
{   
    opcodetable[opcode].adrmode();   
//      A |= gameImage[savepc];   
    A |= cpu_getmemory(savepc);    
       
    if (A) P &= 0xfd; else P |= 0x02;   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_pha(void)   
{   
//      gameImage[0x100+S--] = A;   
    cpu_putmemory(0x100 + S-- , A);   
}   
   
void cpu_php(void)   
{   
//      gameImage[0x100+S--] = P;   
     cpu_putmemory(0x100 + S--, P);   
}   
   
void cpu_pla(void)   
{   
//      A=gameImage[++S+0x100];   
      A = cpu_getmemory(++S + 0x100);   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_plp(void)   
{   
//      P=gameImage[++S+0x100] | 0x20;   
    P = cpu_getmemory(++S + 0x100) | 0x20;   
}   
   
void cpu_rol(void)   
{   
      saveflags=(P & 0x01);   
      opcodetable[opcode].adrmode();   
      value = cpu_getmemory(savepc);   
      P= (P & 0xfe) | ((value >>7) & 0x01);   
      value = value << 1;   
      value |= saveflags;   
      cpu_putmemory(savepc,value);   
      if (value) P &= 0xfd; else P |= 0x02;   
      if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_rola(void)   
{   
      saveflags=(P & 0x01);   
      P= (P & 0xfe) | ((A >>7) & 0x01);   
      A = A << 1;   
      A |= saveflags;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_ror(void)   
{   
    saveflags=(P & 0x01);   
    opcodetable[opcode].adrmode();   
//      value=gameImage[savepc];   
    value = cpu_getmemory(savepc);   
   
    P= (P & 0xfe) | (value & 0x01);   
    value = value >>1;   
    if (saveflags) value |= 0x80;   
    cpu_putmemory(savepc,value);   
    if (value) P &= 0xfd; else P |= 0x02;   
    if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_rora(void)   
{   
      saveflags=(P & 0x01);   
      P= (P & 0xfe) | (A & 0x01);   
      A = A >>1;   
      if (saveflags) A |= 0x80;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_rti(void)   
{   
//      P=gameImage[++S+0x100] | 0x20;   
//      PC=gameImage[++S+0x100];   
//      PC |= (gameImage[++S+0x100] << 8);   
    P   = cpu_getmemory(++S + 0x100);   
    P  |= 0x20;   
    PC  = cpu_getmemory(++S + 0x100);   
    PC |= (cpu_getmemory(++S + 0x100) << 8);   
}   
   
void cpu_rts(void)   
{   
//      PC=gameImage[++S+0x100];   
//      PC |= (gameImage[++S+0x100] << 8);   
//      PC++;   
    PC  = cpu_getmemory(++S + 0x100);   
    PC |= (cpu_getmemory(++S + 0x100) << 8);   
    PC++;   
}   
   
void cpu_sbc(void)   
{   
    opcodetable[opcode].adrmode();   
//      value = gameImage[savepc] ^ 0xff;   
    value = cpu_getmemory(savepc) ^ 0xFF;   
       
    saveflags=(P & 0x01);   
    sum= ((char) A) + ((char) value) + (saveflags << 4);   
    if ((sum>0x7f) || (sum<-0x80)) P |= 0x40; else P &= 0xbf;   
    sum= A + value + saveflags;   
    if (sum>0xff) P |= 0x01; else P &= 0xfe;   
    A=sum;   
    if (P & 0x08)   
    {   
          A -= 0x66;     
          P &= 0xfe;   
          if ((A & 0x0f)>0x09)   
                  A += 0x06;   
          if ((A & 0xf0)>0x90)   
          {   
                  A += 0x60;   
                  P |= 0x01;   
          }   
    }   
    else   
    {   
          cpu_clockticks++;   
    }   
    if (A) P &= 0xfd; else P |= 0x02;   
    if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_sec(void)   
{   
      P |= 0x01;   
}   
   
void cpu_sed(void)   
{   
      P |= 0x08;   
}   
   
void cpu_sei(void)   
{   
      P |= 0x04;   
}   
   
void cpu_sta(void)   
{   
      opcodetable[opcode].adrmode();   
      cpu_putmemory(savepc,A);   
}   
   
void cpu_stx(void)   
{   
      opcodetable[opcode].adrmode();   
      cpu_putmemory(savepc,X);   
}   
   
void cpu_sty(void)   
{   
      opcodetable[opcode].adrmode();   
      cpu_putmemory(savepc,Y);   
}   
   
void cpu_tax(void)   
{   
      X=A;   
      if (X) P &= 0xfd; else P |= 0x02;   
      if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_tay(void)   
{   
      Y=A;   
      if (Y) P &= 0xfd; else P |= 0x02;   
      if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_tsx(void)   
{   
      X=S;   
      if (X) P &= 0xfd; else P |= 0x02;   
      if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_txa(void)   
{   
      A=X;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_txs(void)   
{   
      S=X;   
}   
   
void cpu_tya(void)   
{   
      A=Y;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_bra(void)   
{   
      opcodetable[opcode].adrmode();   
      PC += savepc;   
      cpu_clockticks++;   
}   
   
void cpu_dea(void)   
{   
      A--;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_ina(void)   
{   
      A++;   
      if (A) P &= 0xfd; else P |= 0x02;   
      if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_phx(void)   
{   
      cpu_putmemory(0x100+S--,X);   
}   
   
void cpu_plx(void)   
{   
//  X=gameImage[++S+0x100];   
    X = cpu_getmemory(++S + 0x100);   
   
    if (X) P &= 0xfd; else P |= 0x02;   
    if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_phy(void)   
{   
      cpu_putmemory(0x100+S--, Y);   
}   
   
void cpu_ply(void)   
{   
//  Y=gameImage[++S+0x100];   
    Y = cpu_getmemory(++S + 0x100);   
   
    if (Y) P &= 0xfd; else P |= 0x02;   
    if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   
   
void cpu_stz(void)   
{   
      opcodetable[opcode].adrmode();   
      cpu_putmemory(savepc,0);   
}   
   
void cpu_tsb(void)   
{   
    uint8_t temp;    
   
    opcodetable[opcode].adrmode();   
//      gameImage[savepc] |= A;   
//      if (gameImage[savepc]) P &= 0xfd; else P |= 0x02;   
    temp  = cpu_getmemory(savepc);   
    temp |= A;   
    cpu_putmemory(savepc, temp);   
   
    if(cpu_getmemory(savepc))P &= 0xfd; else P |= 0x02;   
}   
   
void cpu_trb(void)   
{   
    uint8_t temp;       
   
    opcodetable[opcode].adrmode();   
//      gameImage[savepc] = gameImage[savepc] & (A ^ 0xff);   
//      if (gameImage[savepc]) P &= 0xfd; else P |= 0x02;   
    temp  = cpu_getmemory(savepc);   
    temp &= (A ^ 0xFF);   
    cpu_putmemory(savepc, temp);   
   
    if(cpu_getmemory(savepc))P &= 0xfd; else P |= 0x02;   
}   
   
   
/* Reset CPU */   
void cpu_reset(void)   
{   
    A=X=Y=P=0;   
    P |= 0x20;       //Z_FLAG | R_FLAG   
    S=0xff;   
//  PC  = cpu_getmemory(0xfffc & addrmask);   
//  PC |= cpu_getmemory(0xfffd & addrmask) << 8;   
    PC  = cpu_getmemory(0xfffc);  //��λ��ַ�����ж�ȡPC   
    PC |= cpu_getmemory(0xfffd) << 8;   
   
    NMI_Flag = CLR0;   
    IRQ_Flag = CLR0;   
}   
   
/* Non maskerable interrupt */   
void cpu_nmi(void)   
{   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
    cpu_putmemory(0x0100+S--,P);   
    P |= 0x04;   
//  PC=cpu_getmemory(0xfffa & addrmask);   
//  PC |= cpu_getmemory(0xfffb & addrmask) << 8;   
    PC=cpu_getmemory(0xfffa);   
    PC |= cpu_getmemory(0xfffb) << 8;   
}   
   
/* Maskerable Interrupt */   
void cpu_irq(void)   
{   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
    cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
    cpu_putmemory(0x0100+S--,P);   
    P |= 0x04;   
//   PC=cpu_getmemory(0xfffe & addrmask);   
//   PC |= cpu_getmemory(0xffff & addrmask) << 8;   
    PC=cpu_getmemory(0xfffe);   
    PC |= cpu_getmemory(0xffff) << 8;   
}   
   
/******************************************************************************  
 ******************************************************************************  
 *  
 *                              ִ��ָ��  
 *  
 ******************************************************************************  
 ******************************************************************************  
 */   
/* Execute Instruction */   
   
void cpu_exec(int timerTicks)   
{   
//  while (timerTicks > 0)    
//  {   
//          // fetch instruction   
////      opcode = gameImage[PC++];   
//      opcode = cpu_getmemory(PC);   
//      PC++;   
//   
//      // execute instruction �ú���ָ�����Switch�Ľ�ִ��Ч�ʵķ���   
//      opcodetable[opcode].instruction();   
//         
//      // calculate clock cycles   
//      cpu_clockticks += opcodetable[opcode].ticks;   
//      timerTicks -= cpu_clockticks;   
//      cpu_clockticks = 0;   
//  }   
    /*�жϼ��*/   
    if(NMI_Flag == SET1){   
         cpu_nmi();   
         NMI_Flag = CLR0;   
    }else{   
        if(IRQ_Flag == SET1 && !(P & I_FLAG)){   
            cpu_irq();   
            IRQ_Flag = CLR0;   
        }   
    }   
   
    while (timerTicks > cpu_clockticks)    
    {   
        opcode = cpu_getmemory(PC);   
        PC++;   
        opcodetable[opcode].instruction();   
        cpu_clockticks += opcodetable[opcode].ticks;   
    }   
    cpu_clockticks -= timerTicks;   
}   
   
void cpu_initmem( uint8_t* exp_romptr,   
                  uint8_t* sramptr,              
                  uint8_t* prg_rombank0ptr,          
                  uint8_t  rom_num)          
{   
    exp_rom     = exp_romptr;           /* expansion rom*/   
    sram        = sramptr;              /* sram*/   
    prg_rombank0= prg_rombank0ptr;      /* prg-rom lower bank*/   
    prg_rombank1= prg_rombank0ptr + (0x4000 * (rom_num - 1));       /* prg-rom upper bank*/   
   
    cpu_clockticks = 0;   
}