#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include <SDL2/SDL.h>

// Initialise la fenêtre, le renderer, la police, etc.
void interface_init();

// Initialise les positions et paramètres des 4 joueurs (appel après interface_init())
void interface_init_players();


// Marque qu'un joueur est connecté, définit son nom, et le rend visible
// playerIndex : 0 à 3
void interface_player_connected(int playerIndex, const char *playerName);

// Met à jour les cartes d'un joueur (playerIndex) avec un tableau de count cartes
// Le robot (playerIndex = 0) verra ses cartes face visible (valeur)
// Les autres joueurs verront leurs cartes face cachée (toute valeur, typiquement -1 ou 0 pour inconnu)
void interface_update_player_cards(int playerIndex, int *cards, int count);

// Affiche un message global (ex: "Manche Gagnée")
void interface_affiche_message(const char *message);

// Révèle une carte au centre (carte jouée par playerIndex) et affiche le message correspondant
// Retire la carte de sa main (vous devez l'avoir fait côté logique), l'affiche au centre révélée
void interface_reveal_and_move_card(int playerIndex, int card);

// Lance la boucle d'événements SDL (bloquante)
// À appeler dans le thread principal
void interface_run_loop();

// Ferme et nettoie l'interface
void interface_quit();

#endif
