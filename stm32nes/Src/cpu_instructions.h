#include "cpu_zpinstr.h"
/* Instructions */   
static inline void cpu_adc(void)   
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

static inline void cpu_and(void)   
{   
	opcodetable[opcode].adrmode();   
//      value = gameImage[savepc];   
	value = cpu_getmemory(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_asl(void)   
{   
	opcodetable[opcode].adrmode();   
	value = cpu_getmemory(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putmemory(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_asla(void)   
{   
	P= (P & 0xfe) | ((A >>7) & 0x01);   
	A = A << 1;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_bcc(void)   
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

static inline void cpu_bcs(void)   
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

static inline void cpu_beq(void)   
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

static inline void cpu_bit(void)   
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

static inline void cpu_bmi(void)   
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

static inline void cpu_bne(void)   
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

static inline void cpu_bpl(void)   
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

static inline void cpu_brk(void)   
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

static inline void cpu_bvc(void)   
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

static inline void cpu_bvs(void)   
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

static inline void cpu_clc(void)   
{   
	P &= 0xfe;   
}   

static inline void cpu_cld(void)   
{   
	P &= 0xf7;   
}   

static inline void cpu_cli(void)   
{   
	P &= 0xfb;   
}   

static inline void cpu_clv(void)   
{   
	P &= 0xbf;   
}   

static inline void cpu_cmp(void)   
{   
	opcodetable[opcode].adrmode();   
	value = cpu_getmemory(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_cpx(void)   
{   
	opcodetable[opcode].adrmode();   
	value = cpu_getmemory(savepc);   
	if (X+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=X+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_cpy(void)   
{   
	opcodetable[opcode].adrmode();   
	value = cpu_getmemory(savepc);   
	if (Y+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=Y+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_dec(void)   
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

static inline void cpu_dex(void)   
{   
	X--;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_dey(void)   
{   
	Y--;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_eor(void)   
{   
	opcodetable[opcode].adrmode();   
//  A ^= gameImage[savepc];   
	A ^= cpu_getmemory(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_inc(void)   
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

static inline void cpu_inx(void)   
{   
	X++;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_iny(void)   
{   
	Y++;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_jmp(void)   
{   
	opcodetable[opcode].adrmode();   
	PC=savepc;   
}   

static inline void cpu_jsr(void)   
{   
	PC++;   
	// cpu_putmemory(0x0100+S--,(uint8_t)(PC >> 8));   \
	cpu_putmemory(0x0100+S--,(uint8_t)(PC & 0xff));   
	cpu_push16stack(PC);
	PC--;   
	opcodetable[opcode].adrmode();   
	PC=savepc;   
}   

static inline void cpu_lda(void)   
{   
	opcodetable[opcode].adrmode();   
//  A=gameImage[savepc];   
	A = cpu_getmemory(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ldx(void)   
{   
	opcodetable[opcode].adrmode();   
//  X=gameImage[savepc];   
	X = cpu_getmemory(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ldy(void)   
{   
	opcodetable[opcode].adrmode();   
//      Y=gameImage[savepc];   
	Y = cpu_getmemory(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_lsr(void)   
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

static inline void cpu_lsra(void)   
{   
	P= (P & 0xfe) | (A & 0x01);   
	A = A >>1;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_nop(void)   
{   
}   

static inline void cpu_ora(void)   
{   
	opcodetable[opcode].adrmode();   
//      A |= gameImage[savepc];   
	A |= cpu_getmemory(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_pha(void)   
{   
//      gameImage[0x100+S--] = A;   
	cpu_pushstack(A);   
}   

static inline void cpu_php(void)   
{   
//      gameImage[0x100+S--] = P;   
	cpu_pushstack(P);   
}   

static inline void cpu_pla(void)   
{   
//      A=gameImage[++S+0x100];   
	A = cpu_popstack();   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_plp(void)   
{   
//      P=gameImage[++S+0x100] | 0x20;   
	P = cpu_popstack() | 0x20;   
}   

static inline void cpu_rol(void)   
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

static inline void cpu_rola(void)   
{   
	saveflags=(P & 0x01);   
	P= (P & 0xfe) | ((A >>7) & 0x01);   
	A = A << 1;   
	A |= saveflags;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ror(void)   
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

static inline void cpu_rora(void)   
{   
	saveflags=(P & 0x01);   
	P= (P & 0xfe) | (A & 0x01);   
	A = A >>1;   
	if (saveflags) A |= 0x80;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_rti(void)   
{   
//      P=gameImage[++S+0x100] | 0x20;   
//      PC=gameImage[++S+0x100];   
//      PC |= (gameImage[++S+0x100] << 8);   
	P   = cpu_popstack();   
	P  |= 0x20;   
	PC = cpu_pop16stack();
	// PC  = cpu_popstack();   
	// PC |= (cpu_popstack() << 8);   
}   

static inline void cpu_rts(void)   
{   
//      PC=gameImage[++S+0x100];   
//      PC |= (gameImage[++S+0x100] << 8);   
//      PC++;   
	// PC  = cpu_popstack();   
	// PC |= (cpu_popstack() << 8); 
	PC = cpu_pop16stack();  
	PC++;   
}   

static inline void cpu_sbc(void)   
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

static inline void cpu_sec(void)   
{   
	P |= 0x01;   
}   

static inline void cpu_sed(void)   
{   
	P |= 0x08;   
}   

static inline void cpu_sei(void)   
{   
	P |= 0x04;   
}   

static inline void cpu_sta(void)   
{   
	opcodetable[opcode].adrmode();   
	cpu_putmemory(savepc,A);   
}   

static inline void cpu_stx(void)   
{   
	opcodetable[opcode].adrmode();   
	cpu_putmemory(savepc,X);   
}   

static inline void cpu_sty(void)   
{   
	opcodetable[opcode].adrmode();   
	cpu_putmemory(savepc,Y);   
}   

static inline void cpu_tax(void)   
{   
	X=A;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_tay(void)   
{   
	Y=A;   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_tsx(void)   
{   
	X=S;   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_txa(void)   
{   
	A=X;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_txs(void)   
{   
	S=X;   
}   

static inline void cpu_tya(void)   
{   
	A=Y;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_bra(void)   
{   
	opcodetable[opcode].adrmode();   
	PC += savepc;   
	cpu_clockticks++;   
}   

static inline void cpu_dea(void)   
{   
	A--;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_ina(void)   
{   
	A++;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_phx(void)   
{   
	cpu_pushstack(X);   
}   

static inline void cpu_plx(void)   
{   
//  X=gameImage[++S+0x100];   
	X = cpu_popstack();   

	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_phy(void)   
{   
	cpu_pushstack(Y);   
}   

static inline void cpu_ply(void)   
{   
//  Y=gameImage[++S+0x100];   
	Y = cpu_popstack();   

	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;   
}   

static inline void cpu_stz(void)   
{   
	opcodetable[opcode].adrmode();   
	cpu_putmemory(savepc,0);   
}   

static inline void cpu_tsb(void)   
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

static inline void cpu_trb(void)   
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
