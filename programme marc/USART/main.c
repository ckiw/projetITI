#include <p18f4680.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>

#pragma config OSC = IRCIO67 // cf DS51537F-page 222 permet de prendre en compte l'OSC interne, et libère les broches RA6 et RA7
#pragma config MCLRE = ON // Broche 1 en mode reset
#pragma config WDT = OFF // watchDog OFF
#pragma PBADEN = OFF //ADCON1 est configuré pour avoir RB<0-4> en logique au démarrage

void InterruptServiceHigh(void);
void InterruptServiceLow(void);

double vitesse;
char buf[32];

// écrit la fonction InterruptVectorHigh dans la zone mémoire 0x08 définie par le constructeur 
//(en fait sur une interruption le microcontrolleur va voir à cette adresse)cf p113
#pragma code InterruptVectorHigh = 0x08 

void InterruptVectorHigh(void)
{
	_asm
	goto InterruptServiceHigh
	_endasm
} 
// écrit la fonction InterruptVectorLow dans la zone mémoire 0x18 définie par le constructeur 
//(en fait sur une interruption le microcontrolleur va voir à cette adresse) 
#pragma code InterruptVectorLow = 0x18

void InterruptVectorLow(void)
{
	_asm
	goto InterruptServiceLow
	_endasm
}


void main()
{
	char data;
	memset(buf, 0, 32);
	
	//Configure l’oscillateur interne sur 8MHz
	OSCCON=0xF6;
	//Active les priorités sur interruption
	RCONbits.IPEN = 1;
	//Active les interruptions de haute prio
	INTCONbits.GIE = 1;
	//Active les interruptions de basse prio
	INTCONbits.PEIE = 1;
	//Active l'interruption sur INT0/RB0
	INTCONbits.INT0IE = 1;
	// interruption sur front montant pout INT0
	INTCON2bits.INTEDG0 = 0;
	// Initialise le flag INT0/RB0
	INTCONbits.INT0IF = 0;

	// interruption sur front montant pour INT1
	INTCON2bits.INTEDG1 = 0;
	// Initialise le flag INT1/RB1
	INTCON3bits.INT1IF = 0;
	//Priorité interruption RB1
	INTCON3bits.INT1IP = 1;
	//Active l'interruption sur INT1/RB1
	INTCON3bits.INT1IE = 1;

	//Initialise le flag d'interruption de L'USART
	PIR1bits.RCIF = 0;
	//Active la réception du RS232
	RCSTAbits.CREN= 1;
	//Interruption sur reception RS232 en prio haute
	IPR1bits.RCIP = 1;
	//Active l'interruption sur reception RS232
	PIE1bits.RCIE = 1;
	//Active l'interruption sur les changement de RB4 à RB7 p117
	INTCONbits.RBIE = 1;
	//Initialise le flag interruption sur les changement de RB4 à RB7
	INTCONbits.RBIF = 0;
	//Haute priorité sur interruption sur les changement de RB4 à RB7
	INTCON2bits.RBIP = 1;

	//Interruption du timer0 en prio haute
	INTCON2bits.TMR0IP = 1;
	//Initialise le flag d'interruption
	INTCONbits.TMR0IF = 0;
	//Active l'interruption sur overflow
	INTCONbits.TMR0IE = 1;
	//Prescale le timer0
	TMR0H = 0xF3;
	TMR0L = 0xC9;
	//Active le timer 0
	T0CON = 0x86;

	ADCON1 = 0 ; // mettre les broches RB0 à RB3 en mode digital
	
	
	TRISB=0b00000011;
	
	TRISA=0xFF;

	
	OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON &
	USART_ASYNCH_MODE & USART_EIGHT_BIT &
	USART_CONT_RX & USART_BRGH_HIGH, 8);
	
	putrsUSART("Hello !\n\r");
	
	vitesse = 0.0;

	
	while(1){

		
	}
}

#pragma code
#pragma interrupt InterruptServiceHigh

void InterruptServiceHigh()
{
	char result;
	char data;

	if(PIR1bits.RCIF){
		//Reinitialise le flag d'interruption de l'USART
		PIR1bits.RCIF = 0;

	//	while (!DataRdyUSART());
		result = ReadUSART();

		if(result == 'G'){
			PORTBbits.RB2=1;
			putrsUSART("Salut \n\r");
			vitesse = vitesse + 1.5;
		}

		if(result == 'S'){
			PORTBbits.RB3 -= +1;
			putrsUSART("Plop \n\r");
			vitesse = vitesse - 1.5;
		}
	}	
	if(INTCONbits.INT0IF){ //si interruption sur INT0/RB0
		INTCONbits.INT0IF = 0; // Reset flag interruption
		PORTBbits.RB3 += 1; // Change l'état de la sortie RB3
		putrsUSART("Plop \n\r");

	
	}
	if(INTCONbits.RBIF){ //si interruption sur RB4 à RB7
		INTCONbits.RBIF = 0; // Reset flag interruption
		PORTBbits.RB3 = -PORTBbits.RB3 + 1; // Change l'état de la sortie RB3

	
	}
	if(INTCON3bits.INT1IF){ //si interruption sur RB1
		INTCON3bits.INT1IF = 0; // Reset flag interruption
		PORTBbits.RB3 = -PORTBbits.RB3 + 1; // Change l'état de la sortie RB3

	
	}
	
	if (INTCONbits.TMR0IF){
		//Reinitialise le flag d'interruption du timer0
		INTCONbits.TMR0IF = 0;
		//Prescale le timer0
		TMR0H = 0xF3;
		TMR0L = 0xC9;
		putrsUSART("Tick timer");
	}
	
}

#pragma code
#pragma interrupt InterruptServiceLow

void InterruptServiceLow()
{
	
} 