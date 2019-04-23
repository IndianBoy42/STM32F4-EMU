#ifndef _PPU_H_ 
#define _PPU_H_ 
 
/* Includes ------------------------------------------------------------------*/  
#include "stm32f4xx.h"
 #undef NULL   
#define NULL 0 
 
#ifndef FALSE 
	#define FALSE 0 
#endif 
 
#ifndef	TRUE 
	#define TRUE !FALSE 
#endif 
 
typedef struct{ 
	uint8_t spr_ram[256];			
	uint8_t spr_addrcnt;			
}Spr_MemType; 
 
typedef struct{ 
 	uint8_t	y;		
	uint8_t	t_num;	
	uint8_t	attr;	
	uint8_t	x;		
}SpriteType; 
 
typedef struct{     
	uint16_t PPU_addrcnt;	
	uint8_t  PPU_readtemp; 
	const uint8_t *patterntable0;			
	const uint8_t *patterntable1; 		
	uint8_t *name_table[4]; 
	uint8_t image_palette[16];		
	uint8_t sprite_palette[16];		
}PPU_MemType; 
 
typedef struct{ 
	uint8_t R0;	 //$2000 
	uint8_t R1;	 //$2001 
	uint8_t R2;	 //$2002 
//	uint8_t R3;	 //$2003 
//	uint8_t R4;	 //$2004 
	uint8_t R5;	 //$2005 
//	uint8_t R6;	 //$2006 
//	uint8_t R7;	 //$2007 
}PPU_RegType; 
 
#define SPR_VFLIP		0x80 
#define SPR_HFLIP		0x40 
#define SPR_BG_PRIO		0x20	//(0=Sprite In front of BG, 1=Sprite Behind BG) 
 
/*R0*/ 
#define R0_VB_NMI_EN	0x80 
#define R0_SPR_SIZE		0x20	//(0=8x8, 1=8x16) 
#define BG_PATTERN_ADDR	0x10 
#define SPR_PATTERN_ADDR 0x08 
#define PPU_ADDRINCR	0x04  	
#define R0_NAME_TABLE	0x03 
 
/*R1*/ 
#define R1_SPR_VISIBLE	0x10 
#define R1_BG_VISIBLE	0x08 
#define R1_SPR_LEFT8	0x04 
#define R1_BG_LEFT8		0x02 
#define R1_DISPMODE		0x01 
 
/*R2*/ 
#define R2_VBlank_Flag	0x80 
#define R2_SPR0_HIT		0x40 
#define R2_LOST_SPR		0x20 
 
 
 
#define NES_DISP_WIDTH					256	  
#define CLOCKS_PER_SCANLINE				113   
 
#define SCAN_LINE_START_NUM				0 
#define SCAN_LINE_DISPLAY_START_NUM		21 
#define	SCAN_LINE_DISPLAY_END_NUM		261 
#define	SCAN_LINE_END_NUM				262 
 
#define NAME_TABLE_H_MASK	1 
#define NAME_TABLE_V_MASK	2 
 
extern int PPU_scanline; 
extern uint8_t SpriteHitFlag, PPU_Latch_Flag; 
 
extern Spr_MemType Spr_Mem; 
extern PPU_RegType PPU_Reg; 
extern PPU_MemType PPU_Mem; 
extern uint16_t DisplayBuffer[256*240];
 
extern SpriteType  * const sprite; 
void ppu_init(const uint8_t* vromaddr, uint8_t  ScreenMirrorType); 
void ppu_spr0_hit_flag(int y_axes);
void ppu_render_line(int y_axes); 
 
#endif 	/* _PPU_H_*/ 
