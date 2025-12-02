#ifndef INIT_HPP
#define INIT_HPP

using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include "lib_projet_ncurses.hpp"
#include "enregistrement.hpp"


// découper lignes en nombre de mots et en liste mots
int decouper(const string& ligne, string mots[], int max_mots) {
    int nb = 0;
    string mot = "";

    for (int i = 0; i <= ligne.size(); i++) {
        if (i == ligne.size() || ligne[i] == ' ') {
            if (mot != "" && (int)mot[0] != 13 && nb < max_mots) {
                mots[nb] = mot;
                nb++;
                mot = "";
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
        string mots[50];
        int nb = decouper(ligne, mots, 50);
        if (nb == 0) skip = true;
        if (mots[0][0] == '|') skip = true;

        // items
        if (!skip && section == 1) {
            Config_item &it = jeu.cfg_items[jeu.nb_cfg_items];

            // cout << "section = 1, assigne = it.id, valeur = " << mots[0] << endl;
            it.id = stoi(mots[0]);
            it.symbole = mots[1][0];
            it.nom = mots[2];
            it.description = mots[3];

            for (int i = 0; i < NB_STATS; i++) {
                // cout << "section = 1, assigne = it.bonus[" << i << "], valeur = " << mots[4 + i] << endl;
                it.bonus[i] = stoi(mots[4 + i]);
            }

            jeu.nb_cfg_items++;
        }

        // monstres
        else if (!skip && section == 2) {
            Config_monstre &m = jeu.cfg_monstres[jeu.nb_cfg_monstres];

            // cout << "section = 2, assigne = m.id, valeur = " << mots[0] << endl;
            m.id = stoi(mots[0]);
            m.symbole = mots[1][0];
            m.nom = mots[2];
            m.description = mots[3];

            // cout << "section = 2, assigne = m.typeIA, valeur = " << mots[16] << endl;
            m.typeIA = stoi(mots[22]);

            for (int i = 0; i < NB_STATS; i++) {
                // cout << "section = 2, assigne = m.stats_base[" << i << "], valeur = " << mots[4 + i] << endl;
                m.stats_base[i] = stoi(mots[4 + i]);
            }

            for (int i = 0; i < NB_STATS; i++) {
                // cout << "section = 2, assigne = m.stats_afflige[" << i << "], valeur = " << mots[10 + i] << endl;
                m.stats_afflige[i] = stoi(mots[10 + i]);
            }

            for (int i = 0; i < NB_STATS; i++) {
                // cout << "section = 2, assigne = m.stats_prit[" << i << "], valeur = " << mots[16 + i] << endl;
                bool prend = false;
                if(stoi(mots[16 + i]) > 0) prend = true;
                m.stats_prit[i] = prend;
            }

            jeu.nb_cfg_monstres++;
        }

        // porte
        else if (!skip && section == 3) {
            Config_porte &p = jeu.cfg_portes[jeu.nb_cfg_portes];

            // cout << "section = 3, assigne = p.id, valeur = " << mots[0] << endl;
            p.id = stoi(mots[0]);
            p.symbole = mots[1][0];

            // cout << "section = 3, assigne = p.id_contrainte, valeur = " << mots[2] << endl;
            p.id_contrainte = stoi(mots[2]);

            jeu.nb_cfg_portes++;
        }

        // joueur
        else if (!skip && section == 4) {
            if (mots[0] == "symbole") {
                jeu.cfg_joueur.symbole = mots[1][0];
            }

            else if (mots[0] == "nom") {
                jeu.cfg_joueur.nom = mots[1];
            }

            else if (mots[0] == "description") {
                jeu.cfg_joueur.description = mots[1];
            }

            else if (mots[0] == "stats") {
                for (int i = 0; i < NB_STATS; i++) {
                    // cout << "section = 4, assigne = joueur.stats[" << i << "], valeur = " << mots[1 + i] << endl;
                    jeu.cfg_joueur.stats[i] = stoi(mots[1 + i]);
                }
            }

            else if (mots[0] == "inventaire") {
                if ((int)mots[1][0] != 95) {
                    int t = 0;
                    for (int i = 1; i < nb; i++) {
                        // cout << "section = 4, inventaire, valeur = " << mots[i] << endl;
                        jeu.cfg_joueur.inventaire_ids.ids[t++] = stoi(mots[i]);
                    }
                    jeu.cfg_joueur.inventaire_ids.taille = t;
                } else {
                    jeu.cfg_joueur.inventaire_ids.taille = 0;
                }
            }

        }

        // contraintes
        else if (!skip && section == 5) {
            Contrainte &c = jeu.cfgConditions.contraintes[jeu.cfgConditions.nbContraintes];

            // cout << "section = 5, assigne = c.id, valeur = " << mots[0] << endl;
            c.id = stoi(mots[0]);

            for (int i = 0; i < NB_STATS; i++){
                // cout << "section = 5, assigne = c.stats_min[" << i << "], valeur = " << mots[1 + i] << endl;
                c.stats_min[i] = stoi(mots[1 + i]);
            }

            if ((int)mots[NB_STATS + 1][0] == 95) {
                c.nb_items_possede = 0;
            } else {
                for (int i = 0; i < nb - NB_STATS + 1; i++){
                    c.nb_items_possede = i;
                    // cout << "section = 5, assigne = c.items_possede[" << i << "], valeur = " << mots[i + NB_STATS + 1] << endl;
                    c.items_possede[i] = stoi(mots[i + NB_STATS + 1]);
                }
            }

            if ((8 + c.nb_items_possede) < nb && !mots[8 + c.nb_items_possede].empty()) {
                c.symbole_atteint = mots[8 + c.nb_items_possede][0];
            }

            jeu.cfgConditions.nbContraintes++;
        }

        // condition de victoire
        else if (!skip && section == 6) {
            for (int i = 0; i < nb; i++){
                // cout << "section = 6, assigne victoire.ids[" << jeu.cfgConditions.victoire.nb << "], valeur = " << mots[i] << endl;
                jeu.cfgConditions.victoire.ids[jeu.cfgConditions.victoire.nb++] = stoi(mots[i]);
            }
        }

        // condition de défaite
        else if (!skip && section == 7) {
            for (int i = 0; i < nb; i++){
                // cout << "section = 7, assigne defaite.ids[" << jeu.cfgConditions.defaite.nb << "], valeur = " << mots[i] << endl;
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
            char ca = ligne[x];
            jeu.carte.cases[y][x] = ca;

            // joueur
            if (ca == jeu.cfg_joueur.symbole) {
                jeu.joueur.x = x;
                jeu.joueur.y = y;
            }

            // items : a-z
            if (ca >= 'a' && ca <= 'z') {
                for (int i = 0; i < jeu.nb_cfg_items; i++) {
                    if (jeu.cfg_items[i].symbole == ca) {

                        Items &it = jeu.items[jeu.nb_items];
                        it.x = x;
                        it.y = y;
                        it.idConfig = jeu.cfg_items[i].id;
                        it.actif = true;
                        
                        jeu.nb_items++;
                    }
                }
            }

            // monstres : A-Z
            if (ca >= 'A' && ca <= 'Z') {
                for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
                    if (jeu.cfg_monstres[i].symbole == ca) {

                        Monstre &m = jeu.monstres[jeu.nb_monstres];
                        m.x = x;
                        m.y = y;
                        m.idConfig = jeu.cfg_monstres[i].id;

                        for (int k = 0; k < NB_STATS; k++)
                            m.stats[k] = jeu.cfg_monstres[i].stats_base[k];

                        m.actif = true;

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

void initialiser_jeu(Jeu &jeu) {
    // initialisation des stats du joueur à partir de la configuration
    for (int i = 0; i < NB_STATS; i++) {
        jeu.joueur.stat[i] = jeu.cfg_joueur.stats[i];
    }

    // état du jeu
    jeu.etat_termine = false;
    jeu.victoire = false;

    // brouillard (rendre toutes les cases visibles ou non)
    for (int y = 0; y < TAILLE_MAP_Y; y++) {
        for (int x = 0; x < TAILLE_MAP_X; x++) {
            jeu.carte.visible[y][x] = true; // on affiche tout
        }
    }

    // initialisation console
    PaireCouleur couleurs[1];
    couleurs[0].texte = COLOR_WHITE;
    couleurs[0].fond  = COLOR_BLACK;
    initialiser_console(couleurs, 1);
}

#endif
// NOTE : stoi c'est pour transformer string en int, .size() c'est pour la taille d'une liste
