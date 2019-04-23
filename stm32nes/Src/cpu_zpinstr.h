//Zero page indexing instructions
static inline void cpu_tsbzp(void) {
	uint8_t temp;    

	cpu_zp();  
//      gameImage[savepc] |= A;   
//      if (gameImage[savepc]) P &= 0xfd; else P |= 0x02;   
	temp  = cpu_getzeropage(savepc);   
	temp |= A;   
	cpu_putzeropage(savepc, temp);   

	if(cpu_getzeropage(savepc))P &= 0xfd; else P |= 0x02; 

}
static inline void cpu_orazp(void) {
	cpu_zp();  
//      A |= gameImage[savepc];   
	A |= cpu_getzeropage(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_aslzp(void) {
	cpu_zp();  
	value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_trbzp(void) {
	uint8_t temp;       

	cpu_zp();   
//      gameImage[savepc] = gameImage[savepc] & (A ^ 0xff);   
//      if (gameImage[savepc]) P &= 0xfd; else P |= 0x02;   
	temp  = cpu_getzeropage(savepc);   
	temp &= (A ^ 0xFF);   
	cpu_putzeropage(savepc, temp);   

	if(cpu_getzeropage(savepc))P &= 0xfd; else P |= 0x02;

}
static inline void cpu_orazpx(void) {
	cpu_zpx();
//      A |= gameImage[savepc];   
	A |= cpu_getzeropage(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   

}
static inline void cpu_aslzpx(void) {
	cpu_zpx();
	value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_bitzp(void) {
	cpu_zp();  
//  value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

/* non-destrucive logically And between value and the accumulator  
* and set zero flag */   
	if (value & A) P &= 0xfd; else P |= 0x02;   

/* set negative and overflow flags from value */   
	P = (P & 0x3f) | (value & 0xc0);   
}
static inline void cpu_andzp(void) {
	cpu_zp();  
//      value = gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_rolzp(void) {
	saveflags=(P & 0x01);   
	cpu_zp();  
	value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	value |= saveflags;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_bitzpx(void) {
	cpu_zpx();
//  value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

/* non-destrucive logically And between value and the accumulator  
* and set zero flag */   
	if (value & A) P &= 0xfd; else P |= 0x02;   

/* set negative and overflow flags from value */   
	P = (P & 0x3f) | (value & 0xc0);   
}
static inline void cpu_andzpx(void) {
	cpu_zpx();
//      value = gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f;   

}
static inline void cpu_rolzpx(void) {
	saveflags=(P & 0x01);   
	cpu_zpx();
	value = cpu_getzeropage(savepc);   
	P= (P & 0xfe) | ((value >>7) & 0x01);   
	value = value << 1;   
	value |= saveflags;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_eorzp(void) {
	cpu_zp();  
//  A ^= gameImage[savepc];   
	A ^= cpu_getzeropage(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_lsrzp(void) {
	cpu_zp();  
//      value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

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
static inline void cpu_eorzpx(void) {
	cpu_zpx();
//  A ^= gameImage[savepc];   
	A ^= cpu_getzeropage(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_lsrzpx(void) {
	cpu_zpx();
//      value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

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
static inline void cpu_stzzp(void) {
	cpu_zp();    
	cpu_putzeropage(savepc,0); 
}
static inline void cpu_adczp(void) {
	cpu_zp();    
//      value = gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

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
static inline void cpu_rorzp(void) {
	saveflags=(P & 0x01);   
	cpu_zp();    
//      value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

	P= (P & 0xfe) | (value & 0x01);   
	value = value >>1;   
	if (saveflags) value |= 0x80;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_stzzpx(void) {
	cpu_zpx();
	cpu_putzeropage(savepc,0); 
}
static inline void cpu_adczpx(void) {
	cpu_zpx();
//      value = gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

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
static inline void cpu_rorzpx(void) {
	saveflags=(P & 0x01);   
	cpu_zpx();
//      value=gameImage[savepc];   
	value = cpu_getzeropage(savepc);   

	P= (P & 0xfe) | (value & 0x01);   
	value = value >>1;   
	if (saveflags) value |= 0x80;   
	cpu_putzeropage(savepc,value);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_styzp(void) {
	cpu_zp();   
	cpu_putzeropage(savepc,Y);  
}
static inline void cpu_stazp(void) {
	cpu_zp();   
	cpu_putzeropage(savepc,A);
}
static inline void cpu_stxzp(void) {
	cpu_zp();   
	cpu_putzeropage(savepc,X); 
}
static inline void cpu_styzpx(void) {
	cpu_zpx();
	cpu_putzeropage(savepc,Y);  
}
static inline void cpu_stazpx(void) {
	cpu_zpx();
	cpu_putzeropage(savepc,A);
}
static inline void cpu_stxzpy(void) {
	cpu_zpy();
	cpu_putzeropage(savepc,X); 
}
static inline void cpu_ldyzp(void) {
	cpu_zp();  
//      Y=gameImage[savepc];   
	Y = cpu_getzeropage(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_ldazp(void) {
	cpu_zp();  
//  A=gameImage[savepc];   
	A = cpu_getzeropage(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldxzp(void) {
	cpu_zp();  
//  X=gameImage[savepc];   
	X = cpu_getzeropage(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldyzpx(void) {
	cpu_zpx();
//      Y=gameImage[savepc];   
	Y = cpu_getzeropage(savepc);   
	if (Y) P &= 0xfd; else P |= 0x02;   
	if (Y & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_ldazpx(void) {
	cpu_zpx();
//  A=gameImage[savepc];   
	A = cpu_getzeropage(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_ldxzpy(void) {
	cpu_zpy();
//  X=gameImage[savepc];   
	X = cpu_getzeropage(savepc);   
	if (X) P &= 0xfd; else P |= 0x02;   
	if (X & 0x80) P |= 0x80; else P &= 0x7f; 
}
static inline void cpu_cpyzp(void) {
	cpu_zp();  
	value = cpu_getzeropage(savepc);   
	if (Y+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=Y+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_cmpzp(void) {
	cpu_zp();  
	value = cpu_getzeropage(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_deczp(void) {
	uint8_t temp;    

	cpu_zp();  
//  gameImage[savepc]--;   
	temp = cpu_getzeropage(savepc);   
	temp--;   
	cpu_putzeropage(savepc, temp);   

	value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_cmpzpx(void) {
	cpu_zpx();
	value = cpu_getzeropage(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_deczpx(void) {
	uint8_t temp;    

	cpu_zpx();
//  gameImage[savepc]--;   
	temp = cpu_getzeropage(savepc);   
	temp--;   
	cpu_putzeropage(savepc, temp);   

	value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;  
}
static inline void cpu_cpxzp(void) {
	cpu_zp();  
	value = cpu_getzeropage(savepc);   
	if (X+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=X+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f; 

}
static inline void cpu_sbczp(void) {
	cpu_zp();  
//      value = gameImage[savepc] ^ 0xff;   
	value = cpu_getzeropage(savepc) ^ 0xFF;   

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
static inline void cpu_inczp(void) {
	uint8_t temp;     

	cpu_zp();  
//      gameImage[savepc]++;   
	temp = cpu_getzeropage(savepc);   
	temp++;   
	cpu_putzeropage(savepc,temp);   

	value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}
static inline void cpu_sbczpx(void) {
	cpu_zpx();
//      value = gameImage[savepc] ^ 0xff;   
	value = cpu_getzeropage(savepc) ^ 0xFF;   

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
static inline void cpu_inczpx(void) {
	uint8_t temp;     

	cpu_zpx();
//      gameImage[savepc]++;   
	temp = cpu_getzeropage(savepc);   
	temp++;   
	cpu_putzeropage(savepc,temp);   

	value = cpu_getzeropage(savepc);   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}

static inline void cpu_oraindzp(void) {
	cpu_indzp();     
//      A |= gameImage[savepc];   
	A |= cpu_getmemory(savepc);    

	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_andindzp(void) {
	cpu_indzp();    
//      value = gameImage[savepc];   
	value = cpu_getmemory(savepc);   

	A &= value;   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_eorindzp(void) {
	cpu_indzp();    
//  A ^= gameImage[savepc];   
	A ^= cpu_getmemory(savepc);   
	if (A) P &= 0xfd; else P |= 0x02;   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_adcindzp(void) {
	cpu_indzp();      
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

static inline void cpu_staindzp(void) {
	cpu_indzp();     
	cpu_putmemory(savepc,A);   
}

static inline void cpu_ldaindzp(void) {
	cpu_indzp();    
//  A=gameImage[savepc];   
	A = cpu_getmemory(savepc);   
// set the zero flag   
	if (A) P &= 0xfd; else P |= 0x02;   
// set the negative flag   
	if (A & 0x80) P |= 0x80; else P &= 0x7f; 
}

static inline void cpu_cmpindzp(void) {
	cpu_indzp();    
	value = cpu_getmemory(savepc);   
	if (A+0x100-value>0xff) P |= 0x01; else P &= 0xfe;   
	value=A+0x100-value;   
	if (value) P &= 0xfd; else P |= 0x02;   
	if (value & 0x80) P |= 0x80; else P &= 0x7f;
}

static inline void cpu_sbcindzp(void) {
	cpu_indzp();    
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
