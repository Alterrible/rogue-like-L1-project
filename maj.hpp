#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "affichage.hpp"
#include <cmath>
#include <iostream>

void mettre_a_jour_monstres(Jeu& jeu) {
    for (int i = 0; i < jeu.nb_monstres; i++) {

        Monstre& m = jeu.monstres[i];

        // mort → désactivation
        if (!m.actif || m.stats[0] <= 0) {
            m.actif = false;
            continue;
        }

        int mx = m.x, my = m.y;
        int jx = jeu.joueur.x;
        int jy = jeu.joueur.y;

        int dist_joueur = distance_manhattan(mx, my, jx, jy);


        // Mode AGGRO — le joueur est dans le rayon Y
        if (dist_joueur <= 6) {

            // attaque si adjacent
            if (dist_joueur <= 1) {
                appliquer_contraintes_monstre_sur_joueur(jeu, m, jeu.joueur);
                continue;
            }

            // déplacement vers le joueur
            int diffX = jx - mx;
            int diffY = jy - my;
            int dx = 0, dy = 0;

            // direction prioritaire = l’axe le plus long
            if (abs_int(diffX) >= abs_int(diffY))
                dx = (diffX > 0) ? 1 : -1;
            else
                dy = (diffY > 0) ? 1 : -1;

            int nx = mx + dx;
            int ny = my + dy;

            bool peut_bouger =
                case_praticable_pour_monstre(jeu, nx, ny, i) &&
                !(nx == jx && ny == jy);

            // si bloqué, essayer l’autre axe
            if (!peut_bouger) {
                dx = 0; dy = 0;
                if (abs_int(diffX) < abs_int(diffY))
                    dx = (diffX > 0) ? 1 : -1;
                else
                    dy = (diffY > 0) ? 1 : -1;

                nx = mx + dx;
                ny = my + dy;

                peut_bouger = case_praticable_pour_monstre(jeu, nx, ny, i);
            }

            if (peut_bouger) {
                m.x = nx;
                m.y = ny;
            }

            continue;
        }

        // MODE BALADE
        // 30% de chance de ne pas bouger
        if (rand() % 100 < 30) continue;

        int dx = 0, dy = 0;
        switch (rand() % 5) { // 0 = immobile, 1-4 = directions
            case 1: dx = 1; break;
            case 2: dx = -1; break;
            case 3: dy = 1; break;
            case 4: dy = -1; break;
        }

        int nx = mx + dx;
        int ny = my + dy;

        if (case_praticable_pour_monstre(jeu, nx, ny, i)) {
            m.x = nx;
            m.y = ny;
        }
    }
}

// update de la visibilité
void mettre_a_jour_visibilite(Jeu& jeu) {
    int rayon = jeu.joueur.stat[jeu.index_stat_vision];
    int px = jeu.joueur.x;
    int py = jeu.joueur.y;

    // Reset
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++) {
            jeu.carte.visible[y][x] = false;
        }
    }    

    // Nombre de branches
    int nb_branches = 360;

    for (int i = 0; i < nb_branches; i++) {
        float angle = (2.0f * M_PI * i) / nb_branches;

        float dx = cosf(angle);
        float dy = sinf(angle);

        float cx = px + 0.5f;
        float cy = py + 0.5f;

        int prev_ix = (int)floor(cx);
        int prev_iy = (int)floor(cy);

        for (int r = 0; r <= rayon; r++) {

            int ix = (int)floor(cx);
            int iy = (int)floor(cy);

            // limites et diag
            if (ix < 0 || ix >= jeu.carte.largeur) break;
            if (iy < 0 || iy >= jeu.carte.hauteur) break;
            if (bloque_par_coin(jeu, prev_ix, prev_iy, ix, iy)) break;

            jeu.carte.visible[iy][ix] = true;

            // cas pour lesquels on ne laisse pas passer "la vue"
            bool bloque = false;
            if (jeu.carte.cases[iy][ix] == '#') bloque = true; // mur = toujours bloquant            
            if ((jeu.carte.cases[iy][ix] >= '0' && jeu.carte.cases[iy][ix] <= '9') && !(ix == jeu.joueur.x && iy == jeu.joueur.y)) bloque = true; // porte = bloquante sauf si c'est celle du joueur
            if (bloque) break;

            prev_ix = ix;
            prev_iy = iy;

            cx += dx;
            cy += dy;
        }
    }

    // case du joueur toujours visible
    jeu.carte.visible[py][px] = true;
}


// conditions de victoire ou défaite
void verifier_conditions_victoire_defaite(Jeu& jeu) {
    string context;
    Liste_id_contraintes& v = jeu.cfgConditions.victoire;
    Liste_id_contraintes& d = jeu.cfgConditions.defaite;

    // vérifier défaite en premier
    for (int ci = 0; ci < d.nb; ci++) {
        int id_contrainte = d.ids[ci];

        if (check_contrainte(jeu, id_contrainte, context, true)) {
            jeu.etat_termine = true;
            jeu.victoire = false;
            return;
        }
    }

    // vérifier victoire
    for (int ci = 0; ci < v.nb; ci++) {
        int id_contrainte = v.ids[ci];

        if (check_contrainte(jeu, id_contrainte, context)) {
            jeu.etat_termine = true;
            jeu.victoire = true;
            return;
        }
    }
}


// update de la carte
void mettre_a_jour_carte(Jeu& jeu) {
    for (int y = 0; y < jeu.carte.hauteur; ++y) {
        for (int x = 0; x < jeu.carte.largeur; ++x) {

            char ca = jeu.carte.cases[y][x];
            bool efface = false;

            if (ca == jeu.cfg_joueur.symbole) {
                jeu.carte.cases[y][x] = '.';
                efface = true;
            }

            if (!efface) {
                for (int i = 0; i < jeu.nb_cfg_monstres && !efface; i++) {
                    if (ca == jeu.cfg_monstres[i].symbole) {
                        jeu.carte.cases[y][x] = '.';
                        efface = true;
                    }
                }
            }

            if (!efface) {
                for (int i = 0; i < jeu.nb_cfg_items; i++) {
                    if (ca == jeu.cfg_items[i].symbole) {
                        jeu.carte.cases[y][x] = '.';
                        efface = true;
                    }
                }
            }
        }
    }

    // replacer les items actifs
    for (int i = 0; i < jeu.nb_items; i++) {

        int x = jeu.items[i].x;
        int y = jeu.items[i].y;

        bool dans_carte =
            x >= 0 && x < jeu.carte.largeur &&
            y >= 0 && y < jeu.carte.hauteur;

        // on ne place que les items encore actifs (non ramassés)
        if (dans_carte && jeu.items[i].actif) {

            Config_item cfg;
            bool trouve = trouver_config_item_par_id(jeu, jeu.items[i].idConfig, cfg);

            if (trouve) {
                jeu.carte.cases[y][x] = cfg.symbole;
            }
        }
    }

    // replacer monstres
    for (int i = 0; i < jeu.nb_monstres; i++) {

        const Monstre& m = jeu.monstres[i];
        bool actif = m.actif;
        bool vivant = (m.stats[0] > 0);

        if (actif && vivant) {

            int x = m.x;
            int y = m.y;

            bool dans_carte =
                x >= 0 && x < jeu.carte.largeur &&
                y >= 0 && y < jeu.carte.hauteur;

            if (dans_carte) {

                Config_monstre cfg;
                bool found = trouver_config_monstre_par_id(jeu, m.idConfig, cfg);

                if (found) {
                    jeu.carte.cases[y][x] = cfg.symbole;
                }
            }
        }
    }

    // placer joueur
    int px = jeu.joueur.x;
    int py = jeu.joueur.y;

    bool dans_carte =
        px >= 0 && px < jeu.carte.largeur &&
        py >= 0 && py < jeu.carte.hauteur;

    if (dans_carte) {
        jeu.carte.cases[py][px] = jeu.cfg_joueur.symbole;
    }
}

#endif
