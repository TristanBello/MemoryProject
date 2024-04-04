#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *message = "Bonjour serveur, c'est le client !";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "192.168.97.137", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connecté au serveur.\n");

    int i;
    //for (i = 0; i < 5; i++) { // Change 5 to the desired number of iterations
        valread = read( sock , buffer, 1024);
        printf("Message du serveur : %s\n",buffer );
        send(sock , message , strlen(message) , 0 );
        printf("Message envoyé au serveur.\n");
    //}
    
    while(1) {
		
	}

    close(sock);
    printf("Connexion avec le serveur terminée.\n");

    return 0;
}
