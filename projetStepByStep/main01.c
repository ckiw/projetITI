#include <p18f4680.h> //ensemble de définitions et de fonctions spécifiques au PIC18F4680

void main() { // fonction main
	// Configuration

	TRISBbits.TRISB0 = 0; //led rb0 en sortie	(1 pour configurer en entrée)  

	//boucle principale, s'éxécute à l'infini
	while(1){ 
		int i; //déclaration variable i
		PORTBbits.RB0 = !LATBbits.LATB0; // Change l'état de la led
		for(i= 0;i<5000;i++){}; //Boucle de temporisation
	}//fin while
}//fin main
