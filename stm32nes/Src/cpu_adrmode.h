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
  	// savepc  = cpu_getmemory(PC); \
  	savepc += (cpu_getmemory(PC+1) << 8);   
  	savepc = cpu_getmemory16(PC);

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
  	// help    =  cpu_getmemory(PC);   \
  	help   += (cpu_getmemory(PC+1) << 8);   
  	// savepc  =  cpu_getmemory(help);   \
  	savepc += (cpu_getmemory(help+1) << 8);   
  	help = cpu_getmemory16(PC);
  	savepc = cpu_getmemory16(help);

  	PC++;   
  	PC++;   
}

/* ABS,X */   
void cpu_absx(void)   
{   
//      savepc = gameImage[PC] + (gameImage[PC + 1] << 8);   
  	// savepc  = cpu_getmemory(PC);   \
  	savepc += (cpu_getmemory(PC+1) << 8);  
  	savepc = cpu_getmemory16(PC); 

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
	// savepc  = cpu_getmemory(PC);   \
	savepc += (cpu_getmemory(PC+1) << 8);  
	savepc =  cpu_getmemory16(PC);

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
	// savepc  = cpu_getmemory(value);     \
	savepc += cpu_getmemory(value + 1) << 8;
	savepc = cpu_getmemory16(value);    
}   

/* (ZP),Y */   
void cpu_indy(void)   
{   
	//  value = gameImage[PC++];   
	//  savepc = gameImage[value] + (gameImage[value+1] << 8);   
	value = cpu_getmemory(PC);   
	PC++;   
	// savepc  = cpu_getmemory(value);     \
	savepc += cpu_getmemory(value + 1) << 8; 
	savepc = cpu_getmemory16(value);       

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
	// help = cpu_getmemory(PC);   \
	help = (cpu_getmemory(PC+1) << 8) +X;   
	// savepc  = cpu_getmemory(help);  \
	savepc += cpu_getmemory(help + 1) << 8;   
	help = cpu_getmemory16(PC);
	savepc = cpu_getmemory16(help);
}   

/* (ZP) */   
void cpu_indzp(void)   
{   
//      value = gameImage[PC++];   
//      savepc = gameImage[value] + (gameImage[value + 1] << 8);   
	value = cpu_getmemory(PC);   
	PC++;   
	// savepc  = cpu_getmemory(value);     \
	savepc += cpu_getmemory(value + 1) << 8;  
	// savepc = cpu_getmemory16(value);      
	savepc = *(uint16_t*)&cpu_ram[value];      
}   
