#include <p18f4680.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>

#pragma config OSC = IRCIO67
#pragma config MCLRE = ON
#pragma config WDT = OFF

void InterruptServiceHigh(void);
void InterruptServiceLow(void);

double vitesse;
char buf[32];

#pragma code InterruptVectorHigh = 0x08

void InterruptVectorHigh(void)
{
	_asm
	goto InterruptServiceHigh
	_endasm
} 

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
	//Initialise le flag d'interruption
	PIR1bits.RCIF = 0;
	//Active la réception du RS232
	RCSTAbits.CREN= 1;
	//Interruption sur reception RS232 en prio haute
	IPR1bits.RCIP = 1;
	//Active l'interruption sur reception RS232
	PIE1bits.RCIE = 1;
	

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
	
	
	TRISB=0x00;
	PORTBbits.RB1=1;
	TRISC=0x00;
	
	OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON &
	USART_ASYNCH_MODE & USART_EIGHT_BIT &
	USART_CONT_RX & USART_BRGH_HIGH, 8); //baud = 57 000
	
	putrsUSART("Hello !\n\r");
	
	vitesse = 0.0;
	
	data = 0xAA;
    EEADR = 0x01;
    EEDATA = data;
    
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    
	INTCONbits.GIE = 0;
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;
    while(EECON1bits.WR);
    INTCONbits.GIE = 1;
		
	
	while(1){
		putrsUSART("Hello !\n\r");
		Delay10KTCYx(100);
		PORTB=0x00;
		Delay10KTCYx(100);
		PORTBbits.RB1=1;
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
			PORTBbits.RB3=1;
			putrsUSART("Plop \n\r");
			vitesse = vitesse - 1.5;
		}
		
		if(result == 'E'){
			EEADR=0x01;
			EECON1bits.EEPGD = 0;
 			EECON1bits.CFGS = 0;
 			EECON1bits.RD = 1;

 			data=EEDATA;
 		}
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