#ifndef INIT_HPP
#define INIT_HPP

using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include "lib_projet_ncurses.hpp"
#include "enregistrement.hpp"
#include "maj.hpp"
#include "utils.hpp"

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
        bool skip = false;

        if (!ligne.empty() && ligne[0] == '[') {
            section++;
            skip = true;
        }

        string mots[50];
        int nb = decouper(ligne, mots, 50);
        if (nb == 0 || mots[0][0] == '|') skip = true;

        // ITEMS
        if (!skip && section == 1) {
            Config_item &it = jeu.cfg_items[jeu.nb_cfg_items];

            it.id = stoi(mots[0]);
            it.symbole = mots[1][0];
            it.nom = mots[2];
            it.description = mots[3];

            for (int i = 0; i < NB_STATS; i++) 
                it.bonus[i] = stoi(mots[4 + i]);

            jeu.nb_cfg_items++;
        }

        // MONSTRES
        else if (!skip && section == 2) {
            Config_monstre &m = jeu.cfg_monstres[jeu.nb_cfg_monstres];

            m.id = stoi(mots[0]);
            m.symbole = mots[1][0];
            m.nom = mots[2];
            m.description = mots[3];

            m.typeIA = stoi(mots[22]);

            for (int i = 0; i < NB_STATS; i++)
                m.stats_base[i] = stoi(mots[4 + i]);

            for (int i = 0; i < NB_STATS; i++)
                m.stats_afflige[i] = stoi(mots[10 + i]);

            for (int i = 0; i < NB_STATS; i++)
                m.stats_prit[i] = (stoi(mots[16 + i]) > 0);

            jeu.nb_cfg_monstres++;
        }

        // PORTES
        else if (!skip && section == 3) {
            Config_porte &p = jeu.cfg_portes[jeu.nb_cfg_portes];

            p.id = stoi(mots[0]);
            p.symbole = mots[1][0];
            p.id_contrainte = stoi(mots[2]);

            jeu.nb_cfg_portes++;
        }

        // JOUEUR
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
                for (int s = 0; s < NB_STATS; s++)
                    jeu.cfg_joueur.stats[s] = stoi(mots[1 + s]);
            }

            else if (mots[0] == "inventaire") {
                if (mots[1][0] != '_') {
                    int t = 0;
                    for (int i = 1; i < nb; i++)
                        jeu.cfg_joueur.inventaire_ids.ids[t++] = stoi(mots[i]);
                    jeu.cfg_joueur.inventaire_ids.taille = t;
                } else {
                    jeu.cfg_joueur.inventaire_ids.taille = 0;
                }
            }

            else if (mots[0] == "index_stat_vision") {
                jeu.index_stat_vision = stoi(mots[1]) - 1;
            }

            else if (mots[0] == "nom_stats") {
                for (int s = 0; s < NB_STATS; s++)
                    jeu.nom_stats[s] = mots[1 + s];
            }
        }

        // CONTRAINTES
        else if (!skip && section == 5) {
            Contrainte &c = jeu.cfgConditions.contraintes[jeu.cfgConditions.nbContraintes];

            c.id = stoi(mots[0]);

            for (int i = 0; i < NB_STATS; i++)
                c.stats_min[i] = stoi(mots[1 + i]);

            if (mots[NB_STATS + 1][0] == '_') {
                c.nb_items_possede = 0;
            } else {
                for (int i = 0; i < nb - NB_STATS - 1; i++)
                    c.items_possede[i] = stoi(mots[NB_STATS + 1 + i]);
                c.nb_items_possede = nb - NB_STATS - 1;
            }

            if ((8 + c.nb_items_possede) < nb && !mots[8 + c.nb_items_possede].empty()) {
                c.symbole_atteint = mots[8 + c.nb_items_possede][0];
                c.a_symbole = true;
            }

            jeu.cfgConditions.nbContraintes++;
        }

        // VICTOIRE
        else if (!skip && section == 6) {
            for (int i = 0; i < nb; i++)
                jeu.cfgConditions.victoire.ids[jeu.cfgConditions.victoire.nb++] = stoi(mots[i]);
        }

        // DEFAITE
        else if (!skip && section == 7) {
            for (int i = 0; i < nb; i++)
                jeu.cfgConditions.defaite.ids[jeu.cfgConditions.defaite.nb++] = stoi(mots[i]);
        }
    }

    flux.close();
    return true;
}


bool charger_carte(const string& fichier, Jeu& jeu) {
    fstream flux(fichier, ios::in);
    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    string ligne;
    int y = 0;

    jeu.nb_items = 0;
    jeu.nb_monstres = 0;

    while (getline(flux, ligne)) {

        for (size_t x = 0; x < ligne.size(); x++) {

            char ca = ligne[x];
            jeu.carte.cases[y][x] = ca;
            jeu.carte.ex_cases[y][x] = ca;

            // joueur
            if (ca == jeu.cfg_joueur.symbole) {
                jeu.joueur.x = static_cast<int>(x);
                jeu.joueur.y = y;
            }

            // items
            if (ca >= 'a' && ca <= 'z') {
                for (int i = 0; i < jeu.nb_cfg_items; i++) {
                    if (jeu.cfg_items[i].symbole == ca) {

                        Items &it = jeu.items[jeu.nb_items];

                        it.id = jeu.cfg_items[i].id;
                        it.x = static_cast<int>(x);
                        it.y = y;
                        it.idConfig = jeu.cfg_items[i].id;
                        it.actif = true;

                        jeu.nb_items++;
                    }
                }
            }

            // monstres
            if (ca >= 'A' && ca <= 'Z') {
                for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
                    if (jeu.cfg_monstres[i].symbole == ca) {

                        Monstre &m = jeu.monstres[jeu.nb_monstres];

                        m.x = static_cast<int>(x);
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

    for (int i = 0; i < NB_STATS; i++)
        jeu.joueur.stat[i] = jeu.cfg_joueur.stats[i];

    jeu.joueur.nb_inventaire = jeu.cfg_joueur.inventaire_ids.taille;

    for (int i = 0; i < jeu.cfg_joueur.inventaire_ids.taille; i++)
        jeu.joueur.inventaire[i] = jeu.cfg_joueur.inventaire_ids.ids[i];

    jeu.etat_termine = false;
    jeu.victoire = false;

    mettre_a_jour_visibilite(jeu);

    PaireCouleur couleurs[1];
    couleurs[0].texte = COLOR_WHITE;
    couleurs[0].fond  = COLOR_BLACK;

    initialiser_console(couleurs, 1);
}

#endif
