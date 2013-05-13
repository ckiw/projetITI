#include <stdio.h>
#include <p18f4680.h>

#include "ECAN.h"

#pragma config OSC=RC //Horloge sur OSC1, et horloge divisée par 4 sur OSC2 en sortie

void main() {
    unsigned long id;
    BYTE data[8];
    BYTE dataLen;
    ECAN_RX_MSG_FLAGS flags;

    TRISBbits.TRISB2 = 0; //CANRX en entrée
    TRISBbits.TRISB3 = 1; //CANTX en sortie


    // ECAN.def file must be set up correctly.
    ECANInitialize();
    while (!ECANSendMessage(0x123, data, 0, ECAN_TX_STD_FRAME));
    do {
        // Wait for a message to get received.
        while (!ECANReceiveMessage(&id, data, &dataLen, &flags));
        // Increment received id and echo it back.
        id++;
        while (!ECANSendMessage(id, data, dataLen, flags));
    } while (1);
}