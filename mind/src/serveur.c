#include "../include/serveur.h"

// Définition des variables globales
int joueurs[MAX_JOUEURS];
char joueurs_nom[MAX_JOUEURS][250];
int cartes_jouees[MAX_CARTES];
int cartes_generes[MAX_CARTES];
int joueur_count = 0;
int carte_count = 0;
int joueur_count_nom = 0;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock5 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock6 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_tour = PTHREAD_COND_INITIALIZER;
Statistiques stats;
int nombre_partie = 0;
int cartes_reçues = 0;
int manche_valide = 0;
int active_joueurs = 0;
int isModifier = 0;


/////////////////////////////////////////::::

//////////////////////////////////////////

void generation_fichier_stats()
{
    FILE *file = fopen("../data/stats.txt", "w");
    if (file == NULL)
    {
        perror("Erreur lors de la création du fichier stats");
        return;
    }
    FILE *file2 = fopen("../data/classement.txt", "a");
    if (file2 == NULL)
    {
        perror("Erreur lors de la création du fichier classement");
        return;
    }

    int total_manches = 0;
    int total_valeurs_loupees = 0;
    double meilleur_temps = 1e9; // Très grande valeur pour comparaison
    double pire_temps = 0.0;    // Initialiser à une valeur faible
    int meilleure_partie = -1;
    int pire_partie = -1;

    // Calcul des statistiques pour chaque partie
    for (int i = 0; i < nombre_partie; i++)
    {
        total_manches += stats.manches_reussies[i];
        if (stats.valeurs_loupes[i] > 0)
        {
            total_valeurs_loupees++;
        }
        if (stats.temps_reaction_Partie[i] < meilleur_temps)
        {
            meilleur_temps = stats.temps_reaction_Partie[i];
            meilleure_partie = i + 1;
        }
        if (stats.temps_reaction_Partie[i] > pire_temps)
        {
            pire_temps = stats.temps_reaction_Partie[i];
            pire_partie = i + 1;
        }

        fprintf(file, "%d %d %d %.4f\n", i + 1, stats.manches_reussies[i], stats.valeurs_loupes[i], stats.temps_reaction_Partie[i]);
    }

    stats.temps_reaction_moyen = total_manches > 0 ? stats.temps_reaction_moyen / total_manches : 0.0;

    // Ajouter un résumé des statistiques
    fprintf(file, "Total\t%d\t%d\t-\n", total_manches, total_valeurs_loupees);
    fprintf(file, "Meilleure Partie\t%d\t-\t%.4f\n", meilleure_partie, meilleur_temps);
    fprintf(file, "Pire Partie\t%d\t-\t%.4f\n", pire_partie, pire_temps);
    fprintf(file, "Temps de Réaction Moyen\t-\t-\t%.4f\n", stats.temps_reaction_moyen);

    fprintf(file2, "%d,%d\n", MAX_JOUEURS, total_manches);

    fclose(file);
    fclose(file2);
}

void initialiser_stats()
{
    for (int i = 0; i < MAX_PARTIES; i++)
    {
        stats.manches_reussies[i] = 0;
        stats.valeurs_loupes[i] = 0;
        stats.temps_reaction_Partie[i] = 0;
        for (int j = 0; j < MAX_MANCHES; j++)
            stats.temps_reaction[i][j] = 0.0;
    }
    stats.moyenne_carte_loupee = 0;
    stats.temps_reaction_moyen = 0;
}
void actualise_stats()
{
    double somme = 0.0;
    for (int i = 0; i < nombre_partie; i++)
    {
        somme += stats.valeurs_loupes[i];
    }
    stats.moyenne_carte_loupee = somme / nombre_partie;
    somme = 0.0;
    for (int i = 0; i < nombre_partie; i++)
    {
        double somme_partie = 0;
        for (int j = 0; j < MAX_MANCHES; j++)
        {
            somme += stats.temps_reaction[i][j];
            somme_partie += stats.temps_reaction[i][j];
        }
        stats.temps_reaction_Partie[i] = somme_partie*1000;
    }
    stats.temps_reaction_moyen = somme / (nombre_partie * MAX_MANCHES);
}

int genere()
{
    int nombre;
    int trouve;

    do
    {
        nombre = rand() % MAX_CARTES + 1; // Génère un nombre entre 1 et 100
        trouve = 0;

        // Vérifie si le nombre a déjà été généré ce manche
        for (int i = 0; i < MAX_CARTES; i++)
        {
            if (cartes_generes[i] == nombre)
            {
                trouve = 1;
                break;
            }
        }
    } while (trouve); // Recommence si le nombre est déjà dans cartes_generes

    // Ajoute le nombre au tableau cartes_generes
    for (int i = 0; i < MAX_CARTES; i++)
    {
        if (cartes_generes[i] == 0)
        { // Trouve la première case vide
            cartes_generes[i] = nombre;
            break;
        }
    }

    return nombre;
}

// Fonction de gestion des joueurs
void *gestionClient(void *joueur_sock)
{
    int sock = *(int *)joueur_sock;
    char buffer[256];
    int taille;

    int indice_nom = 0;
    for (int i = 0; i < joueur_count; i++)
    {
        if (joueurs[i] == sock)
        {
            indice_nom = i;
        }
    }
    for (int partie = 1; partie <= nombre_partie; partie++)
    {
        isModifier = 0;
        while (joueur_count != MAX_JOUEURS)
            ;

        send(sock, "DEBUT", strlen("DEBUT"), 0);
        sleep(1);
        for (int manche = 1; manche <= MAX_MANCHES; manche++)
        {
            clock_t start_time, end_time; // pour le temps de reaction;

            pthread_mutex_lock(&lock3);
            manche_valide = 1;
            carte_count = 0;
            int cartes_envoyees = 0;

            isModifier = 0;
            pthread_mutex_unlock(&lock3);
            send(sock, "SIGNAL", strlen("SIGNAL"), 0);
            sleep(1);
            send(sock, &manche, sizeof(manche), 0);
            sleep(1);
            for (int i = 0; i < manche; i++)
            {
                int nombre = genere();
                send(sock, &nombre, sizeof(nombre), 0);
                sleep(1);
            }
            start_time = clock();
            while ((taille = recv(sock, buffer, sizeof(buffer), 0)) > 0)
            {
                end_time = clock();
                buffer[taille] = '\0';

                pthread_mutex_lock(&lock4);
                stats.temps_reaction[partie - 1][manche - 1] += ((double)(end_time - start_time) / CLOCKS_PER_SEC);
                // Vérification si le joueur peut encore envoyer un message dans ce manche
                if (cartes_envoyees < manche)
                {
                    ////////////////////////////
                    cartes_jouees[carte_count++] = atoi(buffer);
                    ////////////////////////////
                    // Diffuser le message aux autres joueurs
                    for (int i = 0; i < joueur_count; i++)
                    {
                        if (joueurs[i] != sock)
                        {
                            send(joueurs[i], buffer, strlen(buffer), 0);
                            sleep(1);
                            send(joueurs[i], joueurs_nom[indice_nom], strlen(joueurs_nom[indice_nom]), 0);
                        }
                    }

                    cartes_envoyees++; // Incrémenter le compteur pour ce joueur
                    cartes_reçues++;   // Incrémenter le compteur de messages reçus par tous les joueurs
                }

                pthread_mutex_unlock(&lock4);
                if (cartes_envoyees >= manche)
                {
                    break; // Attendre le prochain signal avant d'accepter un autre message
                }
            }

            pthread_mutex_lock(&lock5);
            // Si tous les joueurs ont envoyé leur nombre de messages pour ce manche
            if (cartes_reçues == MAX_JOUEURS * manche)
            {
                cartes_reçues = 0; // Réinitialiser le compteur pour le prochain manche

                for (int i = 0; i < carte_count; i++)
                {
                    printf("jou %d ", cartes_jouees[i]);
                    fflush(stdout);
                }
                printf("\n");
                fflush(stdout);

                for (int i = 0; i < carte_count - 1; i++)
                {
                    if (cartes_jouees[i] > cartes_jouees[i + 1])
                    {
                        manche_valide = 0;
                        stats.valeurs_loupes[partie - 1] = cartes_jouees[i];
                        break;
                    }
                }

                ///////////////////////
                for (int i = 0; i < carte_count; i++)
                {
                    cartes_generes[i] = 0;
                    cartes_jouees[i] = 0;
                }

                ////////////////////////
                pthread_cond_broadcast(&condition_tour); // Réveiller tous les joueurs
            }
            else
            {

                // Attendre que tous les joueurs aient terminé d'envoyer leur message
                pthread_cond_wait(&condition_tour, &lock5);
            }

            pthread_mutex_unlock(&lock5);
            sleep(1); // Pause d'une seconde entre les tours
            if (!manche_valide)
            {
                sleep(2);
                send(sock, "PERDU", strlen("PERDU"), 0);
                sleep(1);
                break;
            }
            if (manche == MAX_MANCHES)
            {
                sleep(2);
                send(sock, "GAGNE", strlen("GAGNE"), 0);
                sleep(1);
            }
            pthread_mutex_lock(&lock1);
            if (manche_valide && !isModifier)
            {
                stats.manches_reussies[partie - 1]++;
                isModifier = 1;
            }
            pthread_mutex_unlock(&lock1);
            sleep(1);
        }

        send(sock, "FIN", strlen("FIN"), 0);
        sleep(1);
    }

    send(sock, "FINS", strlen("FINS"), 0);
    sleep(1);
    pthread_mutex_lock(&lock6);
    active_joueurs--;
    pthread_mutex_unlock(&lock6);

    close(sock);
    printf("Client déconnecté.\n");
    return NULL;
}

void affichage_stats()
{
    int total_manches = 0;
    int total_valeurs_loupees = 0; // Compteur des valeurs loupées
    double meilleur_temps = 1e9;
    double pire_temps = 0.0;
    int meilleure_partie = -1;
    int pire_partie = -1;

    for (int i = 0; i < nombre_partie; i++)
    {
        printf("Partie %d : %d manches réussies, valeur loupée : %d, temps de réaction : %.4f secondes\n",
               i + 1, stats.manches_reussies[i], stats.valeurs_loupes[i], stats.temps_reaction_Partie[i]);

        total_manches += stats.manches_reussies[i];
        if (stats.valeurs_loupes[i] > 0)
        {
            total_valeurs_loupees++;
        }
        if (stats.temps_reaction_Partie[i] < meilleur_temps)
        {
            meilleur_temps = stats.temps_reaction_Partie[i];
            meilleure_partie = i + 1;
        }
        if (stats.temps_reaction_Partie[i] > pire_temps)
        {
            pire_temps = stats.temps_reaction_Partie[i];
            pire_partie = i + 1;
        }
    }

    printf("Total des manches réussies : %d\n", total_manches);
    printf("Nombre total de valeurs loupées : %d\n", total_valeurs_loupees);
    printf("Meilleure partie : Partie %d avec un temps de réaction de %.4f secondes\n", meilleure_partie, meilleur_temps);
    printf("Pire partie : Partie %d avec un temps de réaction de %.4f secondes\n", pire_partie, pire_temps);
    printf("Temps de réaction moyen : %.4f secondes\n", stats.temps_reaction_moyen);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    srand(time(NULL));
    //////////////////////
    for (int i = 0; i < MAX_CARTES; i++)
    {
        cartes_generes[i] = 0;
        cartes_jouees[i] = 0;
    }
    initialiser_stats();
    //////////////////////
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Erreur de socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Erreur de liaison");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_JOUEURS) < 0)
    {
        perror("Erreur d'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions...\n");
    while (joueur_count < MAX_JOUEURS)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Erreur de connexion joueur");
            continue;
        }

        // Recevoir et stocker le nom du joueur
        char nom[250];
        char nombrepartie[250];
        memset(nom, 0, sizeof(nom));
        recv(new_socket, nom, sizeof(nom), 0);
        strncpy(joueurs_nom[joueur_count_nom], nom, 249);
        joueurs_nom[joueur_count_nom][249] = '\0';

        recv(new_socket, nombrepartie, sizeof(nombrepartie), 0);

        pthread_mutex_lock(&lock1);
        nombre_partie = atoi(nombrepartie);
        joueurs[joueur_count++] = new_socket;
        joueur_count_nom++;
        active_joueurs++; // Incrémenter le nombre de joueurs actifs
        pthread_mutex_unlock(&lock1);

        pthread_t thread;
        pthread_create(&thread, NULL, gestionClient, &new_socket);
        pthread_detach(thread);

        printf("Nouveau joueur connecté.\n");
    }

    // Boucle d'attente pour la déconnexion des joueurs restants
    while (1)
    {
        pthread_mutex_lock(&lock2);
        if (active_joueurs == 0)
        {
            pthread_mutex_unlock(&lock2);
            break; // Tous les joueurs se sont déconnectés, on peut fermer le serveur
        }
        pthread_mutex_unlock(&lock2);
        sleep(1); // Pause d'une seconde avant de vérifier à nouveau
    }
    actualise_stats();
    affichage_stats();
    generation_fichier_stats();
    system("../scripts/generate_report.sh"); // generate_report.sh déplacé dans scripts/
    system("../scripts/scriptClassement.sh"); // scriptClassement.sh déplacé dans scripts/
    printf("Tous les joueurs se sont déconnectés. Fermeture du serveur.\n");
    close(server_fd); // Fermer le serveur après la déconnexion de tous les joueurs
    return 0;
}
