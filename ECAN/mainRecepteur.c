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

    TRISBbits.TRISB1 = 0; //led rb1 en sortie

    ECANInitialize();

    PORTBbits.RB1 = 1;

    while (1) {
        int i;
        BYTE data[8];
        float datb = 1.0;
        unsigned int datc= 60000;
        unsigned long int datd = 4000000000;
        data[0] = 0xAA;
        data[1] = 0xBB;
        data[2] = 0xCC;
        data[3] = 0xCC;
        data[4] = 0xDD;
        data[5] = 0xEE;
        data[6] = 0xFF;
        data[7] = 0x00;


        ECANSendMessage(0x123, data,8,0b00000011);
        ECANSendMessage(0x124, &datb,4,0b00000011);
        ECANSendMessage(0x125, datc,2,0b00000011);
        ECANSendMessage(0x126, datd,4,0b00000011);
        for(i= 0;i<1000;i++){};

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
            case 0x222: PORTBbits.RB1 = !LATBbits.LATB1;

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
        //TMR0H = 0xF3;
        //TMR0L = 0xC9;

    }
}

#pragma code
#pragma interrupt InterruptServiceLow

void InterruptServiceLow() {

}