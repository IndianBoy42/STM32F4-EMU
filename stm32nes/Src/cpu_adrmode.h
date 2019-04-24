/* Adressing modes */   
/* Implied */   
static inline uint16_t cpu_implied(void)   
{   
	return 0;
}   

/* #Immediate */   
static inline uint16_t cpu_immediate(void)   
{   
	uint16_t savepc;
	savepc=PC++;   
	return savepc;
}   

/* ABS */   
static inline uint16_t cpu_abs(void)   
{    
	uint16_t savepc;
  	savepc = cpu_getoperand16();

  	PC++;   
  	PC++;   

 	return savepc;
}   

/* Branch */   
static inline uint16_t cpu_relative(void)   
{     
	uint16_t savepc;
  	savepc  = cpu_getoperand();  

  	if (savepc & 0x80) savepc -= 0x100;   
  	if ((savepc>>8) != (PC>>8))   
  		cpu_clockticks++;   

 	return savepc;
}   

/* (ABS) */   
static inline uint16_t cpu_indirect(void)   
{     
	uint16_t savepc;
  	uint16_t help = cpu_getoperand16();
  	savepc = cpu_getmemory16(help);

  	PC++;   
  	PC++;   

 	return savepc;
}

/* ABS,X */   
static inline uint16_t cpu_absx(void)   
{   
	uint16_t savepc;
  	savepc = cpu_getoperand16(); 

  	PC++;   
  	PC++;   
  	if (opcodeticks[opcode]==4)   
  		if ((savepc>>8) != ((savepc+X)>>8))   
  			cpu_clockticks++;   
	savepc += X;   
	return savepc;
}   

/* ABS,Y */   
static inline uint16_t cpu_absy(void)   
{    
	uint16_t savepc;
	savepc =  cpu_getoperand16();

	PC++;   
	PC++;   

	if (opcodeticks[opcode]==4)   
		if ((savepc>>8) != ((savepc+Y)>>8))   
			cpu_clockticks++;   
	savepc += Y;   
	return savepc;
}   

/* ZP */   
static inline uint16_t cpu_zp(void)   
{   
	uint16_t savepc;
	savepc  = cpu_getoperand(); 
	return savepc;
}   

/* ZP,X */   
static inline uint16_t cpu_zpx(void)   
{   
	uint16_t savepc;
	savepc  = cpu_getoperand() + X;  

	savepc &= 0x00ff;   
	return savepc;
}   

/* ZP,Y */   
static inline uint16_t cpu_zpy(void)   
{    
	uint16_t savepc;
	savepc  = cpu_getoperand() + Y;

	savepc &= 0x00ff;   
	return savepc;
}   

/* (ZP,X) */   
static inline uint16_t cpu_indx(void)   
{      
	uint16_t savepc;
	uint8_t value = cpu_getoperand() + Y; 
	// savepc = cpu_getmemory16(value);    
	savepc = cpu_getzeropage16(value);
	return savepc;
}   

/* (ZP),Y */   
static inline uint16_t cpu_indy(void)   
{     
	uint16_t savepc;
	uint8_t value = cpu_getoperand();  
	savepc = cpu_getmemory16(value);    
	// savepc = cpu_getzeropage16(value);   

	if (opcodeticks[opcode]==5)   
	if ((savepc>>8) != ((savepc+Y)>>8))   
		cpu_clockticks++;   
	savepc += Y;   
	return savepc;
}   

/* (ABS,X) */   
static inline uint16_t cpu_indabsx(void)   
{     
	uint16_t savepc;
	uint16_t help = cpu_getoperand16();
	savepc = cpu_getmemory16(help);
	return savepc;
}   

/* (ZP) */   
static inline uint16_t cpu_indzp(void)   
{     
	uint16_t savepc;
	uint8_t value = cpu_getoperand();    
	savepc = cpu_getzeropage16(value);       
	return savepc;
}   
