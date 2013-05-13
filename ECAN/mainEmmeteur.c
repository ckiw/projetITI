#include <p18f4680.h>
#include "ECAN.h"
#include <stdio.h>

//envoie à un intervalle régulier une trame can telle que
//id 123 , data 0 ou 1, longueur 1

#pragma config OSC = IRCIO7 // Oscillateur interne

void main() {

    long i;
    BYTE data = 1;
    BYTE longueur = 1;

    ECANInitialize();
    
    while (1) {

        if (data == 0) {
            data = 1;
        } else {
            data = 0;
        }
        ECANSendMessage(0x123, &data, longueur, 0b00000011);

        for (i = 0; i < 50000; i++) {
        }
    }
}

