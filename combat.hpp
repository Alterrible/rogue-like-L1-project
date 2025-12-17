#ifndef COMBAT_HPP
#define COMBAT_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "affichage.hpp"

// applique degats sur une stat ciblee
bool appliquer_degats_stat(int stats[], int stat, int degats) {

    bool applique = true;

    if (stats[stat] > 0) {
        stats[stat] = stats[stat] - degats;
        if (stats[stat] < 0) {
            stats[stat] = 0;
        }
    }
    else {
        applique = false;
    }

    return applique;
}


// tour du joueur
void tour_joueur_combat(Jeu& jeu, int stat_cible) {

    Monstre& m = jeu.monstres[jeu.combat.id_monstre];

    int degats = jeu.joueur.stat[2];

    appliquer_degats_stat(m.stats, stat_cible, degats);

    jeu.combat.tour_joueur = false;
}


// tour du monstre
void tour_monstre_combat(Jeu& jeu) {

    Monstre& m = jeu.monstres[jeu.combat.id_monstre];

    int stat_cible = rand() % NB_STATS;
    int degats = m.stats[2];

    appliquer_degats_stat(jeu.joueur.stat, stat_cible, degats);

    jeu.combat.tour_joueur = true;
}


// verification fin de combat
void verifier_fin_combat(Jeu& jeu) {

    Monstre& m = jeu.monstres[jeu.combat.id_monstre];
    string context;

    if (m.stats[0] <= 0) {

        Config_monstre cfg;
        bool trouve = trouver_config_monstre_par_id(jeu, m.idConfig, cfg);

        bool valide = false;
        if (trouve) {
            valide = check_contrainte(jeu, cfg.id_contrainte, context);
        }

        if (valide) {
            m.actif = false;
            jeu.combat.actif = false;
        }
        else {
            m.stats[0] = 1;
            jeu.modal_txt = "contrainte non remplie";
            jeu.modal_active = true;
        }
    }

    if (jeu.joueur.stat[0] <= 0) {
        jeu.etat_termine = true;
        jeu.victoire = false;
    }
}

// affichage combat
void afficher_combat(const Jeu& jeu) {

    effacer_console();

    const Monstre& m = jeu.monstres[jeu.combat.id_monstre];
    Config_monstre cfg;

    bool trouve = trouver_config_monstre_par_id(
        jeu,
        m.idConfig,
        cfg
    );

    if (trouve) {

        ecrire_string("combat", 1, 1);

        ecrire_string(cfg.nom, 1, 3);
        ecrire_string(cfg.description, 1, 4);

        int i = 0;
        while (i < NB_STATS) {

            string txt_monstre =
                jeu.nom_stats[i] + ": " + to_string(m.stats[i]);

            string txt_joueur =
                jeu.nom_stats[i] + ": " + to_string(jeu.joueur.stat[i]);

            ecrire_string(txt_monstre, 1, 6 + i);
            ecrire_string(txt_joueur, 30, 6 + i);

            i = i + 1;
        }
    }
}


#endif
