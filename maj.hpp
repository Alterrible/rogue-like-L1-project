#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include <iostream>

// stockage local des zones de base
bool g_base_monstre_init[TAILLE_MAX] = { false };
int  g_base_monstre_x[TAILLE_MAX] = { 0 };
int  g_base_monstre_y[TAILLE_MAX] = { 0 };

// vérifie si une position est dans la zone de base
bool position_dans_zone_base(int i, int x, int y) {
    int bx = g_base_monstre_x[i];
    int by = g_base_monstre_y[i];
    return distance_manhattan(x, y, bx, by) <= RAYON_ZONE_BASE;
}

// update des monstres
void mettre_a_jour_monstres(Jeu& jeu) {
    for (int i = 0; i < jeu.nb_monstres; i++) {

        Monstre& monstre = jeu.monstres[i];
        bool actif = monstre.actif;
        bool vivant = (monstre.stats[0] > 0);

        if (actif && !vivant) {
            monstre.actif = false;
        }
        else if (actif && vivant) {

            if (!g_base_monstre_init[i]) {
                g_base_monstre_x[i] = monstre.x;
                g_base_monstre_y[i] = monstre.y;
                g_base_monstre_init[i] = true;
            }

            int mx = monstre.x;
            int my = monstre.y;
            int jx = jeu.joueur.x;
            int jy = jeu.joueur.y;

            bool joueur_dans_zone = position_dans_zone_base(i, jx, jy);

            if (joueur_dans_zone) {

                int dist = distance_manhattan(mx, my, jx, jy);

                if (dist == 1 || dist == 0) {
                    appliquer_contraintes_monstre_sur_joueur(jeu, monstre, jeu.joueur);
                }
                else {
                    int diffX = jx - mx;
                    int diffY = jy - my;
                    int dx = 0;
                    int dy = 0;

                    if (abs_int(diffX) >= abs_int(diffY)) {
                        dx = (diffX > 0) ? 1 : -1;
                    }
                    else {
                        if (diffY != 0) {
                            dy = (diffY > 0) ? 1 : -1;
                        }
                    }

                    int nx = mx + dx;
                    int ny = my + dy;

                    bool peut_bouger = 
                        position_dans_zone_base(i, nx, ny) &&
                        case_praticable_pour_monstre(jeu, nx, ny, i) &&
                        !(nx == jx && ny == jy);

                    if (!peut_bouger) {

                        dx = 0;
                        dy = 0;

                        if (abs_int(diffX) < abs_int(diffY)) {
                            if (diffX != 0) {
                                dx = (diffX > 0) ? 1 : -1;
                            }
                        }
                        else {
                            if (diffY != 0) {
                                dy = (diffY > 0) ? 1 : -1;
                            }
                        }

                        nx = mx + dx;
                        ny = my + dy;

                        peut_bouger =
                            position_dans_zone_base(i, nx, ny) &&
                            case_praticable_pour_monstre(jeu, nx, ny, i) &&
                            !(nx == jx && ny == jy);
                    }

                    if (peut_bouger) {
                        monstre.x = nx;
                        monstre.y = ny;
                    }
                }
            }
        }
    }
}

// update de la visibilité
void mettre_a_jour_visibilite(Jeu& jeu) {
}

// conditions de victoire ou défaite
void verifier_conditions_victoire_defaite(Jeu& jeu) {
    Liste_id_contraintes& v = jeu.cfgConditions.victoire;
    Liste_id_contraintes& d = jeu.cfgConditions.defaite;

    // vérifier défaite en premier
    for (int ci = 0; ci < d.nb; ci++) {
        int id_contrainte = d.ids[ci];

        if (check_contrainte(jeu, id_contrainte, true)) {
            jeu.etat_termine = true;
            jeu.victoire = false;
            return;
        }
    }

    // vérifier victoire
    for (int ci = 0; ci < v.nb; ci++) {
        int id_contrainte = v.ids[ci];

        if (check_contrainte(jeu, id_contrainte)) {
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
            bool found = trouver_config_item_par_id(jeu, jeu.items[i].idConfig, cfg);

            if (found) {
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
