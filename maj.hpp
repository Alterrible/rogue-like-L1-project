#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include <iostream>

// --- fonctions utilitaires ---

int abs_int(int x){
    return (x < 0) ? -x : x;
}

int distance_manhattan(int x1, int y1, int x2, int y2) {
    return abs_int(x1 - x2) + abs_int(y1 - y2);
}

//recherche config item sans pointeur
bool trouver_config_item_par_id(const Jeu& jeu, int idItem, Config_item& out) {
    for (int i = 0; i < jeu.nb_cfg_items; ++i) {
        if (jeu.cfg_items[i].id == idItem) {
            out = jeu.cfg_items[i];
            return true;
        }
    }
    return false;
}

//recherche config monstre sans pointeur
bool trouver_config_monstre_par_id(const Jeu& jeu, int idMonstre, Config_monstre& out) {
    for (int i = 0; i < jeu.nb_cfg_monstres; ++i) {
        if (jeu.cfg_monstres[i].id == idMonstre) {
            out = jeu.cfg_monstres[i];
            return true;
        }
    }
    return false;
}

//applique les effets d'un monstre sur le joueur
void appliquer_contraintes_monstre_sur_joueur(const Jeu& jeu, const Monstre& m, Joueur& joueur) {
    Config_monstre cfg;
    bool ok = trouver_config_monstre_par_id(jeu, m.idConfig, cfg);

    if (ok) {
        for (int s = 0; s < NB_STATS; ++s) {
            int delta = cfg.stats_afflige[s];
            if (delta > 0) {
                joueur.stat[s] -= delta;
                if (joueur.stat[s] < 0) {
                    joueur.stat[s] = 0;
                }
            }
        }
    }
}
// ---

//stockage local des zones de base
bool g_base_monstre_init[TAILLE_MAX] = { false };
int  g_base_monstre_x[TAILLE_MAX]    = { 0 };
int  g_base_monstre_y[TAILLE_MAX]    = { 0 };

const int RAYON_ZONE_BASE = 6;

//vérifie si une position est dans la zone de base
bool position_dans_zone_base(int i, int x, int y) {
    int bx = g_base_monstre_x[i];
    int by = g_base_monstre_y[i];
    return distance_manhattan(x, y, bx, by) <= RAYON_ZONE_BASE;
}

//vérifie si une case est praticable pour un monstre
bool case_praticable_pour_monstre(const Jeu& jeu, int x, int y, int indexMonstre) {
    if (x < 0 || x >= jeu.carte.largeur || y < 0 || y >= jeu.carte.hauteur) {
        return false;
    }

    if (jeu.carte.cases[y][x] != '.') {
        return false;
    }

    for (int j = 0; j < jeu.nb_monstres; ++j) {
        bool autre_monstre = (j != indexMonstre);
        bool actif = jeu.monstres[j].actif;
        bool vivant = (jeu.monstres[j].stats[0] > 0);

        if (autre_monstre && actif && vivant) {
            if (jeu.monstres[j].x == x && jeu.monstres[j].y == y) {
                return false;
            }
        }
    }

    return true;
}

//update des monstres
void mettre_a_jour_monstres(Jeu& jeu) {
    for (int i = 0; i < jeu.nb_monstres; ++i) {

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

//update de la visibilité
void mettre_a_jour_visibilite(Jeu& jeu) {
    (void)jeu;
}

//conditions de victoire ou défaite
void verifier_conditions_victoire_defaite(Jeu& jeu) {
    if (jeu.joueur.stat[0] <= 0) {
        jeu.etat_termine = true;
        jeu.victoire = false;
    }
}

//update des stats joueur
void mettre_a_jour_stats_joueur(Jeu& jeu) {
    int stats_temp[NB_STATS];

    for (int s = 0; s < NB_STATS; ++s) {
        stats_temp[s] = jeu.cfg_joueur.stats[s];
    }

    stats_temp[0] = jeu.joueur.stat[0];

    for (int i = 0; i < jeu.joueur.nb_inventaire; ++i) {

        int idItem = jeu.joueur.inventaire[i];
        Config_item cfg;
        bool found = trouver_config_item_par_id(jeu, idItem, cfg);

        if (found) {
            for (int s = 0; s < NB_STATS; ++s) {
                stats_temp[s] += cfg.bonus[s];
            }
        }
    }

    for (int s = 0; s < NB_STATS; ++s) {
        if (stats_temp[s] < 0) {
            stats_temp[s] = 0;
        }
        jeu.joueur.stat[s] = stats_temp[s];
    }
}

//update de la carte

void mettre_a_jour_carte(Jeu& jeu) {
    for (int y = 0; y < jeu.carte.hauteur; ++y) {
        for (int x = 0; x < jeu.carte.largeur; ++x) {

            char c = jeu.carte.cases[y][x];
            bool efface = false;

            if (c == jeu.cfg_joueur.symbole) {
                jeu.carte.cases[y][x] = '.';
                efface = true;
            }

            if (!efface) {
                for (int i = 0; i < jeu.nb_cfg_monstres && !efface; ++i) {
                    if (c == jeu.cfg_monstres[i].symbole) {
                        jeu.carte.cases[y][x] = '.';
                        efface = true;
                    }
                }
            }

            if (!efface) {
                for (int i = 0; i < jeu.nb_cfg_items; ++i) {
                    if (c == jeu.cfg_items[i].symbole) {
                        jeu.carte.cases[y][x] = '.';
                        efface = true;
                    }
                }
            }
        }
    }

    //replacer items
    for (int i = 0; i < jeu.nb_items; ++i) {

        int x = jeu.items[i].x;
        int y = jeu.items[i].y;

        bool dans_carte =
            x >= 0 && x < jeu.carte.largeur &&
            y >= 0 && y < jeu.carte.hauteur;

        if (dans_carte) {

            Config_item cfg;
            bool found = trouver_config_item_par_id(jeu, jeu.items[i].idConfig, cfg);

            if (found) {
                jeu.carte.cases[y][x] = cfg.symbole;
            }
        }
    }

    //replacer monstres
    for (int i = 0; i < jeu.nb_monstres; ++i) {

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

    //placer joueur
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
