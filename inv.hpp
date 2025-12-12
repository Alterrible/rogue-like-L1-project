#ifndef INV_HPP
#define INV_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "lib_projet_ncurses.hpp"
#include <iostream>
#include <cmath>

void maj_inv(Jeu& jeu, int idConfig) {
    // recherche de l'item dans l'inventaire
    int idx = -1;
    for (int i = 0; i < jeu.nb_inventaire_items; i++) {
        if (jeu.inventaire_items[i].id_config == idConfig && idx == -1) {
            idx = i;
        }
    }

    // verification si l'item existe deja
    bool item_existant = (idx != -1);

    // mise a jour d'un item existant
    if (item_existant) {
        jeu.inventaire_items[idx].total++;
        jeu.inventaire_items[idx].restants++;
    }

    // verification de la place disponible
    bool place_disponible = (!item_existant && jeu.nb_inventaire_items < TAILLE_ITEMS);

    // creation d'une nouvelle entree d'inventaire
    if (place_disponible) {
        Inventaire_item& nouv = jeu.inventaire_items[jeu.nb_inventaire_items];
        nouv.id_config = idConfig;
        nouv.total = 1;
        nouv.utilises = 0;
        nouv.restants = 1;
        jeu.nb_inventaire_items++;
    }
}

void afficher_inventaire(Jeu& jeu) {
    // nettoyage de l'ecran
    effacer_console();

    int largeur = jeu.carte.largeur;
    int hauteur = jeu.carte.hauteur;

    // affichage du titre
    string titre = "INVENTAIRE (T = fermer, ESPACE = utiliser)";
    int x_titre = max(0, (largeur - (int)titre.size()) / 2);
    ecrire_string(titre, x_titre, 0);

    // verification de la presence d'items
    bool inventaire_vide = (jeu.nb_inventaire_items == 0);

    if (inventaire_vide) {
        ecrire_string("Inventaire vide.", 2, hauteur / 2);
    }

    // calcul de la zone de liste
    int y_liste = 2;
    int hauteur_liste = hauteur - 7;
    if (hauteur_liste < 1) hauteur_liste = 1;
    int max_visible = hauteur_liste;

    // clamp de la selection
    if (!inventaire_vide) {
        if (jeu.inv_selection_index < 0)
            jeu.inv_selection_index = 0;
        if (jeu.inv_selection_index >= jeu.nb_inventaire_items)
            jeu.inv_selection_index = jeu.nb_inventaire_items - 1;
    }

    // gestion du scroll automatique
    if (!inventaire_vide) {
        if (jeu.inv_scroll_haut > jeu.inv_selection_index)
            jeu.inv_scroll_haut = jeu.inv_selection_index;

        if (jeu.inv_selection_index >= jeu.inv_scroll_haut + max_visible)
            jeu.inv_scroll_haut = jeu.inv_selection_index - max_visible + 1;

        if (jeu.inv_scroll_haut < 0)
            jeu.inv_scroll_haut = 0;

        if (jeu.inv_scroll_haut > jeu.nb_inventaire_items - max_visible)
            jeu.inv_scroll_haut = max(0, jeu.nb_inventaire_items - max_visible);
    }

    // affichage de l'en-tete
    const int COL_NOM = 20;
    const int COL_USE = 12;
    const int COL_RES = 12;

    if (!inventaire_vide) {
        string header =
            "| " + string("Nom", 0, COL_NOM - 1) +
            string(COL_NOM - 3, ' ') +
            "| Utilisés   | Restants   |";

        ecrire_string(header, 1, y_liste++);
    }

    // affichage des lignes visibles
    if (!inventaire_vide) {
        for (int ligne = 0; ligne < max_visible; ligne++) {
            int idx = jeu.inv_scroll_haut + ligne;
            bool idx_valide = (idx < jeu.nb_inventaire_items);

            if (idx_valide) {
                Inventaire_item& inv = jeu.inventaire_items[idx];

                // recherche de la configuration de l'item
                Config_item cfg;
                bool cfg_trouvee = false;
                for (int i = 0; i < jeu.nb_cfg_items; i++) {
                    if (jeu.cfg_items[i].id == inv.id_config && !cfg_trouvee) {
                        cfg = jeu.cfg_items[i];
                        cfg_trouvee = true;
                    }
                }

                // affichage de la ligne si la config existe
                if (cfg_trouvee) {
                    string nom = underscore_espace(cfg.nom);
                    if ((int)nom.size() > COL_NOM - 2)
                        nom = nom.substr(0, COL_NOM - 2);

                    bool sel = (idx == jeu.inv_selection_index);

                    string ligne_str;
                    ligne_str += sel ? ">" : " ";
                    ligne_str += nom;
                    ligne_str += string(COL_NOM - nom.size() - 2, ' ');
                    ligne_str += "x" + to_string(inv.utilises);
                    ligne_str += string(COL_USE - to_string(inv.utilises).size() - 2, ' ');
                    ligne_str += "x" + to_string(inv.restants);
                    ligne_str += string(COL_RES - to_string(inv.restants).size() - 2, ' ');


                    ecrire_string(ligne_str, 1, y_liste + ligne);
                }
            }
        }
    }

    // recuperation de l'item selectionne
    bool selection_valide = (!inventaire_vide &&
        jeu.inv_selection_index >= 0 &&
        jeu.inv_selection_index < jeu.nb_inventaire_items);

    Config_item cfg_sel;
    bool cfg_sel_trouvee = false;

    if (selection_valide) {
        Inventaire_item& inv_sel = jeu.inventaire_items[jeu.inv_selection_index];

        for (int i = 0; i < jeu.nb_cfg_items; i++) {
            if (jeu.cfg_items[i].id == inv_sel.id_config && !cfg_sel_trouvee) {
                cfg_sel = jeu.cfg_items[i];
                cfg_sel_trouvee = true;
            }
        }
    }

    // affichage des details
    int y_details = y_liste + max_visible + 1;
    bool details_affichables = (cfg_sel_trouvee && y_details < hauteur);

    if (details_affichables) {
        string nom = underscore_espace(cfg_sel.nom);
        string desc = underscore_espace(cfg_sel.description);

        ecrire_string("Détails : " + nom, 1, y_details);
        ecrire_string(desc.substr(0, largeur - 2), 1, y_details + 1);

        // affichage des stats
        int y_stats = y_details + 3;
        for (int s = 0; s < NB_STATS && y_stats + s < hauteur; s++) {
            int bonus = cfg_sel.bonus[s];
            string stat = jeu.nom_stats[s] + " : ";
            if (bonus >= 0) stat += "+";
            stat += to_string(bonus);
            ecrire_string(stat, 1, y_stats + s);
        }
    }
}


#endif