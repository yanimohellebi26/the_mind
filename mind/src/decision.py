import joblib
import pandas as pd
import numpy as np

# Charger le modèle enregistré
model = joblib.load('../scripts/modele_entrainement.joblib')

# Fonction de prédiction
def predict(derniere_carte, plus_petite_carte, num_players):
    # Créer un DataFrame avec les noms de colonnes
    # Nouvelles données
    new_data = np.array([[plus_petite_carte,derniere_carte,num_players]])
    columns_expected = ['carte_la_plus_petite', 'derniere_carte', 'nombre_joueurs']
    new_data_df = pd.DataFrame(new_data, columns=columns_expected)

    # Vérification des colonnes
    assert all(col in new_data_df.columns for col in columns_expected), "Les colonnes de la nouvelle donnée ne correspondent pas au modèle"

    # Prédiction
    predicted_wait_time = model.predict(new_data_df)
    return predicted_wait_time[0]  # Retourner le temps d'attente prédit

