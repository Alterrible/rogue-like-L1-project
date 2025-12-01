#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include <iostream>
using namespace std;

// gpt : applique les contraintes des monstres sur le joueur lorsqu'un monstre atteint la case du joueur
void appliquer_contraintes_monstre_sur_joueur(Monstre &m, Joueur &joueur) {
    for (int s = 0; s < NB_STATS; ++s) {
        int delta = m.contrainte_stats[s];
        if (delta >= 0) continue;

        if (s == 0) {
            // gpt : stat[0] = vie, on applique ça comme des dégâts
            joueur.stat[0] += delta; // delta < 0
            if (joueur.stat[0] < 0) joueur.stat[0] = 0;
        } else {
            joueur.stat[s] += delta;
            if (joueur.stat[s] < 0) joueur.stat[s] = 0;
        }
    }
}

// système d'ia
void mettre_a_jour_monstres(Jeu &jeu) {
    // gpt : tous les monstres actifs se déplacent vers le joueur (si vivants)
    for (int i = 0; i < jeu.nb_monstres; i++) {

        if (!jeu.monstres[i].actif) {
            continue;
        }

        if (jeu.monstres[i].stats[0] <= 0) {
            // gpt : monstre mort, on le rend inactif
            jeu.monstres[i].actif = false;
            continue;
        }

        int mx = jeu.monstres[i].x;
        int my = jeu.monstres[i].y;

        // calcule direction vers le joueur si le monstre est à gauche → il va vers la droite et idem pour les autres directions
        int dx = 0;
        int dy = 0;
    
        if (mx < jeu.joueur.x) dx = 1;
        else if (mx > jeu.joueur.x) dx = -1;
        if (my < jeu.joueur.y) dy = 1;
        else if (my > jeu.joueur.y) dy = -1;

        int nx = mx + dx;
        int ny = my + dy;

        // gpt : vérification des bornes de la carte
        if (nx >= 0 && nx < jeu.carte.largeur && ny >= 0 && ny < jeu.carte.hauteur) {

            // gpt : on interdit le déplacement dans les murs (on garde la logique sur '#')
            if (jeu.carte.cases[ny][nx] != '#') {

                // gpt : éviter que deux monstres se marchent dessus
                bool case_occupee_par_monstre = false;
                for (int j = 0; j < jeu.nb_monstres; ++j) {
                    if (j == i) continue;
                    if (!jeu.monstres[j].actif) continue;
                    if (jeu.monstres[j].x == nx && jeu.monstres[j].y == ny) {
                        case_occupee_par_monstre = true;
                        break;
                    }
                }

                if (case_occupee_par_monstre) {
                    continue;
                }

                // gpt : si la case ciblée est celle du joueur → on applique les contraintes du monstre
                if (nx == jeu.joueur.x && ny == jeu.joueur.y) {
                    appliquer_contraintes_monstre_sur_joueur(jeu.monstres[i], jeu.joueur);
                    // gpt : le monstre ne prend pas la place du joueur, il reste où il est
                } else {
                    // gpt : déplacement normal
                    jeu.monstres[i].x = nx;
                    jeu.monstres[i].y = ny;
                }
            }
        }
    }
}

void mettre_a_jour_visibilite(Jeu &jeu) {
    // gpt : visibilité non implémentée ici (placeholder conservé)
}

void verifier_conditions_victoire_defaite(Jeu &jeu) {
    // cas de la mort
    if (jeu.joueur.stat[0] <= 0) {
        jeu.etat_termine = true;
        jeu.victoire = false;
        return;
    }
}

// gpt : recalcule les stats du joueur à partir de la config + des items + des contraintes "statistiques" des monstres
void mettre_a_jour_stats_joueur(Jeu &jeu) {
    int s;

    // gpt : on part des stats de base du joueur pour toutes les stats SAUF la vie (stat[0] déjà modifiée par les dégâts)
    int stats_temp[NB_STATS];
    for (s = 0; s < NB_STATS; ++s) {
        stats_temp[s] = jeu.cfg_joueur.stats[s];
    }

    // gpt : on ne touche pas à la vie dans ce recalcul
    stats_temp[0] = jeu.joueur.stat[0];

    // gpt : ajout des bonus des items possédés par le joueur
    for (int i = 0; i < jeu.joueur.nb_inventaire; ++i) {
        int idCfgItem = jeu.joueur.inventaire[i];
        if (idCfgItem < 0 || idCfgItem >= jeu.nb_cfg_items) {
            continue;
        }

        for (s = 0; s < NB_STATS; ++s) {
            // gpt : on ajoute le bonus directement ; la vie (0) peut aussi être impactée si designé ainsi
            stats_temp[s] += jeu.cfg_items[idCfgItem].bonus[s];
        }
    }

    // gpt : application des contraintes de stats des monstres à portée (ici même case que le joueur)
    for (int i = 0; i < jeu.nb_monstres; ++i) {
        if (!jeu.monstres[i].actif) continue;
        if (jeu.monstres[i].stats[0] <= 0) continue;

        // gpt : on ne considère que les monstres sur la même case que le joueur
        if (jeu.monstres[i].x == jeu.joueur.x && jeu.monstres[i].y == jeu.joueur.y) {
            for (s = 0; s < NB_STATS; ++s) {
                int delta = jeu.monstres[i].contrainte_stats[s];
                if (delta >= 0) continue;

                stats_temp[s] += delta;
                if (stats_temp[s] < 0) stats_temp[s] = 0;
            }
        }
    }

    // gpt : écriture finale dans les stats du joueur
    for (s = 0; s < NB_STATS; ++s) {
        jeu.joueur.stat[s] = stats_temp[s];
    }
}

// gpt : met à jour la représentation de la carte (cases) pour le prochain rafraîchissement
void mettre_a_jour_carte(Jeu &jeu) {
    // gpt : 1) on efface les anciennes positions dynamiques (joueur, monstres, items)
    for (int y = 0; y < jeu.carte.hauteur; ++y) {
        for (int x = 0; x < jeu.carte.largeur; ++x) {
            char c = jeu.carte.cases[y][x];

            // gpt : effacer le joueur
            if (c == jeu.cfg_joueur.symbole) {
                jeu.carte.cases[y][x] = '.';
                continue;
            }

            bool efface = false;

            // gpt : effacer les monstres
            for (int i = 0; i < jeu.nb_cfg_monstres && !efface; ++i) {
                if (c == jeu.cfg_monstres[i].symbole) {
                    jeu.carte.cases[y][x] = '.';
                    efface = true;
                }
            }
            if (efface) continue;

            // gpt : effacer les items
            for (int i = 0; i < jeu.nb_cfg_items; ++i) {
                if (c == jeu.cfg_items[i].symbole) {
                    jeu.carte.cases[y][x] = '.';
                    break;
                }
            }
        }
    }

    // gpt : 2) replacer tous les items
    for (int i = 0; i < jeu.nb_items; ++i) {
        int x = jeu.items[i].x;
        int y = jeu.items[i].y;
        int idCfgItem = jeu.items[i].idConfig;

        if (x < 0 || x >= jeu.carte.largeur || y < 0 || y >= jeu.carte.hauteur) {
            continue;
        }
        if (idCfgItem < 0 || idCfgItem >= jeu.nb_cfg_items) {
            continue;
        }

        jeu.carte.cases[y][x] = jeu.cfg_items[idCfgItem].symbole;
    }

    // gpt : 3) replacer tous les monstres vivants et actifs
    for (int i = 0; i < jeu.nb_monstres; ++i) {
        if (!jeu.monstres[i].actif) continue;
        if (jeu.monstres[i].stats[0] <= 0) continue;

        int x = jeu.monstres[i].x;
        int y = jeu.monstres[i].y;
        int idCfgMonstre = jeu.monstres[i].idConfig;

        if (x < 0 || x >= jeu.carte.largeur || y < 0 || y >= jeu.carte.hauteur) {
            continue;
        }
        if (idCfgMonstre < 0 || idCfgMonstre >= jeu.nb_cfg_monstres) {
            continue;
        }

        jeu.carte.cases[y][x] = jeu.cfg_monstres[idCfgMonstre].symbole;
    }

    // gpt : 4) placer le joueur en dernier (prioritaire pour l'affichage)
    int px = jeu.joueur.x;
    int py = jeu.joueur.y;
    if (px >= 0 && px < jeu.carte.largeur && py >= 0 && py < jeu.carte.hauteur) {
        jeu.carte.cases[py][px] = jeu.cfg_joueur.symbole;
    }
}

void mettre_a_jour_jeu(Jeu &jeu){
    mettre_a_jour_monstres(jeu);
    mettre_a_jour_visibilite(jeu);
    verifier_conditions_victoire_defaite(jeu);

    // gpt : mettre à jour les données de joueurs : 
    // gpt : - recalcule de stats suivant les items possédés
    // gpt : - prise en compte des contraintes de stats des monstres
    mettre_a_jour_stats_joueur(jeu);

    // gpt : calcule de l'état de la carte (mise à jour de carte.cases[y][x])
    mettre_a_jour_carte(jeu);
}

#endif
