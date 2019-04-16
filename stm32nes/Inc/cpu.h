#ifndef _CPU_H 
#define _CPU_H 
#include "stm32f4xx.h"
//#include "PPU.h" 
 
typedef enum {CLR0 = 0, SET1 = !CLR0} CPU_InitFlag; 
 
extern  CPU_InitFlag NMI_Flag; 
extern  CPU_InitFlag NMI_Flag; 

// extern uint8_t* prg_rombank0;	/* prg-rom lower bank*/ 
// extern uint8_t* prg_rombank1;	/* prg-rom upper bank*/  

void cpu_implied(void); 
void cpu_immediate(void); 
void cpu_abs(void); 
void cpu_relative(void); 
void cpu_indirect(void); 
void cpu_absx(void); 
void cpu_absy(void); 
void cpu_zp(void); 
void cpu_zpx(void); 
void cpu_zpy(void); 
void cpu_indx(void); 
void cpu_indy(void); 
void cpu_indabsx(void); 
void cpu_indzp(void); 
void cpu_adc(void); 
void cpu_and(void); 
void cpu_asl(void); 
void cpu_asla(void); 
void cpu_bcc(void); 
void cpu_bcs(void); 
void cpu_beq(void); 
void cpu_bit(void); 
void cpu_bmi(void); 
void cpu_bne(void); 
void cpu_bpl(void); 
void cpu_brk(void); 
void cpu_bvc(void); 
void cpu_bvs(void); 
void cpu_clc(void); 
void cpu_cld(void); 
void cpu_cli(void); 
void cpu_clv(void); 
void cpu_cmp(void); 
void cpu_cpx(void); 
void cpu_cpy(void); 
void cpu_dec(void); 
void cpu_dex(void); 
void cpu_dey(void); 
void cpu_eor(void); 
void cpu_inc(void); 
void cpu_inx(void); 
void cpu_iny(void); 
void cpu_jmp(void); 
void cpu_jsr(void); 
void cpu_lda(void); 
void cpu_ldx(void); 
void cpu_ldy(void); 
void cpu_lsr(void); 
void cpu_lsra(void); 
void cpu_nop(void); 
void cpu_ora(void); 
void cpu_pha(void); 
void cpu_php(void); 
void cpu_pla(void); 
void cpu_plp(void); 
void cpu_rol(void); 
void cpu_rola(void); 
void cpu_ror(void); 
void cpu_rora(void); 
void cpu_rti(void); 
void cpu_rts(void); 
void cpu_sbc(void); 
void cpu_sec(void); 
void cpu_sed(void); 
void cpu_sei(void); 
void cpu_sta(void); 
void cpu_stx(void); 
void cpu_sty(void); 
void cpu_tax(void); 
void cpu_tay(void); 
void cpu_tsx(void); 
void cpu_txa(void); 
void cpu_txs(void); 
void cpu_tya(void); 
void cpu_bra(void); 
void cpu_dea(void); 
void cpu_ina(void); 
void cpu_phx(void); 
void cpu_plx(void); 
void cpu_phy(void); 
void cpu_ply(void); 
void cpu_stz(void); 
void cpu_tsb(void); 
void cpu_trb(void); 
 
/* Non maskerable interrupt */ 
void cpu_nmi(void); 
 
/* Maskerable Interrupt */ 
void cpu_irq(void); 
 
/* must be called first to initialise cpu_all  engines arrays */ 
 
void cpu_initmem(	uint8_t* exp_romptr, 
					uint8_t* sramptr,			 
					uint8_t* prg_rombank0ptr,		 
					uint8_t  rom_num); 
 
/* sets all of cpu_the  registers. The program counter is set from 
 * locations $FFFC and $FFFD masked with the above addrmask 
 */ 
void cpu_reset(void); 
 
/* run cpu_the  engine for specified number of clock cycles */ 
void cpu_exec(int timerTicks); 
 
#endif	/* _CPU_H*/ 
