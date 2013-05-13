#include <p18f4680.h>
#include "ECAN.h"
#include <stdio.h>

//� l'appui sur un bouton situe sur RB0/INT0/pin 33 change l'�tat d'une led en XX (il
// faut penser � enlever le cavalier des leds car RB0 est reli� avec)
//envoie un message can avec l'id 222
#pragma config PBADEN = OFF //Si ON configure portB de 0 � 4 en analogique

void InterruptServiceHigh(void);
void InterruptServiceLow(void);

// �crit la fonction InterruptVectorHigh dans la zone m�moire 0x08 d�finie par le constructeur
//(en fait sur une interruption le microcontrolleur va voir � cette adresse)cf p113
#pragma code InterruptVectorHigh = 0x08

void InterruptVectorHigh(void) {
    _asm
    goto InterruptServiceHigh
            _endasm
}
// �crit la fonction InterruptVectorLow dans la zone m�moire 0x18 d�finie par le constructeur
//(en fait sur une interruption le microcontrolleur va voir � cette adresse)
#pragma code InterruptVectorLow = 0x18

void InterruptVectorLow(void) {
    _asm
    goto InterruptServiceLow
            _endasm
}

#pragma config OSC = IRCIO7 // Oscillateur interne

void main() {

    BYTE data = 0;

    TRISBbits.TRISB5 = 1; // rb0 en entr�e (bouton)

    //Configure les interruptions externes sur INT0 sur front (1:montant 0:descendant)
    INTCON2bits.INTEDG0 = 0;
    //Active les interruptions externes sur INT0
    INTCONbits.INT0IE = 1;

    //Active l'interruption sur les changement de RB4 � RB7 p117
    INTCONbits.RBIE = 1;
    //Initialise le flag interruption sur les changement de RB4 � RB7
    INTCONbits.RBIF = 0;
    //Haute priorit� sur interruption sur les changement de RB4 � RB7
    INTCON2bits.RBIP = 1;

    //Configure l'oscillateur interne sur 4MHz
    OSCCON = 0xEE;
    //Active les priorit�s sur interruption
    RCONbits.IPEN = 1;
    //Active les interruptions de haute prio
    INTCONbits.GIE = 1;
    //Active les interruptions de basse prio
    INTCONbits.PEIE = 1;

    ECANInitialize();



    ECANSendMessage(0x223, &data, 8, 0b00000011);

    while (1) {





    }
}

#pragma code
#pragma interrupt InterruptServiceHigh

void InterruptServiceHigh() {


    if (PIR3bits.RXB0IF || PIR3bits.RXB1IF) { //si message dans un buffer CAN lit le message
        //        unsigned long id;
        //        BYTE data[8];
        //        BYTE dataLen;
        //        ECAN_RX_MSG_FLAGS msgFlag;
        //
        //        while (!ECANReceiveMessage(&id, data, &dataLen, &msgFlag)); //remet le flag � 0
        //        switch (id) {
        //            case 0x123:
        //
        //                break;
        //
        //        }
    }

    if (PIR1bits.RCIF) {
        //Reinitialise le flag d'interruption de l'USART
        PIR1bits.RCIF = 0;
    }

    if (INTCONbits.INT0IF) { //si interruption sur INT0/RB0
        BYTE data = 1;

        INTCONbits.INT0IF = 0; // Reset flag interruption
        ECANSendMessage(0x222, &data, 1, 0b00000011);

    }

    if (INTCONbits.RBIF) { //si interruption sur RB4 � RB7
        BYTE data = 1;
        unsigned char b = PORTB; //voir page 117
        INTCONbits.RBIF = 0; // Reset flag interruption

        ECANSendMessage(0x333, &data, 1, 0b00000011);
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