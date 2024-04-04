#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

// Définition des broches GPIO pour le buzzer
#define BUZZER_PIN 1


int main(void) {
    wiringPiSetup () ;
    softToneCreate (BUZZER_PIN) ;
    
    //Musique de defaite
    int i;
    for(i=0;i<3;i++){
	    softToneWrite (BUZZER_PIN, 100) ;
	    delay (500) ;
	    softToneWrite (BUZZER_PIN, 0) ;
	    delay(200);
    }
    softToneWrite (BUZZER_PIN, 100) ;
    delay (1500) ;
    softToneWrite (BUZZER_PIN, 0) ;
    delay(50);
    return 0;
}
