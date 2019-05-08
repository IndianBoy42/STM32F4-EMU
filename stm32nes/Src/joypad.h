#include "gpio.h"
#include "adc.h"
#include "imu.h"
uint32_t volatile joypad_state = 0;
// static __forceinline 
uint32_t joypad_update() {
	uint32_t state = BUTTONS & 0xFFFF;
	state |= (joys.x0 > 3072)  << 16;
	state |= (joys.x0 < 1024)  << 17;
	state |= (joys.y0 > 3072)  << 18;
	state |= (joys.y0 < 1024)  << 19;
	state |= (joys.x1 > 3072)  << 20;
	state |= (joys.x1 < 1024)  << 21;
	state |= (joys.y1 > 3072)  << 22;
	state |= (joys.y1 < 1024)  << 23;
	state |= (getAccelLeft())  << 24;
	state |= (getAccelRight()) << 25;
	state |= (getAccelFlick()) << 26;
	joypad_state = state;
	return state;
}

uint32_t joypad_A = (BTN_R2|BTN_U2|BTN_JS2_U|BTN_JS2_D|BTN_IMU_J);
uint32_t joypad_B = (BTN_D2|BTN_L2|BTN_JS1|BTN_JS2|BTN_M1|BTN_M2|BTN_JS2_L|BTN_JS2_R);
uint32_t joypad_U = (BTN_U1|BTN_JS1_U);
uint32_t joypad_D = (BTN_D1|BTN_JS1_D);
uint32_t joypad_L = (BTN_L1|BTN_JS1_L|BTN_IMU_L);
uint32_t joypad_R = (BTN_R1|BTN_JS1_R|BTN_IMU_R);
uint32_t joypad_SEL = (BTN_X1);
uint32_t joypad_START = (BTN_X4);

typedef void (*JoypadBtnFunc)(void);
#define JOYPAD_BTN_TABLE(X) \
	X(JOYPAD_A,     joypad_A)         \
	X(JOYPAD_B,     joypad_B)         \
	X(JOYPAD_SEL,   joypad_SEL)       \
	X(JOYPAD_START, joypad_START)     \
	X(JOYPAD_U,     joypad_U)         \
	X(JOYPAD_D,     joypad_D)         \
	X(JOYPAD_L,     joypad_L)         \
	X(JOYPAD_R,     joypad_R)         \

static volatile uint8_t joypad_read_state[2] = {0};
static volatile uint8_t joypad_strobe[2] = {0};
#define JOYPAD_BTN_ENUM(A, B) A,
enum {
	JOYPAD_BTN_TABLE(JOYPAD_BTN_ENUM)
};
void joypad_write(uint8_t n, uint8_t v) {
	// if (joypad_strobe[n] && !v) {
	if (v) {
		joypad_read_state[n] = 0;
		joypad_state = joypad_update();
	}
	joypad_strobe[n] = v;
}

#define JOYPAD_BTN_READ(A, B) case A: return (joypad_state & B) != 0;
uint8_t joypad_read(uint8_t n) {
	if (n==1) return 0;
	// if (joypad_strobe) return joypad_A();
	switch (joypad_read_state[n]++) {
		JOYPAD_BTN_TABLE(JOYPAD_BTN_READ)
		default: return 0;
	}
}
