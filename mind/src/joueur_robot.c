#include "../include/joueur_robot.h"
#include "../include/interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef MAX_CARTES
#define MAX_CARTES 100
#endif

#ifndef PORT
#define PORT 12345
#endif

int sockfd;
pthread_t thread_envoi, thread_reception;
int autorisation_envoyer = 0;
int manche = 0;
int valide = 1;
int mescartes[MAX_CARTES];
int derniere_carte_jouee = -1;
static char robot_name[250]; 


// Variable globale pour le nombre total de joueurs, robot inclus
int nbjoueur = 2; // Ajustez manuellement selon vos besoins (2 à 4 par exemple)

// Pour chaque joueur autre que le robot, on gère un deck fictif
// On suppose nbjoueur max = 4, donc 3 autres joueurs max
static int autre_joueur_connecte[3] = {0,0,0}; 
static int other_player_deck[3][MAX_CARTES];
static int other_player_count[3] = {0,0,0};

double decision_jouer_ou_attendre(int derniere_carte, int plus_petite_carte, int nombre_joueurs) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    double temps_attente = -1.0;

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('/home/yani/Bureau/the_mind/src')");

    pName = PyUnicode_DecodeFSDefault("decision");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "predict");
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(3);
            PyTuple_SetItem(pArgs, 0, PyLong_FromLong(plus_petite_carte));
            PyTuple_SetItem(pArgs, 1, PyLong_FromLong(derniere_carte));
            PyTuple_SetItem(pArgs, 2, PyLong_FromLong(nombre_joueurs));

            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                temps_attente = (double)PyFloat_AsDouble(pValue);
                Py_DECREF(pValue);
            } else {
                PyErr_Print();
                fprintf(stderr, "Erreur lors de l'appel de la fonction predict\n");
            }
        } else {
            PyErr_Print();
            fprintf(stderr, "La fonction predict n'a pas été trouvée ou n'est pas appelable\n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Le module decision n'a pas pu être importé\n");
    }

    return temps_attente;
}

int trouver_plus_petite_carte() {
    int min_carte = 101;
    for (int i = 0; i < manche; i++) {
        if (mescartes[i] < min_carte && mescartes[i] != 0) {
            min_carte = mescartes[i];
        }
    }
    return min_carte;
}

void *threadEnvoi(void *arg) {
    char carte[256];
    Py_Initialize();
    while (valide) {
        int cartes_envoyees = 1;
        while (autorisation_envoyer && valide) {
            int isVal = 0;
            while (!isVal && valide) {
                int plus_petite_carte = trouver_plus_petite_carte();
                if (plus_petite_carte == 101) {
                    autorisation_envoyer = 0;
                    break;
                }

                double temps_attente = decision_jouer_ou_attendre(derniere_carte_jouee, plus_petite_carte,nbjoueur);
                printf("Robot réfléchit pendant %.2f secondes...\n", temps_attente);
                sleep((int)temps_attente);

                snprintf(carte, sizeof(carte), "%d", plus_petite_carte);

                for (int i = 0; i < manche; i++) {
                    if (mescartes[i] == atoi(carte)) {
                        isVal = 1;
                        break;
                    }
                }
            }

            if (!valide) break;

            send(sockfd, carte, strlen(carte), 0);
            printf("Carte envoyée par le robot : %s\n", carte);

            derniere_carte_jouee = atoi(carte);
            for (int i = 0; i < manche; i++) {
                if (mescartes[i] == atoi(carte)) {
                    mescartes[i] = 0;
                    break;
                }
            }

            cartes_envoyees++;
            if (cartes_envoyees > manche) {
                autorisation_envoyer = 0;
            }

            int card_value = atoi(carte);
            // ** Révéler et déplacer la carte jouée par le robot (playerIndex = 0) **
            interface_reveal_and_move_card(0, card_value);
            
            interface_update_player_cards(0, mescartes, manche);
        }
    }
    Py_Finalize();
    return NULL;
}
void *threadReception(void *arg) {
    char message[256];
    int len;
    while (valide) {
        memset(message, 0, sizeof(message));
        len = recv(sockfd, message, sizeof(message), 0);
        if (len > 0) {
            message[len] = '\0';
            if (strcmp(message, "SIGNAL") == 0) {
                recv(sockfd, &manche, sizeof(manche), 0);
                for (int i = 0; i < manche; i++) {
                    recv(sockfd, &mescartes[i], sizeof(mescartes[i]), 0);
                }
                printf("Signal reçu. Cartes distribuées pour le robot :\n");
                for (int i = 0; i < manche; i++) {
                    printf("carte%d : %d  ", i + 1, mescartes[i]);
                }
                printf("\n");
                autorisation_envoyer = 1;
                interface_update_player_cards(0, mescartes, manche);

                // Connecter et attribuer des decks face cachée aux autres joueurs
                for (int p = 1; p < nbjoueur; p++) {
                    int idx = p - 1; // index dans autre_joueur_connecte
                    if (!autre_joueur_connecte[idx]) {
                        char autre_nom[50];
                        snprintf(autre_nom, sizeof(autre_nom), "Joueur%d", p);
                        interface_player_connected(p, autre_nom);
                        for (int i = 0; i < manche; i++) {
                            other_player_deck[idx][i] = -1;  // cartes face cachée
                        }
                        other_player_count[idx] = manche;
                        interface_update_player_cards(p, other_player_deck[idx], other_player_count[idx]);
                        autre_joueur_connecte[idx] = 1;
                    }
                }

            } else if (strcmp(message, "GAGNE") == 0) {
                printf("Manche gagnée\n");
                interface_affiche_message("Manche Gagnée !");
            } else if (strcmp(message, "DEBUT") == 0) {
                printf("DEBUT DE PARTIE\n");
                interface_affiche_message("Début de la Partie");
            } else if (strcmp(message, "FIN") == 0) {
                printf("FIN DE PARTIE\n");
                interface_affiche_message("Fin de la Partie");
            } else if (strcmp(message, "PERDU") == 0) {
                printf("Manche Perdue\n");
                interface_affiche_message("Manche Perdue !");
            } else if (strcmp(message, "FINS") == 0) {
                printf("C'est fini\n");
                valide = 0;
                interface_affiche_message("Partie Terminée");
                break;
            } else {
                char nom[250];
                memset(nom, 0, sizeof(nom));
                recv(sockfd, nom, sizeof(nom), 0);
                printf("%s a joué: %s \n", nom, message);
                derniere_carte_jouee = atoi(message);

                int played_card = atoi(message);

                // Déterminer le playerIndex du joueur à partir du nom ou d'une autre logique.
                int other_player_index = -1;
                for (int p = 1; p < nbjoueur; p++) {
                    char expectedName[50];
                    snprintf(expectedName, sizeof(expectedName), "Joueur%d", p);
                    if (strcmp(nom, expectedName) == 0) {
                        other_player_index = p;
                        break;
                    }
                }

                // Si on n'a pas trouvé, par défaut playerIndex=1 s'il n'y a que 2 joueurs.
                if (other_player_index == -1 && nbjoueur == 2) {
                    other_player_index = 1;
                }

                interface_reveal_and_move_card(other_player_index, played_card);
                interface_update_player_cards(other_player_index, other_player_deck[other_player_index - 1], other_player_count[other_player_index - 1]);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;

    // Vous pouvez changer nbjoueur manuellement ici
    // nbjoueur = 4 par exemple si vous voulez 4 joueurs total
    nbjoueur = 2; // Valeur par défaut, modifiez comme vous le souhaitez
    // Vous pouvez aussi le lire depuis argv si vous le voulez
    // if (argc > 2) nbjoueur = atoi(argv[2]);

    interface_init();
    interface_init_players();

    for (int i = 0; i < MAX_CARTES; i++) {
        mescartes[i] = 0;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur de création du socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide / non supportée");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connexion échouée");
        return -1;
    }

    printf("Connecté au serveur\n");

    if (argc > 1) {
        strncpy(robot_name, argv[1], sizeof(robot_name)-1);
        robot_name[sizeof(robot_name)-1] = '\0';
    } else {
        strncpy(robot_name, "Robot", sizeof(robot_name)-1);
        robot_name[sizeof(robot_name)-1] = '\0';
    }

    send(sockfd, robot_name, strlen(robot_name), 0);
    sleep(1);
    char nombre_parties[] = "2";
    send(sockfd, nombre_parties, strlen(nombre_parties), 0);

    interface_player_connected(0, robot_name);

    pthread_create(&thread_envoi, NULL, threadEnvoi, NULL);
    pthread_create(&thread_reception, NULL, threadReception, NULL);

    interface_run_loop();

    pthread_join(thread_envoi, NULL);
    pthread_join(thread_reception, NULL);

    close(sockfd);
    return 0;
}