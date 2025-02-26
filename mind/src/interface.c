#include "interface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

static const int CARD_WIDTH = 50;
static const int CARD_HEIGHT = 80;
static const int CARD_MARGIN = 10;

typedef struct {
    char name[256];
    int cards[100];
    int card_count;
    int base_x, base_y;    // Position de base pour dessiner les cartes
    int horizontal;        // 1 si les cartes sont alignées horizontalement
    bool connected;        // Le joueur est-il connecté ?
    SDL_Color card_color;  // Couleur spécifique au joueur
} Player;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;

static Player players[4];
static char current_message[256] = "En attente...";

// Dernière carte jouée
static int last_played_card = 0;
static char last_played_msg[256] = "";
static SDL_Color last_played_color = {255, 255, 255, 255};

// Couleurs différentes pour les 4 joueurs
static SDL_Color player_colors[4] = {
    {255, 0, 0, 255},   // Rouge (Joueur 0)
    {0, 0, 255, 255},   // Bleu  (Joueur 1)
    {255, 255, 0, 255}, // Jaune (Joueur 2)
    {0, 255, 0, 255}    // Vert  (Joueur 3)
};

void interface_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        return;
    }

    window = SDL_CreateWindow("Partie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        return;
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        fprintf(stderr, "Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }
}

void interface_init_players() {
    // Joueur 0 (ex: Robot, haut gauche)
    players[0].base_x = 50;
    players[0].base_y = 100;
    players[0].horizontal = 1;
    players[0].connected = false;

    // Joueur 1 (haut droite)
    players[1].base_x = WINDOW_WIDTH - 200;
    players[1].base_y = 100;
    players[1].horizontal = 1;
    players[1].connected = false;

    // Joueur 2 (bas gauche)
    players[2].base_x = 50;
    players[2].base_y = WINDOW_HEIGHT - 150;
    players[2].horizontal = 1;
    players[2].connected = false;

    // Joueur 3 (bas droite)
    players[3].base_x = WINDOW_WIDTH - 200;
    players[3].base_y = WINDOW_HEIGHT - 150;
    players[3].horizontal = 1;
    players[3].connected = false;

    // Initialiser leurs cartes à 0
    for (int p = 0; p < 4; p++) {
        players[p].card_count = 0;
        for (int c = 0; c < 100; c++) {
            players[p].cards[c] = 0;
        }
    }
}

void interface_player_connected(int playerIndex, const char *playerName) {
    if (playerIndex < 0 || playerIndex >= 4) return;
    strncpy(players[playerIndex].name, playerName, sizeof(players[playerIndex].name)-1);
    players[playerIndex].name[sizeof(players[playerIndex].name)-1] = '\0';
    players[playerIndex].connected = true;
    players[playerIndex].card_color = player_colors[playerIndex];
}

void interface_update_player_cards(int playerIndex, int *cards, int count) {
    if (playerIndex < 0 || playerIndex >= 4) return;
    players[playerIndex].card_count = count;
    for (int i = 0; i < count; i++) {
        players[playerIndex].cards[i] = cards[i];
    }
}

void interface_affiche_message(const char *message) {
    strncpy(current_message, message, sizeof(current_message));
    current_message[sizeof(current_message) - 1] = '\0';
}

void interface_reveal_and_move_card(int playerIndex, int card) {
    if (playerIndex < 0 || playerIndex >= 4) return;
    snprintf(last_played_msg, sizeof(last_played_msg), "%s a joué la carte %d", players[playerIndex].name, card);
    last_played_msg[sizeof(last_played_msg)-1] = '\0';
    last_played_card = card;
    last_played_color = players[playerIndex].card_color;
}

static void draw_text(const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        return;
    }

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect dst;
    dst.x = x - w/2; 
    dst.y = y - h/2; 
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

static void draw_player_cards(Player *p, int playerIndex) {
    if (!p->connected) return;

    int card_x = p->base_x;
    int card_y = p->base_y;
    SDL_Color black = {0,0,0,255};

    for (int i = 0; i < p->card_count; i++) {
        int card_value = p->cards[i];
        int w = CARD_WIDTH;
        int h = CARD_HEIGHT;

        if (playerIndex == 0) {
            // Joueur 0 (robot) : cartes face visible
            if (card_value == 0) {
                // Carte déjà jouée, ne pas afficher
                card_x += (w + CARD_MARGIN);
                continue;
            }
            SDL_SetRenderDrawColor(renderer, p->card_color.r, p->card_color.g, p->card_color.b, 255);
            SDL_Rect cardRect = {card_x, card_y, w, h};
            SDL_RenderFillRect(renderer, &cardRect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cardRect);

            char buf[16];
            snprintf(buf, sizeof(buf), "%d", card_value);
            draw_text(buf, card_x + w/2, card_y + h/2, black);

        } else {
            // Autres joueurs : cartes face cachée
            if (card_value == 0) {
                card_x += (w + CARD_MARGIN);
                continue;
            }

            // Carte face cachée (gris)
            SDL_SetRenderDrawColor(renderer, p->card_color.r/2, p->card_color.g/2, p->card_color.b/2, 255);
            SDL_Rect cardRect = {card_x, card_y, w, h};
            SDL_RenderFillRect(renderer, &cardRect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cardRect);
        }

        card_x += (w + CARD_MARGIN);
    }
}

static void render_scene() {
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255,255,255,255};
    draw_text(current_message, WINDOW_WIDTH/2, 50, white);

    // Dessiner les joueurs
    for (int p = 0; p < 4; p++) {
        draw_player_cards(&players[p], p);
    }

    // Dessiner la dernière carte jouée au centre
    if (last_played_card != 0) {
        int center_x = WINDOW_WIDTH / 2;
        int center_y = WINDOW_HEIGHT / 2;

        // Carte avec la couleur du joueur
        SDL_SetRenderDrawColor(renderer, last_played_color.r, last_played_color.g, last_played_color.b, 255);
        SDL_Rect cardRect = {center_x - CARD_WIDTH/2, center_y - CARD_HEIGHT/2, CARD_WIDTH, CARD_HEIGHT};
        SDL_RenderFillRect(renderer, &cardRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &cardRect);

        SDL_Color black = {0,0,0,255};
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", last_played_card);
        draw_text(buf, center_x, center_y, black);
        draw_text(last_played_msg, center_x, center_y + CARD_HEIGHT, black);
    }

    SDL_RenderPresent(renderer);
}

void interface_run_loop() {
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        render_scene();
        SDL_Delay(100);
    }
    interface_quit();
}

void interface_quit() {
    if (font) TTF_CloseFont(font);
    font = NULL;

    if (renderer) SDL_DestroyRenderer(renderer);
    renderer = NULL;

    if (window) SDL_DestroyWindow(window);
    window = NULL;

    TTF_Quit();
    SDL_Quit();
}
