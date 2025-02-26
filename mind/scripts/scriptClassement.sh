#!/bin/bash

# Répertoire des données
DATA_DIR="../data"

# Vérifier si le fichier classement.txt existe
if [ ! -f "$DATA_DIR/classement.txt" ]; then
    echo "Le fichier classement.txt n'existe pas !"
    exit 1
fi

# Trier en fonction de la deuxième colonne (maxmanchereussi) en ordre décroissant
# et rediriger la sortie vers le même fichier
sort -t, -k2,2nr "$DATA_DIR/classement.txt" -o "$DATA_DIR/classement.txt"
