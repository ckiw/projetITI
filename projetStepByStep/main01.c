#include <p18f4680.h> //ensemble de d�finitions et de fonctions sp�cifiques au PIC18F4680

void main() { // fonction main
	// Configuration

	TRISBbits.TRISB0 = 0; //led rb0 en sortie	(1 pour configurer en entr�e)  

	//boucle principale, s'�x�cute � l'infini
	while(1){ 
		int i; //d�claration variable i
		PORTBbits.RB0 = !LATBbits.LATB0; // Change l'�tat de la led
		for(i= 0;i<5000;i++){}; //Boucle de temporisation
	}//fin while
}//fin main
