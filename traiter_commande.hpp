#ifndef TRAITER_COMMANDE_HPP
#define TRAITER_COMMANDE_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include <iostream>
using namespace std;

// traitement principal
void traiter_commande(char cmd, Jeu &jeu) {
    int nouveauX = jeu.joueur.x;
    int nouveauY = jeu.joueur.y;

    bool commande_deplacement = false;
    bool commande_interaction = false;

    // détection entrée
    if (cmd == 'z') { nouveauY--; commande_deplacement = true; }
    else if (cmd == 's') { nouveauY++; commande_deplacement = true; }
    else if (cmd == 'q') { nouveauX--; commande_deplacement = true; }
    else if (cmd == 'd') { nouveauX++; commande_deplacement = true; }

    else if (cmd == 'i') { nouveauY--; commande_interaction = true; }
    else if (cmd == 'k') { nouveauY++; commande_interaction = true; }
    else if (cmd == 'j') { nouveauX--; commande_interaction = true; }
    else if (cmd == 'l') { nouveauX++; commande_interaction = true; }

    // déplacement du joueur
    if (commande_deplacement) {

        // vérifie si la case est dans la carte et validité
        bool case_valide =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        if (case_valide) {
            char c = jeu.carte.cases[nouveauY][nouveauX];
            bool marchable = (c != '#' && !(c >= 'A' && c <= 'Z'));
            bool est_porte_valide = porte_valide(jeu, c);

            if (marchable && est_porte_valide) {
                jeu.joueur.x = nouveauX;
                jeu.joueur.y = nouveauY;
            }
        }
    }

    // interaction i k j l
    if (commande_interaction) {

        bool case_dans_carte =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        bool interaction_possible = case_dans_carte;

        if (interaction_possible) {

            int id_monstre = trouver_monstre(jeu, nouveauX, nouveauY);
            int id_item = trouver_item(jeu, nouveauX, nouveauY);

            // → monstre ? || → item ?
            if (id_monstre != -1) attaque(jeu, id_monstre);
            else if (id_item != -1) ramasser(jeu, id_item);
        }
    }
}

#endif
