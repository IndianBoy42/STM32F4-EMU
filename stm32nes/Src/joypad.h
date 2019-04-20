#include "gpio.h"
#include "adc.h"
__forceinline uint8_t joypad_A() {
	return (BUTTONS & (BTN_R2|BTN_U2)) != 0;
}
__forceinline uint8_t joypad_B() {
	return (BUTTONS & (BTN_D2|BTN_L2)) != 0;

}
__forceinline uint8_t joypad_U() {
	return ((BUTTONS & (BTN_U1))) != 0;
}
__forceinline uint8_t joypad_D() {
	return ((BUTTONS & (BTN_D1))) != 0;
}
__forceinline uint8_t joypad_L() {
	return ((BUTTONS & (BTN_L1))) != 0;
}
__forceinline uint8_t joypad_R() {
	return ((BUTTONS & (BTN_R1))) != 0;
}
__forceinline uint8_t joypad_SEL() {
	return (BUTTONS & (BTN_X1)) != 0;
}
__forceinline uint8_t joypad_START() {
	return (BUTTONS & (BTN_X2)) != 0;
}

typedef void (*JoypadBtnFunc)(void);
#define JOYPAD_BTN_TABLE(X) \
	X(JOYPAD_A,     joypad_A())         \
	X(JOYPAD_B,     joypad_B())         \
	X(JOYPAD_SEL,   joypad_SEL())     \
	X(JOYPAD_START, joypad_START()) \
	X(JOYPAD_U,     joypad_U())         \
	X(JOYPAD_D,     joypad_D())         \
	X(JOYPAD_L,     joypad_L())         \
	X(JOYPAD_R,     joypad_R())         \

static uint8_t joypad_read_state[2] = {0};
static uint8_t joypad_strobe[2] = {0};
#define JOYPAD_BTN_ENUM(A, B) A,
enum {
	JOYPAD_BTN_TABLE(JOYPAD_BTN_ENUM)
};
void joypad_write(uint8_t n, uint8_t v) {
	// if (joypad_strobe && !v) {
	if (v) {
		joypad_read_state[n] = 0;
	}
	joypad_strobe[n] = v;
}

#define JOYPAD_BTN_READ(A, B) case A: return B;
uint8_t joypad_read(uint8_t n) {
	if (n==1) return 0;
	// if (joypad_strobe) return joypad_A();
	switch (joypad_read_state[n]++) {
		JOYPAD_BTN_TABLE(JOYPAD_BTN_READ)
		default: return 0;
	}
}
