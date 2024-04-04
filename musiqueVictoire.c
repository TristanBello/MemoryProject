#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

// Définition des broches GPIO pour le buzzer
#define BUZZER_PIN 1

// Fréquences des notes de victoire
int scale [6] = { 400, 400, 400, 700, 400, 700 } ;

int main(void) {
    wiringPiSetup () ;
    softToneCreate (BUZZER_PIN) ;
    
    //Musique de victoire
    int i;
    for(i = 0 ; i < 4 ; i++) {
    	softToneWrite (BUZZER_PIN, scale [i]) ;
        delay (200) ;
        softToneWrite (BUZZER_PIN, 0) ;
        delay (50) ;
    }
    delay (200) ;
    softToneWrite (BUZZER_PIN, scale [i]) ;
    delay (200) ;
    softToneWrite (BUZZER_PIN, 0) ;
    delay (50) ;
    i++;
    softToneWrite (BUZZER_PIN, scale [i]) ;
    delay (500) ;
    softToneWrite (BUZZER_PIN, 0) ;
    delay(50);
    return 0;
}
