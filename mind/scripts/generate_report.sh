#!/bin/bash

# Répertoire des données
DATA_DIR="../data"

# Créer le répertoire des données s'il n'existe pas
mkdir -p "$DATA_DIR"

# Générer les graphiques avec gnuplot
gnuplot -e "
    set terminal png size 800,600;
    set output '$DATA_DIR/manches_reussies.png';
    set title 'Nombre de Manches Réussies par Partie';
    set xlabel 'Partie';
    set ylabel 'Manches Réussies';
    set yrange [0:*];
    set style data histograms;
    set style fill solid;
    plot '$DATA_DIR/stats.txt' using 2:xtic(1) title 'Manches Réussies';
"

gnuplot -e "
    set terminal png size 800,600;
    set output '$DATA_DIR/valeur_echec.png';
    set title 'Valeur ayant causé un Échec par Partie';
    set xlabel 'Partie';
    set ylabel 'Valeur de l’Échec';
    set xrange [0:*];
    set yrange [0:110];
    set style data histograms;
    set style fill solid;
    plot '$DATA_DIR/stats.txt' using 3:xtic(1) title 'Valeur de l’Échec';
"

gnuplot -e "
    set terminal png size 800,600;
    set output '$DATA_DIR/temps_reaction.png';
    set title 'Temps de Réaction Moyen par Partie';
    set xlabel 'Partie';
    set ylabel 'Temps de Réaction (secondes)';
    set yrange [0:*];
    set xrange [0:*];
    plot '$DATA_DIR/stats.txt' using 1:4 with linespoints title 'Temps de Réaction';
"

# Générer le tableau des statistiques en LaTeX avec tabular
awk 'BEGIN {
    FS = "\t";
    print "\\begin{table}[ht]";
    print "\\centering";
    print "\\begin{tabular}{|l|l|l|l|}";
    print "\\hline";
    print "Partie & Manches Reussies & Valeur de l Echec & Temps de Reaction (secondes) \\\\ \\hline";
}
{
    print $1 " & " $2 " & " $3 " & " $4 " \\\\ \\hline";
}
END {
    print "\\end{tabular}";
    print "\\caption{Tableau des statistiques des parties}";
    print "\\end{table}";
}' "$DATA_DIR/stats.txt" > "$DATA_DIR/stats.tex"

# Inclure le tableau dans le rapport LaTeX
echo "\section*{Résumé des Statistiques}" >> "$DATA_DIR/rapport.tex"
echo "\begin{itemize}" >> "$DATA_DIR/rapport.tex"
echo "\item Total des Manches Réussies : $total_manches" >> "$DATA_DIR/rapport.tex"
echo "\item Nombre total de Valeurs Loupées : $total_valeurs" >> "$DATA_DIR/rapport.tex"
echo "\item Meilleure Partie : Partie $meilleure_partie avec un temps de réaction de $meilleur_temps secondes" >> "$DATA_DIR/rapport.tex"
echo "\item Pire Partie : Partie $pire_partie avec un temps de réaction de $pire_temps secondes" >> "$DATA_DIR/rapport.tex"
echo "\end{itemize}" >> "$DATA_DIR/rapport.tex"

echo "\section*{Tableau des Statistiques}" >> "$DATA_DIR/rapport.tex"
echo "\input{$DATA_DIR/stats.tex}" >> "$DATA_DIR/rapport.tex"

# Compiler le rapport LaTeX en PDF
pdflatex -output-directory="$DATA_DIR" "$DATA_DIR/rapport.tex"
