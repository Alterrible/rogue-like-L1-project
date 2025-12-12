#ifndef TRAITER_COMMANDE_HPP
#define TRAITER_COMMANDE_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include <iostream>
using namespace std;

// traitement principal
bool traiter_commande(char cmd, Jeu &jeu, bool& bvn) {
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

    // fermeture du modal et empêche de jouer tant qu'il est ouvert
    if (jeu.modal_active || bvn) {
        if (cmd == ' ') {
            if (bvn) {
                bvn = false;
            } else {
                jeu.modal_active = false;
                jeu.modal_txt = "";
            }
        }
        return false;
    }

    // ouvrir / fermer inventaire
    if (cmd == 't') {
        if (!jeu.inventaire_actif) {
            jeu.inventaire_actif = true;
            jeu.inv_selection_index = 0;
            jeu.inv_scroll_haut = 0;
        } else {
            jeu.inventaire_actif = false;
        }
        return false; 
    }

    // si inventaire ouvert → intercepter toutes les touches
    if (jeu.inventaire_actif) {

        // comme on veut afficher le nombre et pas l'inventaire alors on regroupe tout les items par id config
        int ids[TAILLE_ITEMS];
        int quantites[TAILLE_ITEMS];
        int nb_types = 0;

        for (int i = 0; i < jeu.joueur.nb_inventaire; i++) {
            int idConf = jeu.joueur.inventaire[i];
            int idx = -1;

            for (int t = 0; t < nb_types; t++)
                if (ids[t] == idConf) { idx = t; break; }

            if (idx == -1) {
                ids[nb_types] = idConf;
                quantites[nb_types] = 1;
                nb_types++;
            } else {
                quantites[idx]++;
            }
        }

        // sélection dans la liste
        if (cmd == 'z' && jeu.inv_selection_index > 0) {
            jeu.inv_selection_index--;
        }

        else if (cmd == 's' && jeu.inv_selection_index < nb_types - 1) {
            jeu.inv_selection_index++;
        }

        // utiliser l'item sélectionné
        else if (cmd == ' ') {
            if (nb_types > 0) {
                int idConf_sel = ids[jeu.inv_selection_index];
                utiliser_item(jeu, idConf_sel);
            }
        }

        return false;
    }


    // déplacement du joueur
    if (commande_deplacement) {

        // vérifie si la case est dans la carte et valide
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

    if (commande_deplacement || commande_interaction) return true;
    else return false;
}

#endif
