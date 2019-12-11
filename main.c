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
#define OVERFLOW 0xFF // Doporucena hodnota preteceni casovace

/* A delay function */
void delay(long long bound) {

  long long i;
  for(i=0;i<bound;i++);
}

void beep(void) {
    for (unsigned int q=0; q<500; q++) {
        PTA->PDOR = GPIO_PDOR_PDO(0x0010);
        delay(500);
        PTA->PDOR = GPIO_PDOR_PDO(0x0000);
        delay(500);
    }
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/* Povoleni hodin do modulu, ktere budou pouzivany */
void SysInit(void) {
	// Defaultne je FTM clocked by internal vus clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // povoleni hodin do portu B (pro RGB LED)
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK; // povoleni hodin do casovace FTM0
}

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK;

    PORTA->PCR[4] = PORT_PCR_MUX(0x01);  // Speaker
    PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Speaker as output
}

void TimerInit(void) {
	// Vynulovaù register ËÌtaËa
	FTM0_CNT = 0x0;

	// Nastaviù hodnotu preteËenia do modulo registru TPMO
	//FTM0_MOD = OVERFLOW;
	//FTM0_MOD = 0xFF;
	FTM0_MOD = 0x6EE;

	// Nastaviù reûim generovania PWM na zvolenom kan·le (n) ËasovaËa
	// v riadiacom registri FTMx_CnSC
	// Edge-aligned PWM: High-true pulses (clear Output on match, set Output on reload),
    // preruöenie ani DMA requests nebud˙ vyuûÌvanÈ.
	FTM0_C1SC = 0x28;


	// Nastavenie st¯Ìdy (duty cycle)
	FTM0_C1V = 0x377;

	// Nastaviù konfigur·ciu ËasovaËa v jeho stavovem a ridicim registru (SC):
	// (up counting mode pre Edge-aligned PWM, Clock Mode Selection (01),
	// Prescale Factor Selection (Divide by 128), bez vyuûitia preruöenia Ëi DMA.
	FTM0_SC = 0xF;
}

int main(void) {
	int increment = 1; // PrÌznak zvyöovania (1) Ëi zniûovania (0) hodnoty compare
	unsigned int compare = 0; // Hodnota pre komparaËn˝ register (urËuj˙ci st¯Ìdu PWM)

	int turned_on = 1;

    MCUInit();
    SysInit();
    PortsInit();
    TimerInit();

    //beep();

    while (1) {
    //for (int i = 0; i < 10000; i++) {
    	//if (increment)  {
    		 // Zvyöuj st¯Ìdu (compare), pokiaæ nie je dosiahnutÈ zvolen·
    		 // maxim·lna hodnota (postupne zvysovani jasu LED).
    		 // Po negaci priznaku increment bude strida snizovana.
    	 //    compare++;
    	 //    increment = !(compare >= 0x377);
    	 //}
    	 //else  {
    		 // Snizuj stridu (compare), dokud neni dosazeno nulove hodnoty
    	     // (postupne snizovani jasu LED), nasledne bude strida opet zvysovana.
    	     //compare--;
    	     //increment = (compare == 0x00);
    	 //}
    	if (compare == 0) {
            PTA->PDOR = GPIO_PDOR_PDO(0x0010);
    	}
    	else if (compare == 0x377) {
            PTA->PDOR = GPIO_PDOR_PDO(0x0000);
    	}

    	compare++;

    	if (compare >= 0x6EE) {
    		compare = 0;
    	}


    	 // 5. Priradte aktualni hodnotu compare do komparacniho registru zvoleneho kanalu
    	 //    casovace TPM0 (napr. kanal c. 2 pro manipulaci s cervenou slozkou RGB LED).
    	 //TPM0_C0V = 127;
    	 //TPM0_C0V = compare;
    	 //TPM0_C1V = OVERFLOW - compare;

    	 // 6. LEDku nechte urcity cas svitit dle aktualni hodnoty stridy. Ve skutecnosti
    	 //    LED velmi rychle blika, pricemz vhodnou frekvenci signalu PWM (danou hodnotou
    	 //    modulo registru casovace) zajistime, ze blikani neni pro lidske oko patrne
    	 //    a LEDka se tak jevi, ze sviti intenzitou odpovidajici aktualni stride PWM.
    	 //    ZDE VYUZIJTE PRIPRAVENOU FUNKCI delay, EXPERIMENTALNE NASTAVTE HODNOTU
    	 //    CEKANI TAK, ABY BYLY PLYNULE ZMENY JASU LED DOBRE PATRNE.
    	 //delay(10000);
    }

    while (1);
}
