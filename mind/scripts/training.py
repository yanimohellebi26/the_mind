import pandas as pd
import xgboost as xgb
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error

# Charger les données de simulation
df_simulation = pd.read_csv('donnees_simulation.csv')

# Préparer les features et la cible
X = df_simulation[['carte_la_plus_petite', 'derniere_carte', 'nombre_joueurs']]
y = df_simulation['temps_attente']

# Séparer les données en train et test
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Entraîner le modèle avec XGBoost
modele = xgb.XGBRegressor(
    n_estimators=100,
    max_depth=6,
    learning_rate=0.1,
    random_state=42
)
modele.fit(X_train, y_train)

# Prédire sur l'ensemble de test
y_pred = modele.predict(X_test)

# Évaluer les performances
mse = mean_squared_error(y_test, y_pred)
print(f"Erreur quadratique moyenne (MSE) : {mse}")

# Sauvegarder le modèle
import joblib
joblib.dump(modele, 'modele_entrainement.joblib')
print("Modèle sauvegardé sous 'modele_entrainement.joblib'.")
