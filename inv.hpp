#ifndef INV_HPP
#define INV_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "lib_projet_ncurses.hpp"
#include <iostream>
#include <cmath>

// mise a jour de l'inventaire lors du ramassage
void maj_inv(Jeu &jeu, int idConfig) {
    int idx;
    bool item_existant;

    // recherche de l'item dans l'inventaire
    idx = -1;
    for (int i = 0; i < jeu.nb_inventaire_items; i++) {
        if (jeu.inventaire_items[i].id_config == idConfig && idx == -1) {
            idx = i;
        }
    }

    // verification si l'item existe deja
    item_existant = (idx != -1);

    // mise a jour d'un item existant
    if (item_existant) {
        jeu.inventaire_items[idx].total++;
        jeu.inventaire_items[idx].restants++;
    }

    // creation d'une nouvelle entree
    if (!item_existant && jeu.nb_inventaire_items < TAILLE_ITEMS) {
        Inventaire_item &nouv = jeu.inventaire_items[jeu.nb_inventaire_items];
        nouv.id_config = idConfig;
        nouv.total = 1;
        nouv.utilises = 0;
        nouv.restants = 1;
        jeu.nb_inventaire_items++;
    }
}

// calcul de l'index haut pour l'affichage
int calcul_index_haut(int taille_liste, int index_selection, int max_visible) {
    int index_haut;

    if (taille_liste <= 0) {
        index_haut = 0;
    } else {
        // regle de scroll pour garder la selection visible
        index_haut = index_selection - max_visible + 1;

        if (index_haut < 0) {
            index_haut = 0;
        }

        if (index_haut > taille_liste - max_visible) {
            index_haut = max(0, taille_liste - max_visible);
        }
    }

    return index_haut;
}

// construction de la liste visible
void construire_liste_visible(const Jeu &jeu, int index_haut, int max_visible, Inventaire_item liste_visible[TAILLE_ITEMS], int &nb_lignes) {
    nb_lignes = 0;

    // copie des items visibles
    for (int i = 0; i < max_visible; i++) {
        int index_global = index_haut + i;

        if (index_global < jeu.nb_inventaire_items) {
            liste_visible[nb_lignes] = jeu.inventaire_items[index_global];
            nb_lignes++;
        }
    }
}

// affichage complet de l'inventaire
void afficher_inventaire(Jeu &jeu) {
    int largeur;
    int hauteur;
    int y_liste;
    int max_visible;
    int index_haut;

    Inventaire_item liste_visible[TAILLE_ITEMS];
    int nb_lignes_visibles;

    effacer_console();

    largeur = jeu.carte.largeur;
    hauteur = jeu.carte.hauteur;

    // calcul de la zone de liste
    y_liste = 2;
    max_visible = hauteur - 15;

    if (max_visible < 1) {
        max_visible = 1;
    }

    index_haut = calcul_index_haut(
        jeu.nb_inventaire_items,
        jeu.inv_selection_index,
        max_visible
    );

    jeu.inv_scroll_haut = index_haut;

    construire_liste_visible(
        jeu,
        index_haut,
        max_visible,
        liste_visible,
        nb_lignes_visibles
    );

    // affichage du titre
    int x_titre;

    string titre = "INVENTAIRE";
    x_titre = max(0, (largeur - (int)titre.size()) / 2);
    ecrire_string(titre, x_titre, 0);
    string sous_titre = "(T = fermer, ESPACE = utiliser)";
    x_titre = max(0, (largeur - (int)sous_titre.size()) / 2);
    ecrire_string(sous_titre, x_titre, 1);

    // affichage de l'en-tete
    string header = "| nom                 | utilises | restants |";
    ecrire_string(header, 1, y_liste);
    y_liste++;

    // affichage des lignes
    for (int i = 0; i < nb_lignes_visibles; i++) {
        Inventaire_item inv;
        Config_item cfg;
        bool cfg_trouvee;
        bool selectionnee;

        string nom;
        string ligne;

        inv = liste_visible[i];
        cfg_trouvee = false;

        // recherche de la configuration
        for (int j = 0; j < jeu.nb_cfg_items; j++) {
            if (jeu.cfg_items[j].id == inv.id_config) {
                cfg = jeu.cfg_items[j];
                cfg_trouvee = true;
            }
        }

        // affichage si configuration valide
        if (cfg_trouvee) {
            nom = underscore_espace(cfg.nom);

            if ((int)nom.size() > 18) {
                nom = nom.substr(0, 18);
            }

            selectionnee = (index_haut + i == jeu.inv_selection_index);

            ligne = selectionnee ? ">" : " ";
            ligne += nom;
            ligne += string(20 - (int)nom.size() - 1, ' ');
            ligne += "x" + to_string(inv.utilises);
            ligne += string(10 - (int)to_string(inv.utilises).size(), ' ');
            ligne += "x" + to_string(inv.restants);

            ecrire_string(ligne, 1, y_liste + i);
        }
    }

    // ligne vide
    y_liste = y_liste + max_visible;
    ecrire_string("", 1, y_liste);
    y_liste++;

    // affichage des details
    if (jeu.nb_inventaire_items > 0) {
        Inventaire_item inv_sel;
        Config_item cfg_sel;
        bool cfg_trouvee;

        inv_sel = jeu.inventaire_items[jeu.inv_selection_index];
        cfg_trouvee = false;

        // recherche de la configuration sélectionnée
        for (int j = 0; j < jeu.nb_cfg_items; j++) {
            if (jeu.cfg_items[j].id == inv_sel.id_config) {
                cfg_sel = jeu.cfg_items[j];
                cfg_trouvee = true;
            }
        }

        // affichage description et stats
        if (cfg_trouvee) {
            string detail = "details : " + underscore_espace(cfg_sel.description).substr(0, largeur - 2); // on crop pour éviter des écrasements de l'autre côté
            ecrire_string(detail, 1, y_liste);

            for (int s = 0; s < NB_STATS; s++) {
                string ligne_stat;

                ligne_stat = jeu.nom_stats[s] + " : ";
                if (cfg_sel.bonus[s] >= 0) {
                    ligne_stat += "+";
                }
                ligne_stat += to_string(cfg_sel.bonus[s]);

                ecrire_string(ligne_stat, 1, y_liste + 2 + s);
            }
        }
    }
}

#endif
