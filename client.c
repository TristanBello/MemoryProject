#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <wiringPiI2C.h>
#include <time.h>
#include <wiringPi.h>
#define PORT 5000

///////////////Définition variables et fonctions//////////////////////////
#define ADDR 0x70							
int fd;									//
									//
// Tableau des chiffres							//
const int chiffres[] = {						//
        0b00111111, // 0						//						
        0b00000110, // 1						//
        0b01011011, // 2						//
        0b01001111, // 3						//
        0b01100110, // 4						//
        0b01101101, // 5						//
        0b01111101, // 6						//
        0b00000111, // 7						//
        0b01111111, // 8						//
        0b01101111  // 9						//
};									//
									//
// Déclaration des fonctions utiles pour l'afficheur 7 segments		//
void initDisplay();							//
void afficherCombinaison();						//
void setDigit(int digit, int value);					//
void clearDisplay();							//
void afficherCombinaison(char *combinaison);				//
									//
// Déclaration des fonctions utiles pour le clavier 3*3			//
#define LOW 0								//
#define HIGH 1								//
									//
int rows[4] = {2, 3, 21, 22};						//
int cols[4] = {6, 25, 24, 23};						//
									//
void initMatriceBtn();							//
int trouverToucheAppuye(int j, int i);					//
//////////////////////////////////////////////////////////////////////////


int main(int argc, char const *argv[]) {
    //Initialisation du 7 segment grâce à la commande python sinon fonctionne pas
    system("sudo python3 ../../Joy-Pi/segment.py");
    
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *message = "Bonjour serveur, c'est le client !";
    char buffer[1024] = {0};
    char score[1024] = {0};
    
    // Initialisation des composants nécessaires
    wiringPiSetup(); // setup la librairie wiringPi
    initDisplay();
    initMatriceBtn();

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "192.168.97.14", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connecté au serveur.\n");
    printf("Début de la partie\n");
    
    // Boucle principale qui joue avec le serveur
    while(1){
    	printf("Reception de la combinaison ...\n");
	valread = read( sock , buffer, 1024);
	printf("Combinaison bien reçue !\n");
	afficherCombinaison(buffer);
	
	// Enregistrement de la combinaison tapée au clavier
	int i, j, nbTouches = 0;
	char choixCombinaison[1024];
	memset(choixCombinaison, 0, sizeof(choixCombinaison)); // Initialisation à une chaîne vide
	printf("Vous devez retaper la combinaison sans faire de fautes, bonne chance !\n");
    	while (nbTouches<strlen(buffer)) {
		for (i = 0; i < 3; i++) {
		    digitalWrite(cols[i], LOW);
		    for (j = 0; j < 3; j++) {
		        if (digitalRead(rows[j]) == LOW) {
		        	int touche = trouverToucheAppuye(j, i); // Récupère la touche appuyée
		        	char touche_str[2]; // Chaîne de caractères pour stocker la touche 
                		sprintf(touche_str, "%d", touche); // Convertit l'entier en une chaîne
                		strcat(choixCombinaison, touche_str);// concatener la touche
		            	while (digitalRead(rows[j]) == LOW); // Wait for button release
		            	nbTouches++;
		        }
		    }
		    digitalWrite(cols[i], HIGH);
		}
	}
	printf("Combinaison tapée :[%s]\n",choixCombinaison);
	if(strcmp(choixCombinaison,buffer)==0){
		system("./musiqueVictoire");
		printf("Combinaison trouvée, vous remportez 1 point !\n");
		send(sock , "OK" , strlen("OK") , 0 );
	}
	else {
		system("./musiqueDefaite");
		printf("Mauvaise combinaison, dommage !\n");
		send(sock , "NOK" , strlen("NOK") , 0 );
		//Reception du score
		valread = read( sock , score, 1024);
		printf("Merci d'avoir joué, vous avez un score de [%spts]. A bientot !\n",score);
		close(sock);
    		printf("Connexion avec le serveur terminée.\n");
    		return 0;
	}
    }	    
    
    close(sock);
    printf("Connexion avec le serveur terminée.\n");
    return 0;
}


// Fonction qui initialise des 7 segments en I2C
void initDisplay() {
	// Initialize I2C communication
	fd=wiringPiI2CSetup(ADDR);
}

// Fonction qui affiche un chiffre sur un digit
void setDigit(int digit, int value) {
	// Write value to the specified digit
	wiringPiI2CWriteReg8(fd, digit, value);
}

// Fonction qui efface les 7 segments
void clearDisplay() {
	// Clear display
	wiringPiI2CWriteReg8(fd, 0, 0);
	wiringPiI2CWriteReg8(fd, 2, 0);
	wiringPiI2CWriteReg8(fd, 6, 0);
	wiringPiI2CWriteReg8(fd, 8, 0);
}

// Fonction pour afficher la combinaison reçue
void afficherCombinaison(char *combinaison){
	int i;
	printf("\nSoyez pret à retenir la combinaison !\n");
		
	//Affichage du décompte
	for(i=3;i>0;i--){
		printf("[%d]\n",i);
		delay(1000);
	}
	printf("[GO]\n");
	//printf("Combinaison : [%s]\n",combinaison);
	clearDisplay();
	for(i=0;i<strlen(combinaison);i++){
		int nb = combinaison[i] - '0'; // Convertit le caractère en entier
		setDigit(0, chiffres[nb]);
		setDigit(2, chiffres[nb]);
		setDigit(6, chiffres[nb]);
		setDigit(8, chiffres[nb]);
		delay(1200);
		clearDisplay();
		delay(250);
	}
}

// Fonction qui initialise la matrice de boutons
void initMatriceBtn() {
    int i;
    
    for (i = 0; i < 4; i++) {
        pinMode(rows[i], INPUT);
        pullUpDnControl(rows[i], PUD_UP);
        pinMode(cols[i], OUTPUT);
        digitalWrite(cols[i], HIGH);
    }
}

// Fonction qui trouve la touche qui a été appuyée
int trouverToucheAppuye(int j, int i) {
    int key = j * 3 + i + 1; // Calculate the key number based on row and column
    //printf("Touche %d\n", key);
    return key;
}
