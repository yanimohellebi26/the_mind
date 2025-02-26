import matplotlib.pyplot as plt
import pandas as pd

# Charger les données de simulation
df_simulation = pd.read_csv('donnees_simulation.csv')

# Analyser la distribution des temps d'attente
plt.hist(df_simulation['temps_attente'], bins=50)
plt.xlabel('Temps d\'attente')
plt.ylabel('Fréquence')
plt.title('Distribution des temps d\'attente')
plt.show()

# Vérifier les premières lignes des données
print(df_simulation.head())
