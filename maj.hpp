#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include "utils.hpp"
#include "affichage.hpp"
#include <cmath>
#include <iostream>

void mettre_a_jour_monstres(Jeu& jeu) {
    int i = 0;

    // boucle sur les monstres
    while (i < jeu.nb_monstres) {

        Monstre& m = jeu.monstres[i];

        // flag de saut vers le monstre suivant
        bool fin_monstre = false;

        // mort → désactivation
        if (!m.actif || m.stats[0] <= 0) {
            m.actif = false;
            fin_monstre = true;
        }

        // traitement si le monstre est encore actif
        if (!fin_monstre) {

            int mx = m.x;
            int my = m.y;
            int jx = jeu.joueur.x;
            int jy = jeu.joueur.y;

            int dist_joueur = distance_manhattan(mx, my, jx, jy);

            // mode aggro
            if (dist_joueur <= 6) {

                // attaque si adjacent
                if (dist_joueur <= 1) {
                    appliquer_contraintes_monstre_sur_joueur(jeu, m, jeu.joueur);
                    fin_monstre = true;
                }

                // déplacement si pas d'attaque
                if (!fin_monstre) {
                    int diffX = jx - mx;
                    int diffY = jy - my;

                    int dx = 0;
                    int dy = 0;

                    // axe prioritaire
                    if (abs_int(diffX) >= abs_int(diffY)) {
                        dx = (diffX > 0) ? 1 : -1;
                    }
                    else {
                        dy = (diffY > 0) ? 1 : -1;
                    }

                    int nx = mx + dx;
                    int ny = my + dy;

                    bool peut_bouger = false;

                    // vérifie déplacement principal
                    if (case_praticable_pour_monstre(jeu, nx, ny, i) && !(nx == jx && ny == jy)) {
                        peut_bouger = true;
                    }

                    // si bloqué → essaye l’autre axe
                    if (!peut_bouger) {
                        dx = 0;
                        dy = 0;

                        if (abs_int(diffX) < abs_int(diffY)) {
                            dx = (diffX > 0) ? 1 : -1;
                        }
                        else {
                            dy = (diffY > 0) ? 1 : -1;
                        }

                        nx = mx + dx;
                        ny = my + dy;

                        if (case_praticable_pour_monstre(jeu, nx, ny, i)) {
                            peut_bouger = true;
                        }
                    }

                    // applique déplacement
                    if (peut_bouger) {
                        m.x = nx;
                        m.y = ny;
                    }

                    fin_monstre = true;
                }
            }

            // mode balade
            if (!fin_monstre) {
                bool bouge = false;

                // 30% de ne pas bouger
                int tirage = rand() % 100;
                if (tirage >= 30) {
                    bouge = true;
                }

                if (bouge) {
                    int dx = 0;
                    int dy = 0;

                    int dir = rand() % 5;

                    // directions aléatoires
                    if (dir == 1) dx = 1;
                    if (dir == 2) dx = -1;
                    if (dir == 3) dy = 1;
                    if (dir == 4) dy = -1;

                    int nx = mx + dx;
                    int ny = my + dy;

                    // applique déplacement si praticable
                    if (case_praticable_pour_monstre(jeu, nx, ny, i)) {
                        m.x = nx;
                        m.y = ny;
                    }
                }
            }
        }

        // passe au monstre suivant
        i = i + 1;
    }
}


// update de la visibilité
void mettre_a_jour_visibilite(Jeu& jeu) {
    int rayon = jeu.joueur.stat[jeu.index_stat_vision];
    int px = jeu.joueur.x;
    int py = jeu.joueur.y;

    // reset la visibilité du tour
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++) {
            jeu.carte.visible[y][x] = false;
        }
    }

    // nombre de rayons projetés
    int nb_branches = 360;

    for (int i = 0; i < nb_branches; i++) {
        // calcul du vecteur direction du rayon
        float angle = (2.0f * M_PI * i) / nb_branches;
        float dx = cosf(angle);
        float dy = sinf(angle);

        // position initiale du rayon centrée sur la case
        float cx = px + 0.5f;
        float cy = py + 0.5f;

        int prev_ix = (int)floor(cx);
        int prev_iy = (int)floor(cy);

        // flag d'arrêt du rayon
        bool arret_rayon = false;

        for (int r = 0; r <= rayon && !arret_rayon; r++) {
            // conversion en coordonnées de grille
            int ix = (int)floor(cx);
            int iy = (int)floor(cy);

            // vérifie les limites de la carte
            bool hors_limite = (ix < 0 || ix >= jeu.carte.largeur || iy < 0 || iy >= jeu.carte.hauteur);
            if (hors_limite) {
                arret_rayon = true;
            }

            // vérifie le blocage diagonal
            if (!arret_rayon) {
                if (bloque_par_coin(jeu, prev_ix, prev_iy, ix, iy)) {
                    arret_rayon = true;
                }
            }

            // mise à jour de la visibilité et de la mémoire si non arrêté
            if (!arret_rayon) {
                jeu.carte.visible[iy][ix] = true;
                jeu.carte.ex_visible[iy][ix] = jeu.carte.cases[iy][ix];
            }

            // vérifie si cette case bloque le champ de vision
            if (!arret_rayon) {
                bool bloque = false;

                // mur bloquant
                if (jeu.carte.cases[iy][ix] == '#') {
                    bloque = true;
                }

                // porte bloquante sauf celle du joueur
                if (jeu.carte.cases[iy][ix] >= '0' && jeu.carte.cases[iy][ix] <= '9') {
                    if (!(ix == px && iy == py)) {
                        bloque = true;
                    }
                }

                if (bloque) {
                    arret_rayon = true;
                }
            }

            // mise à jour des coordonnées précédentes
            if (!arret_rayon) {
                prev_ix = ix;
                prev_iy = iy;

                cx += dx;
                cy += dy;
            }
        }
    }

    // la case du joueur est toujours visible et mémorisée
    jeu.carte.visible[py][px] = true;
    jeu.carte.ex_visible[py][px] = jeu.carte.cases[py][px];
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
