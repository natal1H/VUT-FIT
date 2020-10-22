/**
 * ARM-FITkit3: Jednoduchy elektronicky klavir
 *
 * Autor: Natalia Holkova
 * Login: xholko02
 * Datum: 22.12.2019
 *
 * 90% original
 * - delay(), MCUInit() z FITkit3 dema - autor: Michal Bidlo
 */

#include "MK60D10.h"
#include <stdio.h>

#define SPK 0x10          // Speaker is on PTA4
#define TIME_SCALE 2500
#define TIME_DELAY_SCALE 2500
#define BUS_CLOCK_FREQ 50000000 // 50MHz
#define PRESCALE 8
#define FIXED_DURATION 250

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

/**
 * Funkcia na delay
 *
 * @param bound Dlzka delay
 */
void delay(long long bound) {

	long long i;
	for (i = 0; i < bound; i++);
}

/**
 * Inicializacia MCU - zakladne nastavenie hodin, vypnutie watchdog
 */
void MCUInit(void) {
	MCG_C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/**
 * Inicializacia portov, povolenie hodin
 */
void PortsInit(void) {
	/* Zapnutie vsetkych hodin na porty */
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK);
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK; // povolenie hodin do casovaca FTM0
	SIM->SCGC1 |= SIM_SCGC1_UART5_MASK; // povolenie hodiny do UART5

	PORTE->PCR[8] = (0 | PORT_PCR_MUX(0x03));
	PORTE->PCR[9] = (0 | PORT_PCR_MUX(0x03));

	PORTA->PCR[4] = PORT_PCR_MUX(0x03);  // Bzuciak
	PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Bzuciak na vystup
}

/**
 * Inicializacia UART5
 */
void UART5Init() {
	UART5->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	UART5->BDH = 0x00;
	UART5->BDL = 0x1A; // Baud rate 115 200 Bd, 1 stop bit
	UART5->C4 = 0x0F; // Oversampling ratio 16, match address mode vypnute
	UART5->C1 = 0x00; // 8 data bitu, bez parity
	UART5->C3 = 0x00;
	UART5->MA1 = 0x00; // no match address (mode disabled in C4)
	UART5->MA2 = 0x00; // no match address (mode disabled in C4)
	UART5->S2 |= 0xC0;
	UART5->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

/**
 * Funkcia na poslanie znaku
 *
 * @param ch Znak na poslanie
 */
void SendCh(char ch) {
	while (!(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK));
	UART5->D = ch;
}

/**
 * Funkcia na prijatie znaku
 *
 * @return prijaty znak
 */
char ReceiveCh(void) {
	while (!(UART5->S1 & UART_S1_RDRF_MASK));
	return UART5->D;
}

/**
 * Funkcia na poslanie retazca
 *
 * @param s Znak na poslanie
 */
void SendStr(char *s) {
	int i = 0;
	while (s[i] != 0) {
		SendCh(s[i++]);
	}
}

/**
 * Funkcia na ziskanie hodnoty MOD pre FTM casovac
 *
 * @param Vstupna frekvencia do FTM casovaca
 * @return hodnota MOD
 */
int getMOD(int freq) {
	int tmp = BUS_CLOCK_FREQ / PRESCALE;
	return (tmp - freq) / freq;
}

/**
 * Funkcia na vypocet hodnoty pre FTMx_CnV (50% duty cycle)
 *
 * @param MOD hodnota v FTMx_CnV
 * @return hodnota pre FTMx_CnV
 */
int getCnV(int MOD) {
	return (MOD + 1) / 2;
}

/**
 * Inicializacia FTM casovaca
 *
 * @param freq Frekvencia tonu na vygenerovanie
 */
void TimerInit(int freq) {
	// Vynulova register citaca
	FTM0_CNT = 0x0;

	// Nastavi hodnotu pretecenia do modulo registru FTMO
	FTM0_MOD = getMOD(freq);

	// Nastavi rezim generovania PWM na zvolenom kanale (n) casovaca
	// v riadiacom registri FTMx_CnSC
	// Edge-aligned PWM: High-true pulses (clear Output on match, set Output on reload),
	// prerusenie ani DMA requests nebudu vyuzivane.
	FTM0_C1SC = 0x28;

	// Nastavenie stridy (duty cycle)
	FTM0_C1V = getCnV(FTM0_MOD);

	// Nastavi konfiguraciu casovaca v jeho stavovem a riadiacom registri (SC):
	// (up counting mode pre Edge-aligned PWM, Clock Mode Selection (01),
	// Prescale Factor Selection (Divide by 8), bez vyuzitia prerusenia ci DMA.
	FTM0_SC = 0xB;
}

/**
 * Vypnutie casovaca
 */
void TimerTurnOff(void) {
	// Vynulova register citaca
	FTM0_CNT = 0x0;

	// Disable FTM counter
	FTM0_SC = 0x0;
}

/**
 * Ziskanie TOF flagu z FTM0
 *
 * @return True ak nastaveny TOF
 */
int getTOF() {
	return (FTM0_SC & 0x80) ? 1 : 0;
}

/**
 * Zahranie tonu o danej frekvencii a trvani (nie je presne)
 *
 * @param tone frekvencia v Hz
 * @param duration dlzka trvania v ms (nie je presne)
 */
void playNote(int tone, int duratiom) {
	int count = 0;
	int count_end = ((tone * duratiom) / 1000) * TIME_SCALE;
	int half_time = count_end / 2;

	TimerInit(tone);

	while (count < count_end) {
		if (getTOF() == 1) { // TOF - Timer Overflow Flag
			count++;
		}

		if (count >= half_time) {
			// Doznievanie
			if (FTM0_C1V > 0)
				FTM0_C1V -= 1;
		}
	}

	TimerTurnOff();
	delay(100000);
}

/**
 * Zobrazenie header v terminale
 */
void displayHeader(void) {
	SendStr("\r- Simple electronic piano -\n");
	SendStr("\r===========================\n");
}

/**
 * Zobrazenie hlavneho menu v terminale
 */
void displayOptionsMain(void) {
	SendStr("\rChoose action:\n");
	SendStr("\r1. Play prepared song\n");
	SendStr("\r2. Play your own song\n");
	SendStr("\r3. Quit\n");
	SendStr("\rYour answer (1/2/3): ");
}

/**
 * Zahraj song 1
 */
void playSong1(void) {
	playNote(E4, 500);
	playNote(G4, 500);
	playNote(A4, 1000);

	delay(500 * TIME_DELAY_SCALE);

	playNote(E4, 500);
	playNote(G4, 500);
	playNote(B4, 300);
	playNote(A4, 1000);

	delay(500 * TIME_DELAY_SCALE);

	playNote(E4, 500);
	playNote(G4, 500);
	playNote(A4, 1000);
	playNote(G4, 500);
	playNote(E4, 1000);

	delay(1000 * TIME_DELAY_SCALE);

}

/**
 * Zahraj song 2
 */
void playSong2(void) {
	playNote(C4, 500);
	playNote(D4, 500);
	playNote(E4, 500);
	playNote(F4, 500);
	playNote(G4, 500);
	playNote(A4, 500);
	playNote(B4, 500);
	playNote(C5, 500);

	delay(1000 * TIME_DELAY_SCALE);

	playNote(C5, 500);
	playNote(B4, 500);
	playNote(A4, 500);
	playNote(G4, 500);
	playNote(F4, 500);
	playNote(E4, 500);
	playNote(D4, 500);
	playNote(C4, 500);

	delay(1000 * TIME_DELAY_SCALE);
}

/**
 * Zahraj song 3
 */
void playSong3(void) {
	playNote(E4, 500);
	playNote(E4, 500);
	playNote(F4, 500);
	playNote(G4, 500);
	playNote(G4, 500);
	playNote(F4, 500);
	playNote(E4, 500);
	playNote(D4, 500);
	playNote(C4, 500);
	playNote(C4, 500);
	playNote(D4, 500);
	playNote(E4, 500);
	playNote(E4, 1000);
	playNote(D4, 1000);

	delay(1000 * TIME_DELAY_SCALE);

	playNote(E4, 500);
	playNote(E4, 500);
	playNote(F4, 500);
	playNote(G4, 500);
	playNote(G4, 500);
	playNote(F4, 500);
	playNote(E4, 500);
	playNote(D4, 500);
	playNote(C4, 500);
	playNote(C4, 500);
	playNote(D4, 500);
	playNote(E4, 500);
	playNote(D4, 1000);
	playNote(C4, 1000);

	delay(1000 * TIME_DELAY_SCALE);
}

/**
 * Zobrazenie moznosti pre predpripravene songy
 */
void displaypreparedSongsOptions() {
	SendStr("\rChoose song:\n");
	SendStr("\r1. \"Smoke on the water\"\n");
	SendStr("\r2. Range test\n");
	SendStr("\r3. \"Ode to joy\"\n");
	SendStr("\r4. Back\n");
	SendStr("\rYour answer (1/2/3/4): ");
}

/**
 * Zahranie predpripravenej pesnicky
 */
void actionPlayPreparedSong(void) {
	displaypreparedSongsOptions();

	char answer = ReceiveCh();
	SendCh(answer);

	while (1) {
		if (answer == '1') {
			SendStr("\n\rYou chose to song 1.\n");
			playSong1();
			break;
		}
		else if (answer == '2') {
			SendStr("\n\rYou chose to song 2.\n");
			playSong2();
			break;
		}
		else if (answer == '3') {
			SendStr("\n\rYou chose to song 3.\n");
			playSong3();
			break;
		}
		else if (answer == '4') {
			// "Back"
			SendStr("\n\rBack...\n\r");
			break;

		}
		else {
			// Repeat asking
			SendStr("\n\rWrong answer, choose again (1/2/3): ");
			answer = ReceiveCh();
			SendCh(answer);
			continue;
		}

		displaypreparedSongsOptions();
		answer = ReceiveCh();
		SendCh(answer);
	}
}

/**
 * Zobraz moznosti prehravanie vlastnej pesnicky v terminale
 */
void displayOwnSongOptions(void) {
	SendStr("\rInput style options:\n");
	SendStr("\r1. Keyboard press (start/stop)\n");
	SendStr("\r2. Keyboard press (fixed duration)\n");
	SendStr("\r3. Back\n");
	SendStr("\rYour answer (1/2/3): ");
}

/**
 * Zahranie vlastnej pesnicky pomocou klaves 1-8
 * Ton hra, pokial sa nestlaci " "/ nie je stlaceny iny ton alebo vypnutie pomocu "q"
 */
void actionPlayInputKeyboardStartStop(void) {
	SendStr("\rKeyboard binding:\n");
	SendStr("\r  - \"1\" - C4\n");
	SendStr("\r  - \"2\" - D4\n");
	SendStr("\r  - \"3\" - E4\n");
	SendStr("\r  - \"4\" - F4\n");
	SendStr("\r  - \"5\" - G4\n");
	SendStr("\r  - \"6\" - A4\n");
	SendStr("\r  - \"7\" - B4\n");
	SendStr("\r  - \"8\" - C5\n");
	SendStr("\r  - \" \" - stop sound\n");
	SendStr("\r  - \"q\" - back\n");
	SendStr("\r================\n");

	char answer = ReceiveCh();

	while (1) {
		if (answer == '1') {
			TimerTurnOff();
			TimerInit(C4);
		}
		else if (answer == '2') {
			TimerTurnOff();
			TimerInit(D4);
		}
		else if (answer == '3') {
			TimerTurnOff();
			TimerInit(E4);
		}
		else if (answer == '4') {
			TimerTurnOff();
			TimerInit(F4);
		}
		else if (answer == '5') {
			TimerTurnOff();
			TimerInit(G4);
		}
		else if (answer == '6') {
			TimerTurnOff();
			TimerInit(A4);
		}
		else if (answer == '7') {
			TimerTurnOff();
			TimerInit(B4);
		}
		else if (answer == '8') {
			TimerTurnOff();
			TimerInit(C5);
		}
		else if (answer == ' ') {
			TimerTurnOff();
		}
		else if (answer == 'q') {
			// "Back"
			TimerTurnOff();
			SendStr("\n\rBack...\n\r");
			break;
		}
		answer = ReceiveCh();
	}
}

/**
 * Zahranie vlastnej pesnicky pomocou klaves 1-8, ton ma fixed dlzku
 */
void actionPlayInputKeyboardFixed(void) {
	SendStr("\rKeyboard binding:\n");
	SendStr("\r  - \"1\" - C4\n");
	SendStr("\r  - \"2\" - D4\n");
	SendStr("\r  - \"3\" - E4\n");
	SendStr("\r  - \"4\" - F4\n");
	SendStr("\r  - \"5\" - G4\n");
	SendStr("\r  - \"6\" - A4\n");
	SendStr("\r  - \"7\" - B4\n");
	SendStr("\r  - \"8\" - C5\n");
	SendStr("\r  - \"q\" - back\n");
	SendStr("\r================\n");

	char answer = ReceiveCh();
	while (1) {
		if (answer == '1')
			playNote(C4, FIXED_DURATION);
		else if (answer == '2')
			playNote(D4, FIXED_DURATION);
		else if (answer == '3')
			playNote(E4, FIXED_DURATION);
		else if (answer == '4')
			playNote(F4, FIXED_DURATION);
		else if (answer == '5')
			playNote(G4, FIXED_DURATION);
		else if (answer == '6')
			playNote(A4, FIXED_DURATION);
		else if (answer == '7')
			playNote(B4, FIXED_DURATION);
		else if (answer == '8')
			playNote(C5, FIXED_DURATION);
		else if (answer == 'q') {
			// "Back"
			SendStr("\n\rBack...\n\r");
			break;
		}

		answer = ReceiveCh();
	}

}

/**
 * Zahratie vlastnej pesnicky
 */
void actionPlayOwnSong(void) {
	displayOwnSongOptions();

	char answer = ReceiveCh();
	SendCh(answer);

	while (1) {
		if (answer == '1') {
			SendStr("\n\rYou chose keyboard input (start/stop).\n");
			actionPlayInputKeyboardStartStop();
		}
		else if (answer == '2') {
			SendStr("\n\rYou chose keyboard input (fixed duration).\n");
			actionPlayInputKeyboardFixed();
		}
		else if (answer == '3') {
			// "Quit"
			SendStr("\n\rBack...\n\r");
			break;
		}
		else {
			// Repeat asking
			SendStr("\n\rWrong answer, choose again (1/2/3): ");
			answer = ReceiveCh();
			SendCh(answer);
			continue;
		}

		displayOwnSongOptions();
		answer = ReceiveCh();
		SendCh(answer);
	}
}

/**
 * Volba co urobit
 */
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
	MCUInit();
	PortsInit();
	UART5Init();

	determineAction();

	while (1);
}
