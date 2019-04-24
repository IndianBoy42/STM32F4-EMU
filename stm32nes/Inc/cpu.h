#ifndef _CPU_H 
#define _CPU_H 
#include "stm32f4xx.h"
//#include "PPU.h" 
 
typedef enum {CLR0 = 0, SET1 = !CLR0} CPU_InitFlag; 
 
extern  CPU_InitFlag NMI_Flag; 
extern  CPU_InitFlag NMI_Flag; 

// extern uint8_t* prg_rombank0;	/* prg-rom lower bank*/ 
// extern uint8_t* prg_rombank1;	/* prg-rom upper bank*/  

typedef uint16_t (*CpuAdrMode)(void);
uint16_t cpu_implied(void); 
uint16_t cpu_immediate(void); 
uint16_t cpu_abs(void); 
uint16_t cpu_relative(void); 
uint16_t cpu_indirect(void); 
uint16_t cpu_absx(void); 
uint16_t cpu_absy(void); 
uint16_t cpu_zp(void); 
uint16_t cpu_zpx(void); 
uint16_t cpu_zpy(void); 
uint16_t cpu_indx(void); 
uint16_t cpu_indy(void); 
uint16_t cpu_indabsx(void); 
uint16_t cpu_indzp(void); 

void cpu_adc(CpuAdrMode adrmode); 
void cpu_and(CpuAdrMode adrmode); 
void cpu_asl(CpuAdrMode adrmode); 
void cpu_asla(CpuAdrMode adrmode); 
void cpu_bcc(CpuAdrMode adrmode); 
void cpu_bcs(CpuAdrMode adrmode); 
void cpu_beq(CpuAdrMode adrmode); 
void cpu_bit(CpuAdrMode adrmode); 
void cpu_bmi(CpuAdrMode adrmode); 
void cpu_bne(CpuAdrMode adrmode); 
void cpu_bpl(CpuAdrMode adrmode); 
void cpu_brk(CpuAdrMode adrmode); 
void cpu_bvc(CpuAdrMode adrmode); 
void cpu_bvs(CpuAdrMode adrmode); 
void cpu_clc(CpuAdrMode adrmode); 
void cpu_cld(CpuAdrMode adrmode); 
void cpu_cli(CpuAdrMode adrmode); 
void cpu_clv(CpuAdrMode adrmode); 
void cpu_cmp(CpuAdrMode adrmode); 
void cpu_cpx(CpuAdrMode adrmode); 
void cpu_cpy(CpuAdrMode adrmode); 
void cpu_dec(CpuAdrMode adrmode); 
void cpu_dex(CpuAdrMode adrmode); 
void cpu_dey(CpuAdrMode adrmode); 
void cpu_eor(CpuAdrMode adrmode); 
void cpu_inc(CpuAdrMode adrmode); 
void cpu_inx(CpuAdrMode adrmode); 
void cpu_iny(CpuAdrMode adrmode); 
void cpu_jmp(CpuAdrMode adrmode); 
void cpu_jsr(CpuAdrMode adrmode); 
void cpu_lda(CpuAdrMode adrmode); 
void cpu_ldx(CpuAdrMode adrmode); 
void cpu_ldy(CpuAdrMode adrmode); 
void cpu_lsr(CpuAdrMode adrmode); 
void cpu_lsra(CpuAdrMode adrmode); 
void cpu_nop(CpuAdrMode adrmode); 
void cpu_ora(CpuAdrMode adrmode); 
void cpu_pha(CpuAdrMode adrmode); 
void cpu_php(CpuAdrMode adrmode); 
void cpu_pla(CpuAdrMode adrmode); 
void cpu_plp(CpuAdrMode adrmode); 
void cpu_rol(CpuAdrMode adrmode); 
void cpu_rola(CpuAdrMode adrmode); 
void cpu_ror(CpuAdrMode adrmode); 
void cpu_rora(CpuAdrMode adrmode); 
void cpu_rti(CpuAdrMode adrmode); 
void cpu_rts(CpuAdrMode adrmode); 
void cpu_sbc(CpuAdrMode adrmode); 
void cpu_sec(CpuAdrMode adrmode); 
void cpu_sed(CpuAdrMode adrmode); 
void cpu_sei(CpuAdrMode adrmode); 
void cpu_sta(CpuAdrMode adrmode); 
void cpu_stx(CpuAdrMode adrmode); 
void cpu_sty(CpuAdrMode adrmode); 
void cpu_tax(CpuAdrMode adrmode); 
void cpu_tay(CpuAdrMode adrmode); 
void cpu_tsx(CpuAdrMode adrmode); 
void cpu_txa(CpuAdrMode adrmode); 
void cpu_txs(CpuAdrMode adrmode); 
void cpu_tya(CpuAdrMode adrmode); 
void cpu_bra(CpuAdrMode adrmode); 
void cpu_dea(CpuAdrMode adrmode); 
void cpu_ina(CpuAdrMode adrmode); 
void cpu_phx(CpuAdrMode adrmode); 
void cpu_plx(CpuAdrMode adrmode); 
void cpu_phy(CpuAdrMode adrmode); 
void cpu_ply(CpuAdrMode adrmode); 
void cpu_stz(CpuAdrMode adrmode); 
void cpu_tsb(CpuAdrMode adrmode); 
void cpu_trb(CpuAdrMode adrmode); 
void cpu_tsbzp(CpuAdrMode adrmode);
void cpu_orazp(CpuAdrMode adrmode);
void cpu_aslzp(CpuAdrMode adrmode);
void cpu_trbzp(CpuAdrMode adrmode);
void cpu_orazpx(CpuAdrMode adrmode);
void cpu_aslzpx(CpuAdrMode adrmode);
void cpu_bitzp(CpuAdrMode adrmode);
void cpu_andzp(CpuAdrMode adrmode);
void cpu_rolzp(CpuAdrMode adrmode);
void cpu_bitzpx(CpuAdrMode adrmode);
void cpu_andzpx(CpuAdrMode adrmode);
void cpu_rolzpx(CpuAdrMode adrmode);
void cpu_eorzp(CpuAdrMode adrmode);
void cpu_lsrzp(CpuAdrMode adrmode);
void cpu_eorzpx(CpuAdrMode adrmode);
void cpu_lsrzpx(CpuAdrMode adrmode);
void cpu_stzzp(CpuAdrMode adrmode);
void cpu_adczp(CpuAdrMode adrmode);
void cpu_rorzp(CpuAdrMode adrmode);
void cpu_stzzpx(CpuAdrMode adrmode);
void cpu_adczpx(CpuAdrMode adrmode);
void cpu_rorzpx(CpuAdrMode adrmode);
void cpu_styzp(CpuAdrMode adrmode);
void cpu_stazp(CpuAdrMode adrmode);
void cpu_stxzp(CpuAdrMode adrmode);
void cpu_styzpx(CpuAdrMode adrmode);
void cpu_stazpx(CpuAdrMode adrmode);
void cpu_stxzpy(CpuAdrMode adrmode);
void cpu_ldyzp(CpuAdrMode adrmode);
void cpu_ldazp(CpuAdrMode adrmode);
void cpu_ldxzp(CpuAdrMode adrmode);
void cpu_ldyzpx(CpuAdrMode adrmode);
void cpu_ldazpx(CpuAdrMode adrmode);
void cpu_ldxzpy(CpuAdrMode adrmode);
void cpu_cpyzp(CpuAdrMode adrmode);
void cpu_cmpzp(CpuAdrMode adrmode);
void cpu_deczp(CpuAdrMode adrmode);
void cpu_cmpzpx(CpuAdrMode adrmode);
void cpu_deczpx(CpuAdrMode adrmode);
void cpu_cpxzp(CpuAdrMode adrmode);
void cpu_sbczp(CpuAdrMode adrmode);
void cpu_inczp(CpuAdrMode adrmode);
void cpu_sbczpx(CpuAdrMode adrmode);
void cpu_inczpx(CpuAdrMode adrmode);
void cpu_oraindzp(CpuAdrMode adrmode);
void cpu_andindzp(CpuAdrMode adrmode);
void cpu_eorindzp(CpuAdrMode adrmode);
void cpu_adcindzp(CpuAdrMode adrmode);
void cpu_staindzp(CpuAdrMode adrmode);
void cpu_ldaindzp(CpuAdrMode adrmode);
void cpu_cmpindzp(CpuAdrMode adrmode);
void cpu_sbcindzp(CpuAdrMode adrmode);
 
/* Non maskerable interrupt */ 
void cpu_nmi(void); 
 
/* Maskerable Interrupt */ 
void cpu_irq(void); 
 
/* must be called first to initialise cpu_all  engines arrays */ 
 
void cpu_initmem(const uint8_t* prg_rombank0ptr, uint8_t rom_num); 
 
/* sets all of cpu_the  registers. The program counter is set from 
 * locations $FFFC and $FFFD masked with the above addrmask 
 */ 
void cpu_reset(void); 
 
/* run cpu_the  engine for specified number of clock cycles */ 
void cpu_exec(int timerTicks); 
 
#endif	/* _CPU_H*/ 
