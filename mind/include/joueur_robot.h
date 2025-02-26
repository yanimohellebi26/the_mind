#ifndef JOUEUR_ROBOT_H
#define JOUEUR_ROBOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <python3.12/Python.h>

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
extern int derniere_carte_jouee;

// Fonctions
int trouver_plus_petite_carte();
double decision_jouer_ou_attendre(int derniere_carte, int plus_petite_carte, int nombre_joueurs);
void *threadEnvoi(void *arg);
void *threadReception(void *arg);

#endif // JOUEUR_ROBOT_H