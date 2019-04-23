#include "cpu.h" 
#include "nes.h"
#include "joypad.h"
#include "ppu.h" 
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

#define NMI_VECTOR (*(uint16_t*)&prg_rombank1[0x3FFA]) 
#define IRQ_VECTOR (*(uint16_t*)&prg_rombank1[0x3FFE])
#define RST_VECTOR (*(uint16_t*)&prg_rombank1[0x3FFC])

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
#include "cpu_optable.h"

/* help variables */   
uint16_t savepc;   
uint8_t value;   
int  sum, saveflags;   
uint16_t help;   

static __forceinline void cpu_pushstack(uint8_t value) {
    cpu_ram[0x0100+S--] = value;
}
static __forceinline void cpu_push16stack(uint16_t value) {
    S -= 2;
    *(uint16_t*)&cpu_ram[0x0101+S] = value;
}
static __forceinline uint8_t cpu_popstack(void) {
    return cpu_ram[++S + 0x100];
}
static __forceinline uint16_t cpu_pop16stack(void) {
    uint16_t tmp = *(uint16_t*)&cpu_ram[S + 0x101];
    S += 2;
    return tmp;
}
static __forceinline uint8_t cpu_getzeropage(uint8_t addr) {
	return cpu_ram[addr];
}
static __forceinline void cpu_putzeropage(uint8_t addr, uint8_t value) {
	cpu_ram[addr] = value;
}

#include "cpu_adrmode.h"
#include "cpu_instructions.h"   

/* Reset CPU */   
void cpu_reset(void)   
{   
	A=X=Y=0;   
	P = R_FLAG | Z_FLAG;       //Z_FLAG | R_FLAG   
	S=0xff;   

	PC = RST_VECTOR; 

	NMI_Flag = CLR0;   
	IRQ_Flag = CLR0;   
}   

/* Non maskerable interrupt */   
void cpu_nmi(void)   
{   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
	// cpu_putmemory(0x0100+S--,P);   
	cpu_push16stack(PC);
	cpu_pushstack(P);

	P |= 0x04; 

	// PC = cpu_getmemory16(0xfffa);
	PC = NMI_VECTOR;
	// PC=cpu_getmemory(0xfffa);   
	// PC |= cpu_getmemory(0xfffb) << 8;   
}   

/* Maskerable Interrupt */   
void cpu_irq(void)   
{   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
	// cpu_putmemory(0x0100+S--,P);  
	cpu_push16stack(PC);
	cpu_pushstack(P);

	P |= 0x04;     

	// PC = cpu_getmemory16(0xfffe);
	PC = IRQ_VECTOR;
	// PC=cpu_getmemory(0xfffe);   
	// PC |= cpu_getmemory(0xffff) << 8;   
}

/* Execute Instruction */   
void cpu_exec(int timerTicks)   
{   
	if(NMI_Flag == SET1){   
		cpu_nmi();   
		gpio_toggle(LED2);
		NMI_Flag = CLR0;   
	} 
	/*else if(IRQ_Flag == SET1 && !(P & I_FLAG)){   
		cpu_irq();   
		IRQ_Flag = CLR0;   
	}*/   

	while (timerTicks > cpu_clockticks)    
	{   
		opcode = cpu_getmemory(PC);   
		PC++;   
		// opcodetable[opcode].instruction();   
		// cpu_clockticks += opcodetable[opcode].ticks;   
		exec_instr(opcode);
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

	// uint8_t* ptr = sram;
	// tft_printh(0,line+2, *ptr++, 8);
	// tft_printh(2,line+2, *ptr++, 8);
	// tft_printh(4,line+2, *ptr++, 8);
	// tft_printh(6,line+2, *ptr++, 8);
	// tft_printc(0,line+3, (char*)sram);
}
