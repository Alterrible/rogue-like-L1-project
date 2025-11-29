#ifndef INIT_HPP
#define INIT_HPP

using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include "enregistrement.hpp"


// découper lignes en nombre de mots
int decouper(const string& ligne, string mots[], int max_mots) {
    int nb = 0;
    string mot = "";

    for (int i = 0; i <= ligne.size(); i++) {
        if (i == ligne.size() || ligne[i] == ' ') {
            if (mot != "" && nb < max_mots) {
                mots[nb] = mot;
                nb++;
                mot.clear();
            }
        } else {
            mot += ligne[i];
        }
    }
    return nb;
}


bool charger_configuration(const string& fichier, Jeu& jeu) {
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    // remise à zéro
    jeu.nb_cfg_items = 0;
    jeu.nb_cfg_monstres = 0;
    jeu.nb_cfg_portes = 0;

    jeu.cfgConditions.nbContraintes = 0;
    jeu.cfgConditions.victoire.nb = 0;
    jeu.cfgConditions.defaite.nb = 0;

    int section = 0;
    string ligne;

    while (getline(flux, ligne)) {
        bool skip = false; // évite problème de recherche dans ligne vide ou changement de section

        // nouvelle section -> [
        if (ligne.size() > 0 && ligne[0] == '[') {
            section++;
            skip = true;
        }

        // mots découpés
        string mots[20];
        int nb = decouper(ligne, mots, 20);
        if (nb == 0) skip = true;

        // items
        if (!skip && section == 1) {
            Config_item &it = jeu.cfg_items[jeu.nb_cfg_items];

            it.id = stoi(mots[0]);
            it.symbole = mots[1][0];
            it.nom = mots[2];
            it.description = mots[3];

            for (int i = 0; i < NB_STATS; i++) {
                it.bonus[i] = stoi(mots[4 + i]);
            }

            jeu.nb_cfg_items++;
        }

        // monstres
        else if (!skip && section == 2) {
            Config_monstre &m = jeu.cfg_monstres[jeu.nb_cfg_monstres];

            m.id = stoi(mots[0]);
            m.symbole = mots[1][0];
            m.nom = mots[2];
            m.description = mots[3];

            for (int i = 0; i < NB_STATS; i++) {
                m.stats_base[i] = stoi(mots[4 + i]);
            }

            m.typeIA = stoi(mots[10]);
            m.id_item_contrainte = stoi(mots[11]);

            for (int i = 0; i < NB_STATS; i++) {
                m.contrainte_stats[i] = stoi(mots[12 + i]);
            }

            m.inventaire_ids.taille = 0;

            jeu.nb_cfg_monstres++;
        }

        // porte
        else if (!skip && section == 3) {
            Config_porte &p = jeu.cfg_portes[jeu.nb_cfg_portes];

            p.id = stoi(mots[0]);
            p.symbole = mots[1][0];
            p.id_item_contrainte = stoi(mots[2]);

            for (int i = 0; i < NB_STATS; i++) {
                if (!mots[3 + i].empty())
                    p.contrainte_stats[i] = stoi(mots[3 + i]);
                else
                    p.contrainte_stats[i] = 0;   // par défaut
            }

            jeu.nb_cfg_portes++;
        }

        // joueur
        else if (!skip && section == 4) {

            if (mots[0] == "nom") {
                jeu.cfg_joueur.nom = mots[1];
            }

            else if (mots[0] == "description") {
                jeu.cfg_joueur.description = mots[1];
            }

            else if (mots[0] == "stats") {
                for (int i = 0; i < NB_STATS; i++) {
                    jeu.cfg_joueur.stats[i] = stoi(mots[1 + i]);
                }
            }

            jeu.cfg_joueur.symbole = '@';
            jeu.cfg_joueur.inventaire_ids.taille = 0;
            jeu.cfg_joueur.actif = true;
        }

        // contraintes
        else if (!skip && section == 5) {
            Contrainte &c = jeu.cfgConditions.contraintes[jeu.cfgConditions.nbContraintes];

            c.id = stoi(mots[0]);
            c.description = mots[1];

            for (int i = 0; i < NB_STATS; i++) {
                c.stats_min[i] = stoi(mots[2 + i]);
            }

            jeu.cfgConditions.nbContraintes++;
        }

        // condition de victoire
        else if (!skip && section == 7) {
            for (int i = 0; i < nb; i++) {
                jeu.cfgConditions.victoire.ids[jeu.cfgConditions.victoire.nb++] = stoi(mots[i]);
            }
        }

        // condition de défaite
        else if (!skip && section == 8) {
            for (int i = 0; i < nb; i++) {
                jeu.cfgConditions.defaite.ids[jeu.cfgConditions.defaite.nb++] = stoi(mots[i]);
            }
        }
    }

    flux.close();
    return true;
}

bool charger_carte(const string& fichier, Jeu& jeu) {
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    string ligne;
    int y = 0;

    // remis à zéro
    jeu.nb_items = 0;
    jeu.nb_monstres = 0;

    while (getline(flux, ligne)) {

        for (int x = 0; x < ligne.size(); x++) {
            char c = ligne[x];
            jeu.carte.cases[y][x] = c;

            // joueur
            if (c == '@') {
                jeu.joueur.x = x;
                jeu.joueur.y = y;
            }

            // items : a-z
            if (c >= 'a' && c <= 'z') {
                for (int i = 0; i < jeu.nb_cfg_items; i++) {
                    if (jeu.cfg_items[i].symbole == c) {

                        Items &It = jeu.items[jeu.nb_items];
                        It.x = x;
                        It.y = y;
                        It.idConfig = jeu.cfg_items[i].id;

                        jeu.nb_items++;
                    }
                }
            }

            // monstres : A-Z
            if (c >= 'A' && c <= 'Z') {
                for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
                    if (jeu.cfg_monstres[i].symbole == c) {

                        Monstre &M = jeu.monstres[jeu.nb_monstres];
                        M.x = x;
                        M.y = y;
                        M.idConfig = jeu.cfg_monstres[i].id;

                        for (int k = 0; k < NB_STATS; k++)
                            M.stats[k] = jeu.cfg_monstres[i].stats_base[k];

                        M.actif = true;

                        jeu.nb_monstres++;
                    }
                }
            }
        }
        y++;
    }

    jeu.carte.hauteur = y;
    jeu.carte.largeur = TAILLE_MAP_X;

    flux.close();
    return true;
}

#endif
// NOTE : stoi c'est pour transformer string en int, .size() c'est pour la taille d'une liste
