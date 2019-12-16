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
#define TIME_SCALE 2500
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

void PortsInit(void)
{
	/* Turn on all port clocks */
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK);
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK; // povoleni hodin do casovace FTM0
	SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;

	PORTE->PCR[8] = (0 | PORT_PCR_MUX(0x03));
	PORTE->PCR[9] = (0 | PORT_PCR_MUX(0x03));

	PORTA->PCR[4] = PORT_PCR_MUX(0x03);  // Speaker
	PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Speaker as output
}

void UART5Init() {
	UART5->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	UART5->BDH = 0x00;
	UART5->BDL = 0x1A; // Baud rate 115 200 Bd, 1 stop bit
	UART5->C4 = 0x0F; // Oversampling ratio 16, match address mode disabled
	UART5->C1 = 0x00; // 8 data bitu, bez parity
	UART5->C3 = 0x00;
	UART5->MA1 = 0x00; // no match address (mode disabled in C4)
	UART5->MA2 = 0x00; // no match address (mode disabled in C4)
	UART5->S2 |= 0xC0; // ?
	UART5->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK); // ?
}

void SendCh(char ch) {
	while (!(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK));
	UART5->D = ch;
}

char ReceiveCh(void) {
	while (!(UART5->S1 & UART_S1_RDRF_MASK));
	return UART5->D;
}

void SendStr(char *s) {
	int i = 0;
	while (s[i] != 0) {
		SendCh(s[i++]);
	}
}

int getMOD(int freq, int prescale) {
	int tmp = BUS_CLOCK_FREQ / prescale;
	return (tmp - freq) / freq;
}

int getCxV(int MOD) {
	return (MOD + 1) / 2;
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
	int count_end = ((tone * duratiom) / 1000) * TIME_SCALE;

	TimerInit(tone);

	while (count < count_end) {
		if (getTOF() == 1) { // TOF - Timer Overflow Flag
			count++;
		}
	}

	TImerTurnOff();
	delay(100000);
}

unsigned char prompt[14] = "\rUART_Tx_Rdy\n";

void displayHeader(void) {
	SendStr("\r- Simple electronic piano -\n");
	SendStr("\r===========================\n");
}

void displayOptionsMain(void) {
	SendStr("\rChoose action:\n");
	SendStr("\r1. Play prepared song\n");
	SendStr("\r2. Play your own song\n");
	SendStr("\r3. Quit\n");
	SendStr("\rYour answer (1/2/3): ");
}

void playSong1(void) {
	playNote(E4, 500);
	playNote(G4, 500);
	playNote(A4, 1000);

	delay(500 * TIME_SCALE);

	playNote(E4, 500);
	playNote(G4, 500);
	playNote(B4, 300);
	playNote(A4, 1000);

	delay(500 * TIME_SCALE);

	playNote(E4, 500);
	playNote(G4, 500);
	playNote(A4, 1000);
	playNote(G4, 500);
	playNote(E4, 500);
}

void actionPlayPreparedSong(void) {

}

void actionPlayOwnSong(void) {

}

int determineAction(void) {
	displayHeader();
	displayOptionsMain();

	char answer = ReceiveCh();
	SendCh(answer);

	while (1) {
		if (answer == '1') {
			SendStr("\n\rYou chose to play prepared song.\n");

			actionPlayPreparedSong();
		}
		else if (answer == '2') {
			SendStr("\n\rYou chose to play your own song.\n");

			actionPlayOwnSong();
		}
		else if (answer == '3') {
			// "Quit"
			SendStr("\n\rQuiting...\n\r");
			break;
		}
		else {
			// Repeat asking
			SendStr("\n\rWrong answer, choose again (1/2/3): ");
			answer = ReceiveCh();
			SendCh(answer);
			continue;
		}

		displayOptionsMain();
		answer = ReceiveCh();
		SendCh(answer);
	}

	return 0;
}

int main(void) {
	int count = 0;

	MCUInit();
	PortsInit();
	UART5Init();

	//determineAction();

	playSong1();

	while (1) {
		//playNote(C5, 2000);
		//playNote(A4, 500);
	}
}
