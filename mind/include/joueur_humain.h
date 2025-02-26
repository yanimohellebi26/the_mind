#ifndef JOUEUR_HUMAIN_H
#define JOUEUR_HUMAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

// Définitions des constantes
#define PORT 8080
#define MAX_CARTES 50

// Variables globales
extern int sockfd;
extern pthread_t thread_envoi, thread_reception;
extern int autorisation_envoyer;
extern int manche;
extern int valide;
extern int mescartes[MAX_CARTES];

// Fonctions
void *threadEnvoi(void *arg);
void *threadReception(void *arg);

#endif // JOUEUR_HUMAIN_H