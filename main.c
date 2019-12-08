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

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

#define SPK 0x10          // Speaker is on PTA4

enum tone_e {
	c1 = 261,
	c1S = 277,
	d1 = 294,
	d1S = 311,
	e1 = 329,
	f1 = 349,
	f1S = 370,
	g1 = 391,
	g1S = 415,
	a1 = 440,
	a1S = 455,
	b1 = 466,
	h1 = 493,
	c2 = 523,
	c2S = 554,
	d2 = 587,
	d2S = 622,
	e2 = 659,
	f2 = 698,
	f2S = 740,
	g2 = 784,
	g2S = 830,
	a2 = 880
} Tone;

int n=0;								// index
unsigned char c;						// prijaty znak
unsigned char prompt[7] = "Login>";		// text vyzvy
unsigned char corrl[9] = "xholko02";		// Vas login
unsigned char login[9];					// misto pro prijaty retezec
int pressed_up = 0, pressed_down = 0;
int beep_flag = 0;
unsigned int compare = 0x200;


/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void) {
    /* Turn on all port (A, B, E) and UART5 clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;
	SIM->SCGC1 = SIM_SCGC1_UART5_MASK;

    PORTE->PCR[8] = PORT_PCR_MUX(0x03); // UART0_TX
    PORTE->PCR[9] = PORT_PCR_MUX(0x03); // UART0_RX

    PORTA->PCR[4] = PORT_PCR_MUX(0x01);  // Speaker

    /* Change corresponding PTB port pins as outputs */
    PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Speaker as output
}

/* Initialize UART */
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

void SendCh(char ch)  {
	while(!(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK) );
	UART5->D = ch;
}

char ReceiveCh(void) {
	while(!(UART5->S1 & UART_S1_RDRF_MASK));
	return UART5->D;
}

void SendStr(char *s)  {
	int i = 0;
	while (s[i]!=0) {
		SendCh(s[i++]);
	}
}

void delay(long long bound) {
  long long i;
  for(i=0;i<bound;i++);
}

/* Pipnuti pres bzucak - generuje 500 period obdelnikoveho signalu */
void beep(void) {
    for (unsigned int q=0; q<500; q++) {
        PTA->PDOR = GPIO_PDOR_PDO(0x0010);
        delay(500);
        PTA->PDOR = GPIO_PDOR_PDO(0x0000);
        delay(500);
    }
}

/* freq - frequency, duration - duration in microseconds? */
void playNote(int note, int duration) {

	long dl = (long)(10000/note);
	long time = (long)((duration*100)/(dl*2));
	for (int i=0;i<time;i++)
	{
		GPIOA_PDOR ^= SPK; // Speaker switched on
		delay(dl*55);   //
	}
	delay(800);
}

void PlayNote(int freq, int duration) {
	long half_period = (long) (10000 / freq); // x10000 to get int
	long time = (long) ((duration * 100) / (half_period));
	for (int i=0; i < time; i++) {
//		GPIOA_PDOR ^= SPK; // Speaker switched on
        PTA->PDOR = GPIO_PDOR_PDO(0x0010);
		delay(half_period * 30);   //
		PTA->PDOR = GPIO_PDOR_PDO(0x0000);
		delay(half_period * 30);   //
	}
	delay(800);
}

void PlaySong1() //Imperial march
{
    PlayNote(a1, 500);
    PlayNote(a1, 500);
    PlayNote(a1, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 650);

    delay(3000);

    PlayNote(e2, 500);
    PlayNote(e2, 500);
    PlayNote(e2, 500);
    PlayNote(f2, 350);
    PlayNote(c2, 150);
    PlayNote(g1S, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 650);

    delay(3000);

    PlayNote(a1, 500);
    PlayNote(a1, 300);
    PlayNote(a1, 150);
    PlayNote(a2, 400);
    PlayNote(g2S, 200);
    PlayNote(g2, 200);
    PlayNote(f2S, 125);
    PlayNote(f2, 125);
    PlayNote(f2S, 250);

    delay(3000);
}

int main(void) {
    MCUInit();
    PortsInit();
    UART5Init();

//    while (1) {
//		SendStr(prompt); // Vyslani vyzvy k zadani loginu

//		for (n = 0; n < 8; n++) {
//			c = ReceiveCh();
//			SendCh(c); // Prijaty znak se hned vysle - echo linky
//			login[n] = c; // Postupne se uklada do pole
//		}

//		beep(); // potvrzeni pipnutim
//		SendStr("\r\n");
    	//beep();
//        Music(1);
//   	}
    PlaySong1();

    while(1);
}
