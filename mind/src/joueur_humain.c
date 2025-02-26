#include "../include/joueur_humain.h" // Fichier spécifique à joueur_humain   
// Définition des variables globales
int sockfd;
pthread_t thread_envoi, thread_reception;
int autorisation_envoyer = 0;
int manche = 0;
int valide = 1;
int mescartes[MAX_CARTES];

void *threadEnvoi(void *arg)
{
    char carte[256];
    while (1)
    {
        int cartes_envoyees = 1;

        while (autorisation_envoyer)
        {
            int isVal = 0;
            while (!isVal)
            {
                scanf("%s", carte);
                int i = 0;
                while (i < manche && !isVal)
                {
                    if (atoi(carte) == mescartes[i])
                        isVal = 1;
                    i++;
                }
            }
            send(sockfd, carte, strlen(carte), 0);
            cartes_envoyees++;
            if (cartes_envoyees > manche)
                autorisation_envoyer = 0; // Réinitialiser jusqu'à nouveau signal
        }
        if (!valide)
            break;
    }
    return NULL;
}

void *threadReception(void *arg)
{
    char message[256];
    int len;
    while (1)
    {
        memset(message, 0, sizeof(message));

        len = recv(sockfd, message, sizeof(message), 0);

        if (len > 0)
        {
            message[len] = '\0';
            if (strcmp(message, "SIGNAL") == 0)
            {
                recv(sockfd, &manche, sizeof(manche), 0);
                for (int i = 0; i < manche; i++)
                {

                    recv(sockfd, &mescartes[i], sizeof(mescartes[i]), 0);
                }
                for (int i = 0; i < manche; i++)
                {

                    printf("carte%d : %d  ", i + 1, mescartes[i]);
                }
                printf("\n");
                autorisation_envoyer = 1; // Activer l'envoi
                // printf("Signal reçu : vous pouvez envoyer %d message. \n > :",manche);
            }
            else if (strcmp(message, "GAGNE") == 0)
            {
                printf("Manche gagnée\n");
            }
            else if (strcmp(message, "DEBUT") == 0)
            {
                printf("DEBUT DE PARTIE\n");
            }
            else if (strcmp(message, "FIN") == 0)
            {
                printf("FIN DE PARTIE\n");
            }
            else if (strcmp(message, "PERDU") == 0)
            {
                printf("Manche Perdue\n");
            }
            else if (strcmp(message, "FINS") == 0)
            {
                printf("C'est fini\n");
                valide = 0; 
                break;
            }
            else
            {
                char nom[250];

                memset(nom, 0, sizeof(nom));
                recv(sockfd, nom, sizeof(nom), 0);
                printf("%s a joué: %s \n", nom, message);
            }
        }
        else
        {
            perror("j'ai rien");
            break;
        }
    }
}
int main()
{
    struct sockaddr_in serv_addr;
    //////////////////////

    srand(time(NULL));
    char nom[250];
    printf("Votre nom : ");
    scanf("%s", &nom);
    char partie[250];
    printf("nombre de partie : ");
    scanf("%s", &partie);
    /////////////////////
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Adresse invalide/Non supportée");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connexion échouée");
        exit(EXIT_FAILURE);
    }

    send(sockfd, nom, strlen(nom), 0);sleep(1);
    send(sockfd, "2", strlen("2"), 0);
    //send(sockfd, partie, strlen(partie), 0);
    
    pthread_create(&thread_envoi, NULL, threadEnvoi, NULL);
    pthread_create(&thread_reception, NULL, threadReception, NULL);

    pthread_join(thread_envoi, NULL);
    pthread_join(thread_reception, NULL);

    close(sockfd);
    return 0;
}