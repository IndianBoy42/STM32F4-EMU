#include "cpu.h" 
#include "nes.h"
#include "joypad.h"
#include "ppu.h" 
#include "instr.h"
#include "mem.h"
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

CPU_InitFlag NMI_Flag;   
static CPU_InitFlag IRQ_Flag;   

static uint8_t A;
static uint8_t X;
static uint8_t Y;
static uint8_t P;
static uint8_t S;
static uint16_t PC;

/* internal registers */   
uint8_t opcode;   
int  cpu_clockticks;   

/* help variables */   
uint16_t savepc;   
uint8_t value;   
int  sum, saveflags;   
uint16_t help;   

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
	A=X=Y=0;   
	P = R_FLAG | Z_FLAG;       //Z_FLAG | R_FLAG   
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

/* Execute Instruction */   
void cpu_exec(int timerTicks)   
{   
	if(NMI_Flag == SET1){   
		cpu_nmi();   
		NMI_Flag = CLR0;   
	}else if(IRQ_Flag == SET1 && !(P & I_FLAG)){   
		cpu_irq();   
		IRQ_Flag = CLR0;   
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

void cpu_initmem(const uint8_t* prg_rombank0ptr, uint8_t rom_num)          
{
	prg_rombank0= prg_rombank0ptr;      /* prg-rom lower bank*/   
	prg_rombank1= prg_rombank0ptr + (0x4000 * (rom_num - 1));       /* prg-rom upper bank*/   

	cpu_clockticks = 0;   
}
#include "lcd_main.h"
void cpu_debug_print(uint8_t line) {
	tft_printc(0, line, "A  X  Y  P  S  PC   NQ *PC");
	tft_printc(30, line, (char*)opcode_names[opcode]);

	tft_printh(0,  line+1, A, 8);
	tft_printh(3,  line+1, X, 8);
	tft_printh(6,  line+1, Y, 8);
	tft_printh(9,  line+1, P, 8);
	tft_printh(12, line+1, S, 8);
	tft_printh(15, line+1, PC, 16);
	tft_printb(20, line+1, NMI_Flag, 1);
	tft_printb(21, line+1, IRQ_Flag, 1);
	
	tft_printh(23, line+1, cpu_getmemory(PC),   8);
	tft_printh(25, line+1, cpu_getmemory(PC+1), 8);
	tft_printh(27, line+1, cpu_getmemory(PC+2), 8);
	tft_printh(29, line+1, cpu_getmemory(PC+3), 8);

	uint8_t* ptr = sram;
	tft_printh(0,line+2, *ptr++, 8);
	tft_printh(2,line+2, *ptr++, 8);
	tft_printh(4,line+2, *ptr++, 8);
	tft_printh(6,line+2, *ptr++, 8);
	tft_printc(0,line+3, (char*)sram);
}
