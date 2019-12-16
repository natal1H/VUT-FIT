//       An example for demonstrating basic principles of FITkit3 usage.
//
// It includes GPIO - inputs from button press/release, outputs for LED control,
// timer in output compare mode for generating periodic events (via interrupt
// service routine) and speaker handling (via alternating log. 0/1 through
// GPIO output on a reasonable frequency). Using this as a basis for IMP projects
// as well as for testing basic FITkit3 operation is strongly recommended.
//
//            (c) 2019 Michal Bidlo, BUT FIT, bidlom@fit.vutbr.cz
////////////////////////////////////////////////////////////////////////////
/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
#include <stdio.h>

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))
#define SPK 0x10          // Speaker is on PTA4

#define BUS_CLOCK_FREQ 50000000 // 50MHz

enum Tones {
	C4 = 261,
	D4 = 293,
	E4 = 329,
	F4 = 349,
	G4 = 392,
	A4 = 440,
	B4 = 493,
	C5 = 523
};

/* A delay function */
void delay(long long bound) {

	long long i;
	for (i = 0; i < bound; i++);
}

void beep(void) {
	for (unsigned int q = 0; q < 500; q++) {
		PTA->PDOR = GPIO_PDOR_PDO(0x0010);
		delay(500);
		PTA->PDOR = GPIO_PDOR_PDO(0x0000);
		delay(500);
	}
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void) {
	MCG_C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/* Povoleni hodin do modulu, ktere budou pouzivany */
void SysInit(void) {
	// Defaultne je FTM clocked by internal bus clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // povoleni hodin do portu B (pro RGB LED)
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK; // povoleni hodin do casovace FTM0
}

void PortsInit(void)
{
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK;

	PORTA->PCR[4] = PORT_PCR_MUX(0x03);  // Speaker
	PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Speaker as output
}

int getMOD(int freq, int prescale) {
	int tmp = BUS_CLOCK_FREQ / prescale;
	return (tmp - freq) / freq;
}

int getCxV(int MOD) {
	return (MOD + 1) / 2;
}

void FTM0_IRQHandler(void) {

}

void TimerInit(int freq) {
	int prescale = 8;

	// Vynulova register citaca
	FTM0_CNT = 0x0;

	// Nastavi hodnotu preteèenia do modulo registru FTMO
	FTM0_MOD = getMOD(freq, prescale);

	// Nastavi režim generovania PWM na zvolenom kanale (n) casovaca
	// v riadiacom registri FTMx_CnSC
	// Edge-aligned PWM: High-true pulses (clear Output on match, set Output on reload),
	// prerusenie ani DMA requests nebudu vyuzivane.
	FTM0_C1SC = 0x28;


	// Nastavenie stridy (duty cycle)
	FTM0_C1V = getCxV(FTM0_MOD);

	// Nastavi konfiguraciu casovaca v jeho stavovem a riadiacom registri (SC):
	// (up counting mode pre Edge-aligned PWM, Clock Mode Selection (01),
	// Prescale Factor Selection (Divide by 8), bez vyuzitia prerusenia ci DMA.
	FTM0_SC = 0b1011;
}

void TImerTurnOff(void) {
	// Vynulova register citaca
	FTM0_CNT = 0x0;

	// Disable FTM counter
	FTM0_SC = 0x0;
}

int getTOF() {
	return (FTM0_SC & 0x80) ? 1 : 0;
}

/**
 * tone freq in Hz
 * duration in ms
 */
void playNote(int tone, int duratiom) {
	int count = 0;
	int count_end = ((tone * duratiom) / 1000) * 2500;

	TimerInit(tone);

	while (count < count_end) {
		if (getTOF() == 1) { // TOF - Timer Overflow Flag
			count++;
		}
	}

	TImerTurnOff();
}

int main(void) {
	int count = 0;

	MCUInit();
	SysInit();
	PortsInit();

	while (1) {
		playNote(C5, 2000);
		delay(100000);
		playNote(A4, 2000);
	}
}
