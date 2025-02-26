#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

// Définitions des constantes
#define PORT 8080
#define MAX_JOUEURS 2
#define MAX_MANCHES 5
#define MAX_PARTIES 2
#define MAX_CARTES 50


// Structures
typedef struct
{
    int manches_reussies[MAX_PARTIES];
    int valeurs_loupes[MAX_PARTIES];
    double moyenne_carte_loupee;
    double temps_reaction[MAX_PARTIES][MAX_MANCHES];
    double temps_reaction_Partie[MAX_PARTIES];
    double temps_reaction_moyen;
} Statistiques;

// Variables globales
extern int joueurs[MAX_JOUEURS];
extern char joueurs_nom[MAX_JOUEURS][250];
extern int cartes_jouees[MAX_CARTES];
extern int cartes_generes[MAX_CARTES];
extern int joueur_count;
extern int carte_count;
extern int joueur_count_nom;
extern pthread_mutex_t lock1, lock2, lock3, lock4, lock5, lock6;
extern pthread_cond_t condition_tour;
extern Statistiques stats;
extern int nombre_partie;
extern int cartes_reçues;
extern int manche_valide;
extern int active_joueurs;
extern int isModifier;

// Fonctions
void generation_fichier_stats();
void initialiser_stats();
void actualise_stats();
void affichage_stats();
void *gestionClient(void *joueur_sock);
int genere();

#endif // SERVER_H
