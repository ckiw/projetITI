#include <p18f4680.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>

#pragma config OSC = IRCIO67


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

	char i;
	char text[] = "bite couille poil";	
	//Configure l’oscillateur interne sur 8MHz
	OSCCON=0xF6;

	


	
	
	TRISB=0x00;
	PORTBbits.RB1=1;
	TRISC=0x00;
	
	OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON &
	USART_ASYNCH_MODE & USART_EIGHT_BIT &
	USART_CONT_RX & USART_BRGH_HIGH, 51);
	


	putcUSART(0x7C); //Caractère de commande
	putcUSART(0x9C); //Luminosité au max


    putcUSART('A');
	putcUSART('b');
	Delay10KTCYx(200);

	putcUSART(0xFE); //Caractère position
	putcUSART(148); //Position 0 (ligne 1/4, caractère 1/20)
	
	
	i = 'A';
	putrsUSART("bonjourTuVeuxEtreMonAmiPourToujoursEtAussiAfficherDuTexteCommeJeTeDemande?Ouimai");
	Delay10KTCYx(50);
	
	while(1){


		if(i != 0x7C){
			if( i == 0xFE ){
				
				putcUSART(128);
			} else {
				putcUSART(i);
			}
		}
	 	Delay10KTCYx(20);
	 	PORTB=0x00;
	 	Delay10KTCYx(20);
	 	PORTBbits.RB1=1;
	 	i++;
		
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
			putrsUSART("Salut ");
			vitesse = vitesse + 1.5;
		}

		if(result == 'S'){
			PORTBbits.RB3=1;
			putrsUSART("Plop ");
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
	
	//if (INTCONbits.TMR0IF){
		//Reinitialise le flag d'interruption du timer0
	//	INTCONbits.TMR0IF = 0;
		//Prescale le timer0
	//	TMR0H = 0xF3;
	//	TMR0L = 0xC9;
	//	putrsUSART("Ti");
	//}
	
}

#pragma code
#pragma interrupt InterruptServiceLow

void InterruptServiceLow()
{
} 