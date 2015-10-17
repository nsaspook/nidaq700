#include "blinker.h"

static volatile union Obits2 LEDS;

uint8_t blink_led(uint8_t led, uint8_t on_led, uint8_t start) // blink and store status of 8 leds
{
	if (led > 7) return LEDS.out_byte;

	if (on_led) {
		V.blink_out &= ~(0x0001 << led); // reset the bit and store it on [8..15] of blink
	} else {
		V.blink_out |= (0x0001 << led); // set the bit and store it on [8..15] of blink
	}

	if (start) {
		V.blink |= 0x0001 << led; // set the blink bit for the LED
	} else {
		V.blink &= ~(0x0001 << led); // reset the blink bit for the LED
		LEDS.out_byte &= ~(0x01 << led); // reset the LEDS bit for the LED
		LEDS.out_byte |= (((V.blink_out >> led)&0x01) << led); // restore the LEDS bit status to the on_led state
	}
	return LEDS.out_byte;
}

uint8_t is_led_blinking(uint8_t led)
{
	return(V.blink >> led)&0b00000001;
}

uint8_t blink_led_alt(uint8_t alt) // set alt and returns the previous value
{
	uint8_t old_alt = V.blink_alt;
	V.blink_alt = alt;
	return old_alt;
}

uint8_t is_led_on(uint8_t led)
{
	return(V.blink_out >> led)&0b00000001;
}

void Blink_Init(void)
{
	LEDS.out_byte = 0xff;
	V.blink = 0;
	V.blink_out = 0;
	V.blink_alt = 0;
	// High priority interrupt for Blink timer. Called 1/2sec.
	ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_7 | T4_INT_SUB_PRIOR_3);
	OpenTimer4(T4_ON | T4_SOURCE_INT | T4_PS_1_8, (50000000 / 2 / 64 / 2)); // for 2hz
}

static void Drive_leds(void)
{

	if (LEDS.out_bits.b0) {
		mPORTAClearBits(BIT_0);
	} else {
		mPORTASetBits(BIT_0); // LED OFF
	}
	if (LEDS.out_bits.b1) {
		mPORTAClearBits(BIT_1);
	} else {
		mPORTASetBits(BIT_1); // LED OFF
	}
}

void __ISR(_TIMER_4_VECTOR, IPL7AUTO) TimerBlinkHandler(void)
{
	// check the LED blink flags
	if (V.blink_alt) {
		if (V.blink & 0b00000001) LEDS.out_bits.b0 = !LEDS.out_bits.b0;
		if (V.blink & 0b00000010) LEDS.out_bits.b1 = !LEDS.out_bits.b0;
		if (V.blink & 0b00000100) LEDS.out_bits.b2 = !LEDS.out_bits.b2;
		if (V.blink & 0b00001000) LEDS.out_bits.b3 = !LEDS.out_bits.b2;
		if (V.blink & 0b00010000) LEDS.out_bits.b4 = !LEDS.out_bits.b4;
		if (V.blink & 0b00100000) LEDS.out_bits.b5 = !LEDS.out_bits.b4;
		if (V.blink & 0b01000000) LEDS.out_bits.b6 = !LEDS.out_bits.b6;
		if (V.blink & 0b10000000) LEDS.out_bits.b7 = !LEDS.out_bits.b6;
	} else {
		if (V.blink & 0b00000001) LEDS.out_bits.b0 = !LEDS.out_bits.b0;
		if (V.blink & 0b00000010) LEDS.out_bits.b1 = !LEDS.out_bits.b1;
		if (V.blink & 0b00000100) LEDS.out_bits.b2 = !LEDS.out_bits.b2;
		if (V.blink & 0b00001000) LEDS.out_bits.b3 = !LEDS.out_bits.b3;
		if (V.blink & 0b00010000) LEDS.out_bits.b4 = !LEDS.out_bits.b4;
		if (V.blink & 0b00100000) LEDS.out_bits.b5 = !LEDS.out_bits.b5;
		if (V.blink & 0b01000000) LEDS.out_bits.b6 = !LEDS.out_bits.b6;
		if (V.blink & 0b10000000) LEDS.out_bits.b7 = !LEDS.out_bits.b7;
	}
	Drive_leds();
	V.blink_count++;
	INTClearFlag(INT_T4);
}