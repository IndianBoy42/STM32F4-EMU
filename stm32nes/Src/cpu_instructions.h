#include "cpu_zpinstr.h"
/* Instructions */   
static inline void cpu_adc(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + saveflags;   
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

static inline void cpu_and(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_asl(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putmemory(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_asla(CpuAdrMode adrmode)   
{   
	P= (P & 0xfe) | ((A >>7) & 0x01);   
	A = A << 1;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_bcc(CpuAdrMode adrmode)   
{   
	if ((P & 0x01)==0)   
	{   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_bcs(CpuAdrMode adrmode)   
{   
	if (P & 0x01){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_beq(CpuAdrMode adrmode)   
{   
	if (P & 0x02){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_bit(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   

/* non-destrucive logically And between value and the accumulator  
* and set zero flag */   
	if (value & A) P &= 0xfd; else P |= 0x02;   

/* set negative and overflow flags from value */   
	P = (P & 0x3f) | (value & 0xc0);   
}   

static inline void cpu_bmi(CpuAdrMode adrmode)   
{   
	if (P & 0x80){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_bne(CpuAdrMode adrmode)   
{   
	if ((P & 0x02)==0){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_bpl(CpuAdrMode adrmode)   
{   
	if ((P & 0x80)==0)  {   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_brk(CpuAdrMode adrmode)   
{   
	PC++; 

	// cpu_putmemory(0x0100+S--,(uint8_t)(PC>>8));   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
	// cpu_putmemory(0x0100+S--,P);  
	cpu_push16stack(PC);
	cpu_pushstack(P);

	P |= 0x14;     

	// PC = cpu_getmemory16(0xfffe);
	PC = IRQ_VECTOR;
	// PC=cpu_getmemory(0xfffe);   
	// PC |= cpu_getmemory(0xffff) << 8;    
}   

static inline void cpu_bvc(CpuAdrMode adrmode)   
{   
	if ((P & 0x40)==0){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_bvs(CpuAdrMode adrmode)   
{   
	if (P & 0x40){   
		uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
		PC += savepc;   
		cpu_clockticks++;   
	}else{   
		// uint8_t value = cpu_getmemory(PC);   
		PC++;   
	}   
}   

static inline void cpu_clc(CpuAdrMode adrmode)   
{   
	P &= 0xfe;   
}   

static inline void cpu_cld(CpuAdrMode adrmode)   
{   
	P &= 0xf7;   
}   

static inline void cpu_cli(CpuAdrMode adrmode)   
{   
	P &= 0xfb;   
}   

static inline void cpu_clv(CpuAdrMode adrmode)   
{   
	P &= 0xbf;   
}   

static inline void cpu_cmp(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_cpx(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   
	if (X+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=X+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_cpy(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   
	if (Y+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=Y+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_dec(CpuAdrMode adrmode)   
{   
	uint8_t temp;    

	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	temp = cpu_getmemory(savepc);   
	temp--;   
	cpu_putmemory(savepc, temp);   

	uint8_t value = cpu_getmemory(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_dex(CpuAdrMode adrmode)   
{   
	X--;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_dey(CpuAdrMode adrmode)   
{   
	Y--;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_eor(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	A ^= cpu_getmemory(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_inc(CpuAdrMode adrmode)   
{   
	uint8_t temp;     

	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	temp = cpu_getmemory(savepc);   
	temp++;   
	cpu_putmemory(savepc,temp);   

	uint8_t value = cpu_getmemory(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_inx(CpuAdrMode adrmode)   
{   
	X++;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_iny(CpuAdrMode adrmode)   
{   
	Y++;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_jmp(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	PC=savepc;   
}   

static inline void cpu_jsr(CpuAdrMode adrmode)   
{   
	PC++;   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC >> 8));   \
	cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
	cpu_push16stack(PC);
	PC--;   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	PC=savepc;   
}   

static inline void cpu_lda(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	A = cpu_getmemory(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ldx(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	X = cpu_getmemory(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ldy(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	Y = cpu_getmemory(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_lsr(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   

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

static inline void cpu_lsra(CpuAdrMode adrmode)   
{   
	P= (P & 0xfe) | (A & 0x01);   
	A = A >>1;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_nop(CpuAdrMode adrmode)   
{   
}   

static inline void cpu_ora(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	A |= cpu_getmemory(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_pha(CpuAdrMode adrmode)   
{   
	cpu_pushstack(A);   
}   

static inline void cpu_php(CpuAdrMode adrmode)   
{   
	cpu_pushstack(P);   
}   

static inline void cpu_pla(CpuAdrMode adrmode)   
{   
	A = cpu_popstack();   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_plp(CpuAdrMode adrmode)   
{   
	P = cpu_popstack() | 0x20;   
}   

static inline void cpu_rol(CpuAdrMode adrmode)   
{   
	int saveflags=(P & 0x01);   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	value |= saveflags;   
	cpu_putmemory(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_rola(CpuAdrMode adrmode)   
{   
	int saveflags=(P & 0x01);   
	P= (P & 0xfe) | ((A >>7) & 0x01);   
	A = A << 1;   
	A |= saveflags;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ror(CpuAdrMode adrmode)   
{   
	int saveflags=(P & 0x01);   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc);   

	P= (P & 0xfe) | (value & 0x01);   
	value = value >>1;   
	if (saveflags) value |= 0x80;   
	cpu_putmemory(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_rora(CpuAdrMode adrmode)   
{   
	int saveflags=(P & 0x01);   
	P= (P & 0xfe) | (A & 0x01);   
	A = A >>1;   
	if (saveflags) A |= 0x80;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_rti(CpuAdrMode adrmode)   
{   
	P   = cpu_popstack();   
	P  |= 0x20;   
	PC = cpu_pop16stack();
	// PC  = cpu_popstack();   
	// PC |= (cpu_popstack() << 8);   
}   

static inline void cpu_rts(CpuAdrMode adrmode)   
{   
//      PC++;   
	// PC  = cpu_popstack();   
	// PC |= (cpu_popstack() << 8); 
	PC = cpu_pop16stack();  
	PC++;   
}   

static inline void cpu_sbc(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	uint8_t value = cpu_getmemory(savepc) ^ 0xFF;   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + (saveflags << 4);   
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

static inline void cpu_sec(CpuAdrMode adrmode)   
{   
	P |= 0x01;   
}   

static inline void cpu_sed(CpuAdrMode adrmode)   
{   
	P |= 0x08;   
}   

static inline void cpu_sei(CpuAdrMode adrmode)   
{   
	P |= 0x04;   
}   

static inline void cpu_sta(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	cpu_putmemory(savepc,A);   
}   

static inline void cpu_stx(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	cpu_putmemory(savepc,X);   
}   

static inline void cpu_sty(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	cpu_putmemory(savepc,Y);   
}   

static inline void cpu_tax(CpuAdrMode adrmode)   
{   
	X=A;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_tay(CpuAdrMode adrmode)   
{   
	Y=A;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_tsx(CpuAdrMode adrmode)   
{   
	X=S;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_txa(CpuAdrMode adrmode)   
{   
	A=X;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_txs(CpuAdrMode adrmode)   
{   
	S=X;   
}   

static inline void cpu_tya(CpuAdrMode adrmode)   
{   
	A=Y;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_bra(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	PC += savepc;   
	cpu_clockticks++;   
}   

static inline void cpu_dea(CpuAdrMode adrmode)   
{   
	A--;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ina(CpuAdrMode adrmode)   
{   
	A++;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_phx(CpuAdrMode adrmode)   
{   
	cpu_pushstack(X);   
}   

static inline void cpu_plx(CpuAdrMode adrmode)   
{   
	X = cpu_popstack();   

	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_phy(CpuAdrMode adrmode)   
{   
	cpu_pushstack(Y);   
}   

static inline void cpu_ply(CpuAdrMode adrmode)   
{   
	Y = cpu_popstack();   

	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_stz(CpuAdrMode adrmode)   
{   
	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	cpu_putmemory(savepc,0);   
}   

static inline void cpu_tsb(CpuAdrMode adrmode)   
{   
	uint8_t temp;    

	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	temp  = cpu_getmemory(savepc);   
	temp |= A;   
	cpu_putmemory(savepc, temp);   

	if(cpu_getmemory(savepc))P &= 0xfd; else P |= 0x02;   
}   

static inline void cpu_trb(CpuAdrMode adrmode)   
{   
	uint8_t temp;       

	uint16_t savepc = /*opcodetable[opcode].*/adrmode();   
	temp  = cpu_getmemory(savepc);   
	temp &= (A ^ 0xFF);   
	cpu_putmemory(savepc, temp);   

	if(cpu_getmemory(savepc))P &= 0xfd; else P |= 0x02;   
}

//Zero page indexing instructions
static inline void cpu_tsbzp(CpuAdrMode x) {
	uint8_t temp;    

	uint16_t savepc = cpu_zp();  
	temp  = cpu_getzeropage(savepc);   
	temp |= A;   
	cpu_putzeropage(savepc, temp);   

	if(cpu_getzeropage(savepc))P &= 0xfd; else P |= 0x02; 

}
static inline void cpu_orazp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	A |= cpu_getzeropage(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_aslzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_trbzp(CpuAdrMode x) {
	uint8_t temp;       

	uint16_t savepc = cpu_zp();   
	temp  = cpu_getzeropage(savepc);   
	temp &= (A ^ 0xFF);   
	cpu_putzeropage(savepc, temp);   

	if(cpu_getzeropage(savepc))P &= 0xfd; else P |= 0x02;

}
static inline void cpu_orazpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	A |= cpu_getzeropage(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   

}
static inline void cpu_aslzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_bitzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   

/* non-destrucive logically And between value and the accumulator  
* and set zero flag */   
	if (value & A) P &= 0xfd; else P |= 0x02;   

/* set negative and overflow flags from value */   
	P = (P & 0x3f) | (value & 0xc0);   
}
static inline void cpu_andzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_rolzp(CpuAdrMode x) {
	int saveflags=(P & 0x01);   
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	value |= saveflags;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_bitzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   

/* non-destrucive logically And between value and the accumulator  
* and set zero flag */   
	if (value & A) P &= 0xfd; else P |= 0x02;   

/* set negative and overflow flags from value */   
	P = (P & 0x3f) | (value & 0xc0);   
}
static inline void cpu_andzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   

}
static inline void cpu_rolzpx(CpuAdrMode x) {
	int saveflags=(P & 0x01);   
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	value |= saveflags;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_eorzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	A ^= cpu_getzeropage(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_lsrzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   

/* set carry flag if shifting right causes a bit to be lost */   
	P= (P & 0xfe) | (value & 0x01);   

	value = value >>1;   
	cpu_putzeropage(savepc,value);   

/* set zero flag if value is zero */   
	if (value != 0) P &= 0xfd; else P |= 0x02;   

/* set negative flag if bit 8 set??? can this happen on an LSR? */   
	if ((value & 0x80) == 0x80)   
		P |= 0x80;   
	else   
		P &= 0x7f; 
}
static inline void cpu_eorzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	A ^= cpu_getzeropage(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_lsrzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   

/* set carry flag if shifting right causes a bit to be lost */   
	P= (P & 0xfe) | (value & 0x01);   

	value = value >>1;   
	cpu_putzeropage(savepc,value);   

/* set zero flag if value is zero */   
	if (value != 0) P &= 0xfd; else P |= 0x02;   

/* set negative flag if bit 8 set??? can this happen on an LSR? */   
	if ((value & 0x80) == 0x80)   
		P |= 0x80;   
	else   
		P &= 0x7f; 
}
static inline void cpu_stzzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();    
	cpu_putzeropage(savepc,0); 
}
static inline void cpu_adczp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();    
	uint8_t value = cpu_getzeropage(savepc);   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + saveflags;   
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
static inline void cpu_rorzp(CpuAdrMode x) {
	int saveflags=(P & 0x01);   
	uint16_t savepc = cpu_zp();    
	uint8_t value = cpu_getzeropage(savepc);   

	P= (P & 0xfe) | (value & 0x01);   
	value = value >>1;   
	if (saveflags) value |= 0x80;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_stzzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	cpu_putzeropage(savepc,0); 
}
static inline void cpu_adczpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + saveflags;   
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
static inline void cpu_rorzpx(CpuAdrMode x) {
	int saveflags=(P & 0x01);   
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   

	P= (P & 0xfe) | (value & 0x01);   
	value = value >>1;   
	if (saveflags) value |= 0x80;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_styzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();   
	cpu_putzeropage(savepc,Y);  
}
static inline void cpu_stazp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();   
	cpu_putzeropage(savepc,A);
}
static inline void cpu_stxzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();   
	cpu_putzeropage(savepc,X); 
}
static inline void cpu_styzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	cpu_putzeropage(savepc,Y);  
}
static inline void cpu_stazpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	cpu_putzeropage(savepc,A);
}
static inline void cpu_stxzpy(CpuAdrMode x) {
	uint16_t savepc = cpu_zpy();
	cpu_putzeropage(savepc,X); 
}
static inline void cpu_ldyzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	Y = cpu_getzeropage(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_ldazp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	A = cpu_getzeropage(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldxzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	X = cpu_getzeropage(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldyzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	Y = cpu_getzeropage(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_ldazpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	A = cpu_getzeropage(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldxzpy(CpuAdrMode x) {
	uint16_t savepc = cpu_zpy();
	X = cpu_getzeropage(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_cpyzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   
	if (Y+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=Y+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_cmpzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_deczp(CpuAdrMode x) {
	uint8_t temp;    

	uint16_t savepc = cpu_zp();  
	temp = cpu_getzeropage(savepc);   
	temp--;   
	cpu_putzeropage(savepc, temp);   

	uint8_t value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_cmpzpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_deczpx(CpuAdrMode x) {
	uint8_t temp;    

	uint16_t savepc = cpu_zpx();
	temp = cpu_getzeropage(savepc);   
	temp--;   
	cpu_putzeropage(savepc, temp);   

	uint8_t value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_cpxzp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc);   
	if (X+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=X+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_sbczp(CpuAdrMode x) {
	uint16_t savepc = cpu_zp();  
	uint8_t value = cpu_getzeropage(savepc) ^ 0xFF;   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + (saveflags << 4);   
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
static inline void cpu_inczp(CpuAdrMode x) {
	uint8_t temp;     

	uint16_t savepc = cpu_zp();  
	temp = cpu_getzeropage(savepc);   
	temp++;   
	cpu_putzeropage(savepc,temp);   

	uint8_t value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_sbczpx(CpuAdrMode x) {
	uint16_t savepc = cpu_zpx();
	uint8_t value = cpu_getzeropage(savepc) ^ 0xFF;   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + (saveflags << 4);   
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
static inline void cpu_inczpx(CpuAdrMode x) {
	uint8_t temp;     

	uint16_t savepc = cpu_zpx();
	temp = cpu_getzeropage(savepc);   
	temp++;   
	cpu_putzeropage(savepc,temp);   

	uint8_t value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}

static inline void cpu_oraindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();     
	A |= cpu_getmemory(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_andindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();    
	uint8_t value = cpu_getmemory(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_eorindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();    
	A ^= cpu_getmemory(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_adcindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();      
	uint8_t value = cpu_getmemory(savepc);   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + saveflags;   
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

static inline void cpu_staindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();     
	cpu_putmemory(savepc,A);   
}

static inline void cpu_ldaindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();    
	A = cpu_getmemory(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_cmpindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();    
	uint8_t value = cpu_getmemory(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}

static inline void cpu_sbcindzp(CpuAdrMode x) {
	uint16_t savepc = cpu_indzp();    
	uint8_t value = cpu_getmemory(savepc) ^ 0xFF;   

	int saveflags=(P & 0x01);   
	int sum= ((char) A) + ((char) value) + (saveflags << 4);   
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
