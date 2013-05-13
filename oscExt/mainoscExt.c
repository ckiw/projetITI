#include <p18f4680.h>
#include "ECAN.h"
#include <stdio.h>

//Programme pour tester que l'oscillateur externe fonctionne bien 40MHz

#pragma config PBADEN = OFF //Si ON configure portB de 0 à 4 en analogique

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

#pragma config OSC = EC // Oscillateur externe + osc2 comme horloge divisée par 4

void main() {

    BYTE data = 0;

    //Configure les interruptions externes sur INT0 sur front (1:montant 0:descendant)
    INTCON2bits.INTEDG0 = 0;
    //Active les interruptions externes sur INT0
    INTCONbits.INT0IE = 1;

    //Configure les interruptions externes sur INT1 sur front (1:montant 0:descendant)
    INTCON2bits.INTEDG1 = 0;
    //Configure l'interruption externe INT1 sur 1:Haut prio 0:Basse prio
    INTCON3bits.INT1IP = 1;
    //Active les interruptions externes sur INT1
    INTCON3bits.INT1IE = 1;

    //Active l'interruption sur les changement de RB4 à RB7 p117
    INTCONbits.RBIE = 1;
    //Initialise le flag interruption sur les changement de RB4 à RB7
    INTCONbits.RBIF = 0;
    //Haute priorité sur interruption sur les changement de RB4 à RB7
    INTCON2bits.RBIP = 1;

    //Configure l'oscillateur interne sur 4MHz
    OSCCON = 0xEE;
    //Active les priorités sur interruption
    RCONbits.IPEN = 1;
    //Active les interruptions de haute prio
    INTCONbits.GIE = 1;
    //Active les interruptions de basse prio
    INTCONbits.PEIE = 1;

    TRISBbits.TRISB0 = 1; // rb0 en entrée (bouton)
    TRISBbits.TRISB1 = 1; // rb1 en entrée (bouton)

    TRISDbits.TRISD0 = 1; // rb0 en entrée (bouton)
    TRISDbits.TRISD1 = 1; // rb1 en entrée (bouton)


    ECANInitialize();



    ECANSendMessage(0x224, &data, 8, 0b00000011);

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
        //        while (!ECANReceiveMessage(&id, data, &dataLen, &msgFlag)); //remet le flag à 0
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
        BYTE data[2];
        data[0] = LATDbits.LATD0;
        data[1] = LATDbits.LATD1;
        //voir page 117 si erreur


        ECANSendMessage(0x222, data, 2, 0b00000011);
        INTCONbits.INT0IF = 0; // Reset flag interruption

    }
    //        if (INTCON3bits.INT1IF) { //si interruption sur RB1

    //        INTCON3bits.INT1IF = 0; // Reset flag interruption

    //    }

    if (INTCONbits.RBIF) { //si interruption sur RB4 à RB7
        INTCONbits.RBIF = 0; // Reset flag interruption
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