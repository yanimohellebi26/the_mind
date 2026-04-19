# The Mind — Jeu de Cartes en Réseau avec IA

Implémentation en C et Python du jeu de cartes coopératif **The Mind**, jouable en réseau (client/serveur) avec un joueur robot piloté par IA (ML).

## Fonctionnalités

- Architecture **client/serveur** en C (sockets TCP)
- Joueur humain et **joueur robot** (décision ML)
- Modèle d'apprentissage automatique entraîné (`modele_entrainement.joblib`)
- Simulation de parties et collecte de statistiques
- Rapport LaTeX complet (`rapport.pdf`, `stats.pdf`)

## Structure

```
├── serveur.c / serveur.h     # Serveur de jeu (C)
├── client.c                  # Client joueur (C)
├── joueur_humain.c/.h        # Logique joueur humain
├── joueur_robot.c/.h         # Robot (décision via Python)
├── interface.c/.h            # Interface terminal
├── decision.py               # Prise de décision ML
├── simulation.py             # Simulation de parties
├── training.py               # Entraînement du modèle
├── analyse.py                # Analyse des résultats
├── makefile                  # Build
├── rapport.tex / rapport.pdf # Rapport
└── stats.tex / stats.pdf     # Statistiques
```

## Compilation & Lancement

```bash
# Compiler
make

# Lancer le serveur
./serveur

# Lancer un client
./joueur_humain
./joueur_robot

# Simuler des parties
python simulation.py
```

## Entraînement du modèle

```bash
python training.py
```

## Technologies

`C` · `Python` · `scikit-learn` · Sockets TCP · LaTeX · IA / ML