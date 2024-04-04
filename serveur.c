#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 5000

int id = 1;

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char *message = "Bienvenue !";
    char *debutPartie = "La partie débute dans 5 secondes !";
    char *scoreJoueur = "Votre score : ";
    char buffer[1024] = {0};
    int valread;
    int nb, i;
    int score = 0;
    char aux[1];
    char scoreJ[3];
    char combinaison[1024];
    memset(combinaison, 0, sizeof(combinaison)); // Initialisation à une chaîne vide
    
    
    //Génération d'une cominaison aléatoire
    //for(i=0; i < 2; i++) {
        srand(time(NULL));
        nb = rand()%9999+1111;
        printf("%d", nb);
        sprintf(aux, "%d", nb); // Convertit l'entier en une chaîne
        strcat(combinaison, aux);
        usleep(200000);
    //}
    
    // Nous entrons des valeurs à la main car la fonction rand met le même chiffre à la suite
    /*strcat(combinaison, "1");
    strcat(combinaison, "6");
    strcat(combinaison, "2");
    strcat(combinaison, "5");
    strcat(combinaison, "8");
    * */
    
    // Envoyer un message initial au client
    printf("Nouveau joueur connecté.\n");
    //printf("[ Joueur %d ] : %s\n", id, buffer);
    
    printf("[ Joueur %d ] : La partie débute dans 5 secondes !\n", id);
    printf("[ Joueur %d ] : Nouvelle combinaison : %s\n", id, combinaison);
    int idJoueur = id;
    id++;
    
    while(1) {
        send(client_socket, combinaison, strlen(combinaison), 0);

        // Gestion de la communication avec le client
        //while ((valread = read(client_socket, buffer, 1024)) > 0) {
        valread = read(client_socket, buffer, 1024);
            
        printf("[ Joueur %d ] : Résultat %s\n", idJoueur, buffer);
        //}
        
        //Le joueur s'est trompé de combinaison
        if(strcmp(buffer, "NOK") == 0) {
            //On affiche le score du joueur sur le serveur
            printf("[ Joueur %d ] : Score = %dpts\n", idJoueur, score);
            
            //On envoie le score au joueur
            sprintf(scoreJ, "%d", score);
            printf("test scoreJ : %s\n", scoreJ);
            send(client_socket, scoreJ, strlen(scoreJ), 0);
            break;
        }
        
        //Le joueur tape la bonne combinaison
        if(strcmp(buffer, "OK") == 0) {
            //On génère un nouveau chiffre random à ajouter dans la combinaison
            srand(time(NULL));
            nb = rand()%9+1;
            sprintf(aux, "%d", nb); // Convertit l'entier en une chaîne
            strcat(combinaison, aux);
            
            //On écrit la nouvelle combinaison sur l'écran du serveur et on incrémente le score
            printf("[ Joueur %d ] : Nouvelle combinaison : %s\n", idJoueur, combinaison);
            score++;
        }
        
    }
    
    // Fermer la connexion avec le client
    close(client_socket);
    free(arg);
    printf("[ Joueur %d ] : La partie est terminée.\n", idJoueur);

    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    

    // Création du socket serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration du socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Attachement du socket au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Écoute de nouvelles connexions
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexion...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Créer un nouveau thread pour gérer la connexion client
        pthread_t tid;
        int *client_socket = malloc(sizeof(int));
        if (client_socket == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        *client_socket = new_socket;
        if (pthread_create(&tid, NULL, handle_client, (void *)client_socket) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }

        // Détacher le thread
        pthread_detach(tid);
    }

    return 0;
}
