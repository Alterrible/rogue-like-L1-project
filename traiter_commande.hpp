#ifndef TRAITER_COMMANDE_HPP
#define TRAITER_COMMANDE_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "combat.hpp"
#include <iostream>
using namespace std;

// traitement principal
bool traiter_commande(char cmd, Jeu &jeu, bool& bvn) {
    int nouveauX = jeu.joueur.x;
    int nouveauY = jeu.joueur.y;

    bool commande_deplacement = false;
    bool commande_interaction = false;

    // detection entree
    if (cmd == 'z') { nouveauY--; commande_deplacement = true; }
    else if (cmd == 's') { nouveauY++; commande_deplacement = true; }
    else if (cmd == 'q') { nouveauX--; commande_deplacement = true; }
    else if (cmd == 'd') { nouveauX++; commande_deplacement = true; }

    else if (cmd == 'i') { nouveauY--; commande_interaction = true; }
    else if (cmd == 'k') { nouveauY++; commande_interaction = true; }
    else if (cmd == 'j') { nouveauX--; commande_interaction = true; }
    else if (cmd == 'l') { nouveauX++; commande_interaction = true; }

    // fermeture modal et blocage
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

    // combat actif
    if (jeu.combat.actif) {

        afficher_combat(jeu);

        if (jeu.combat.tour_joueur) {

            // choix stat ciblee via touches '0'..'5'
            if (cmd >= '0' && cmd < ('0' + NB_STATS)) {
                tour_joueur_combat(jeu, cmd - '0');
            }

            // acces inventaire en combat
            if (cmd == 't') {
                jeu.inventaire_actif = !jeu.inventaire_actif;

                if (jeu.inventaire_actif) {
                    jeu.inv_selection_index = 0;
                    jeu.inv_scroll_haut = 0;
                }
            }

            // inventaire actif en combat
            if (jeu.inventaire_actif) {

                int nb = jeu.nb_inventaire_items;

                if (cmd == 'z') {
                    jeu.inv_selection_index--;
                }
                else if (cmd == 's') {
                    jeu.inv_selection_index++;
                }

                if (jeu.inv_selection_index < 0)
                    jeu.inv_selection_index = 0;

                if (jeu.inv_selection_index >= nb && nb > 0)
                    jeu.inv_selection_index = nb - 1;

                if (cmd == ' ' && nb > 0) {
                    Inventaire_item& inv = jeu.inventaire_items[jeu.inv_selection_index];

                    if (inv.restants > 0) {
                        utiliser_item(jeu, inv.id_config);
                    }
                }
            }
        }
        else {
            tour_monstre_combat(jeu);
        }

        verifier_fin_combat(jeu);
        return false;
    }

    // ouvrir/fermer inventaire
    if (cmd == 't') {
        jeu.inventaire_actif = !jeu.inventaire_actif;

        if (jeu.inventaire_actif) {
            jeu.inv_selection_index = 0;
            jeu.inv_scroll_haut = 0;
        }

        return false;
    }

    // inventaire actif
    if (jeu.inventaire_actif) {

        int nb = jeu.nb_inventaire_items;

        // navigation
        if (cmd == 'z') {
            jeu.inv_selection_index--;
        }
        else if (cmd == 's') {
            jeu.inv_selection_index++;
        }

        // selection
        if (jeu.inv_selection_index < 0)
            jeu.inv_selection_index = 0;

        if (jeu.inv_selection_index >= nb && nb > 0)
            jeu.inv_selection_index = nb - 1;

        // utiliser item
        if (cmd == ' ' && nb > 0) {
            Inventaire_item& inv = jeu.inventaire_items[jeu.inv_selection_index];

            if (inv.restants > 0) {
                utiliser_item(jeu, inv.id_config);
            }
        }

        return false;
    }

    // deplacement joueur
    if (commande_deplacement) {

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

            // monstre ou item
            if (id_monstre != -1) {
                // entree en combat
                jeu.combat.actif = true;
                jeu.combat.id_monstre = id_monstre;
                jeu.combat.tour_joueur = true;
            }
            else if (id_item != -1) {
                ramasser(jeu, id_item);
            }
        }
    }

    if (commande_deplacement || commande_interaction) return true;
    else return false;
}

#endif
