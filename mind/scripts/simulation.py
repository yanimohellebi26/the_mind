import random
import pandas as pd
import numpy as np

# Nombre de parties à simuler
N_PARTIES = 10000
MAX_CARTES = 100

# Fonction pour simuler une partie
def simuler_partie():
    # Initialisation de la partie
    joueurs = random.randint(2, 5)  # Nombre de joueurs aléatoire entre 2 et 5
    cartes = list(range(1, MAX_CARTES + 1))  # Cartes de 1 à 100
    random.shuffle(cartes)  # Mélanger les cartes
    main_robot = cartes[:10]  # Le robot reçoit les 10 premières cartes
    derniere_carte = -1  # Pas de carte jouée initialement
    temps_attente = 0  # Temps d'attente initial à 0
    
    data = []
    
    # Simuler le déroulement de la partie
    for i in range(10):  # La partie dure 10 tours
        plus_petite_carte = min([card for card in main_robot if card > derniere_carte], default=101)
        
        if plus_petite_carte == 101:
            break  # Si aucune carte n'est jouable, fin de la partie
        
        # Calculer le temps d'attente (de manière aléatoire pour la simulation)
        temps_attente = random.uniform(1, 15)  # Entre 1 et 5 secondes
        
        # Ajouter les données de cette action à notre dataframe
        data.append({
            'carte_la_plus_petite': plus_petite_carte,
            'derniere_carte': derniere_carte,
            'nombre_joueurs': joueurs,
            'temps_attente': temps_attente
        })
        
        # Mettre à jour la dernière carte jouée
        derniere_carte = plus_petite_carte
        
        # Retirer la carte jouée de la main du robot
        main_robot.remove(plus_petite_carte)
    
    return data

# Simuler N_PARTIES
toutes_les_donnees = []
for _ in range(N_PARTIES):
    toutes_les_donnees.extend(simuler_partie())

# Sauvegarder les données dans un fichier CSV
df_simulation = pd.DataFrame(toutes_les_donnees)
df_simulation.to_csv('donnees_simulation.csv', index=False)
print("Simulation terminée et sauvegardée dans 'donnees_simulation.csv'.")