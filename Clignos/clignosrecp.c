#include <p18f4680.h>
#include "ECAN.h"
#include <stdio.h>

//la reception d'une trame can définie
//id 123 , data 0 ou 1, longueur 1 allume une led sur rb1


void InterruptServiceHigh(void);
void InterruptServiceLow(void);

// écrit la fonction InterruptVectorHigh dans la zone mémoire 0x08 définie par le constructeur
//(en fait sur une interruption le microcontrolleur va voir à cette adresse)cf p113
#pragma code InterruptVectorHigh = 0x08

void InterruptVectorHigh(void) {
    _asm
    goto InterruptServiceHigh
            _endasm
}
// écrit la fonction InterruptVectorLow dans la zone mémoire 0x18 définie par le constructeur
//(en fait sur une interruption le microcontrolleur va voir à cette adresse)
#pragma code InterruptVectorLow = 0x18

void InterruptVectorLow(void) {
    _asm
    goto InterruptServiceLow
            _endasm
}

#pragma config OSC = IRCIO7 // Oscillateur interne

BOOL clignoGauche = 0;
BOOL clignoDroit = 0;

void main() {

    //Configure l'oscillateur interne sur 4MHz
    OSCCON = 0xEE;
    //Active les priorités sur interruption
    RCONbits.IPEN = 1;
    //Active les interruptions de haute prio
    INTCONbits.GIE = 1;
    //Active les interruptions de basse prio
    INTCONbits.PEIE = 1;

    //Définit les priorités hautes sur interruption de reception CAN
    IPR3bits.RXB0IP = 1; //pour le buffer 0
    IPR3bits.RXB1IP = 1; //pour le buffer 1

    //Active les interruptions sur reception CAN (quand un buffer est plein)
    PIE3bits.RXB0IE = 1; // sur le buffer 0
    PIE3bits.RXB1IE = 1; // sur le buffer 1

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

    TRISBbits.TRISB0 = 0; //led rb0 en sortie
    TRISBbits.TRISB1 = 0; //led rb1 en sortie

    ECANInitialize();

    PORTBbits.RB0 = 0;
    PORTBbits.RB1 = 0;





    while (1) {


    }
}

#pragma code
#pragma interrupt InterruptServiceHigh

void InterruptServiceHigh() {

    if (PIR3bits.RXB0IF || PIR3bits.RXB1IF) { //si message dans un buffer lit le message
        unsigned long id;
        BYTE data[8];
        BYTE dataLen;
        ECAN_RX_MSG_FLAGS msgFlag;

        while (!ECANReceiveMessage(&id, data, &dataLen, &msgFlag));
        switch (id) {
            case 0x222:
                clignoGauche = data[0];
                clignoDroit = data[1];
                
                break;
        }
    }

    if (PIR1bits.RCIF) {
        //Reinitialise le flag d'interruption de l'USART
        PIR1bits.RCIF = 0;
    }

    if (INTCONbits.INT0IF) { //si interruption sur INT0/RB0
        INTCONbits.INT0IF = 0; // Reset flag interruption
    }

    if (INTCONbits.RBIF) { //si interruption sur RB4 à RB7
        INTCONbits.RBIF = 0; // Reset flag interruption
    }

    if (INTCON3bits.INT1IF) { //si interruption sur RB1
        INTCON3bits.INT1IF = 0; // Reset flag interruption
    }

    if (INTCONbits.TMR0IF) {
        //Reinitialise le flag d'interruption du timer0
        INTCONbits.TMR0IF = 0;
        //Prescale le timer0
        TMR0H = 0xF3;
        TMR0L = 0xC9;
        if (clignoGauche && clignoDroit) {
            PORTBbits.RB1 = PORTBbits.RB0 = !LATBbits.LATB1; //Permet que les deux clignos clignottent en même temps en warning

        } else if (clignoGauche) {

            PORTBbits.RB1 = !LATBbits.LATB1;
            PORTBbits.RB0 = 0;
        } else if (clignoDroit) {
            PORTBbits.RB0 = !LATBbits.LATB0;
            PORTBbits.RB1 = 0;
        } else {
            PORTBbits.RB0 = 0;
            PORTBbits.RB1 = 0;
        }


    }
}

#pragma code
#pragma interrupt InterruptServiceLow

void InterruptServiceLow() {

}