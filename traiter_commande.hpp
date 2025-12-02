#ifndef TRAITER_COMMANDE_HPP
#define TRAITER_COMMANDE_HPP

#include "enregistrement.hpp"
#include <iostream>
using namespace std;


int trouver_monstre(const Jeu& jeu, int x, int y) {
    for (int i = 0; i < jeu.nb_monstres; i++) {
        const Monstre& m = jeu.monstres[i];

        if (m.actif && m.x == x && m.y == y) {
            return i;
        }
    }

    return -1;
}

int trouver_item(const Jeu& jeu, int x, int y) {
    for (int i = 0; i < jeu.nb_items; i++) {
        const Items& it = jeu.items[i];

        if (it.actif && it.x == x && it.y == y) {
            return i;
        }
    }

    return -1;
}


// -----------------------------------------------------------------------------
// Vérifie si la stat i_stat du joueur est >= stat_requise
// -----------------------------------------------------------------------------
bool a_stat(Jeu& jeu, int i_stat, int stat_requise) {
    return jeu.joueur.stat[i_stat] >= stat_requise;
}

// -----------------------------------------------------------------------------
// Vérifie si le joueur possède NB items d'id donné
// Exemple : a_items(jeu, idItem = 3, nbVoulu = 2)
// -----------------------------------------------------------------------------
bool a_items(Jeu& jeu, int id_item_requis, int nb_voulu) {
    int nb_check = 0;

    for (int inv = 0; inv < jeu.joueur.nb_inventaire; inv++) {
        int idItem = jeu.joueur.inventaire[inv];

        if (idItem == id_item_requis) {
            nb_check++;

            if (nb_check >= nb_voulu) {
                return true;
            }
        }
    }

    return false; // pas assez d’items
}

// -----------------------------------------------------------------------------
// Vérifie une contrainte (stat, items, etc.) selon cfgConditions
// id_contrainte → index d’une condition dans cfgConditions.contraintes[]
// -----------------------------------------------------------------------------
bool check_contrainte(Jeu& jeu, int id_contrainte) {
    int indexCtr = -1;

    // Recherche de la contrainte par son ID
    for (int c = 0; c < jeu.cfgConditions.nbContraintes; c++) {
        if (jeu.cfgConditions.contraintes[c].id == id_contrainte) {
            indexCtr = c;
            break;
        }
    }

    // Aucune contrainte trouvée
    if (indexCtr == -1)
        return false;

    const Contrainte& ctr = jeu.cfgConditions.contraintes[indexCtr];

    // -----------------------------------------
    // Vérification des items requis
    // -----------------------------------------
    for (int i = 0; i < ctr.nb_items_possede; i++) {

        int id_item_requis = ctr.items_possede[i];

        // Chaque item listé doit être possédé au moins 1 fois
        if (!a_items(jeu, id_item_requis, 1)) {
            return false; // item manquant
        }
    }

    // -----------------------------------------
    // Vérification des stats minimales
    // -----------------------------------------
    for (int s = 0; s < NB_STATS; s++) {

        int min_req = ctr.stats_min[s];

        // 0 signifie "aucune contrainte"
        if (min_req > 0) {
            if (!a_stat(jeu, s, min_req)) {
                return false; // stat insuffisante
            }
        }
    }

    // -----------------------------------------
    // Toutes les conditions sont remplies
    // -----------------------------------------
    return true;
}

// -----------------------------------------------------------------------------
// Vérifie si la porte (symbole = '0'..'9') peut être ouverte
// -----------------------------------------------------------------------------
bool porte_valide(Jeu& jeu, char c, int /*nouveauY*/, int /*nouveauX*/) {
    // Si ce n'est pas une porte (0..9), pas de contrainte
    if (!(c >= '0' && c <= '9')) return true;

    // On cherche la porte correspondante dans la configuration
    for (int p = 0; p < jeu.nb_cfg_portes; p++) {
        const Config_porte& porte = jeu.cfg_portes[p];

        if (porte.symbole == c) {
            // Vérifie la contrainte associée
            bool ok = check_contrainte(jeu, porte.id_contrainte);
            return ok;
        }
    }

    // Symbole de porte inconnu -> porte invalide
    return false;
}

// ---------------------------------------------------------------------------
//  TRAITEMENT PRINCIPAL
// ---------------------------------------------------------------------------
void traiter_commande(char cmd, Jeu &jeu) {
    int nouveauX = jeu.joueur.x;
    int nouveauY = jeu.joueur.y;

    bool commande_deplacement = false;
    bool commande_interaction = false;

    // -----------------------------------------------------------------------
    // 1) RECONNAISSANCE DU TYPE DE COMMANDE
    // -----------------------------------------------------------------------
    if (cmd == 'z') { nouveauY--; commande_deplacement = true; } // haut
    else if (cmd == 's') { nouveauY++; commande_deplacement = true; } // bas
    else if (cmd == 'q') { nouveauX--; commande_deplacement = true; } // gauche
    else if (cmd == 'd') { nouveauX++; commande_deplacement = true; } // droite

    else if (cmd == 'i') { nouveauY--; commande_interaction = true; } // haut
    else if (cmd == 'k') { nouveauY++; commande_interaction = true; } // bas
    else if (cmd == 'j') { nouveauX--; commande_interaction = true; } // gauche
    else if (cmd == 'l') { nouveauX++; commande_interaction = true; } // droite

    // -----------------------------------------------------------------------
    // 2) DÉPLACEMENT
    // -----------------------------------------------------------------------
    if (commande_deplacement) {

        // Vérifie d'abord que la case est dans la carte
        bool case_valide =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        if (case_valide) {
            char c = jeu.carte.cases[nouveauY][nouveauX];
            bool marchable = (c != '#' && !(c >= 'A' && c <= 'Z'));
            bool est_porte_valide = porte_valide(jeu, c, nouveauY, nouveauX);

            // On ne marche que sur les cases marchables ET dont la "porte" est valide
            if (marchable && est_porte_valide) {
                jeu.joueur.x = nouveauX;
                jeu.joueur.y = nouveauY;
            }
        }
    }

    // -----------------------------------------------------------------------
    // 3) INTERACTION : i k j l
    // -----------------------------------------------------------------------
    if (commande_interaction) {

        bool case_dans_carte =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        bool interaction_possible = case_dans_carte;

        // si hors carte, on ignore simplement l'interaction
        if (interaction_possible) {

            int id_monstre = trouver_monstre(jeu, nouveauX, nouveauY);
            int id_item = trouver_item(jeu, nouveauX, nouveauY);

            cout << to_string(id_monstre) << endl;
            cout << to_string(id_item) << endl;

            // -------------------------------------------------------------------
            // → MONSTRE ?
            // -------------------------------------------------------------------
            if (id_monstre != -1) {

                Monstre &m = jeu.monstres[id_monstre];
                Config_monstre &cfg_m = jeu.cfg_monstres[m.idConfig];

                for (int s = 0; s < NB_STATS; s++) {
                    if (cfg_m.stats_prit[s]) {
                        m.stats[s] -= jeu.joueur.stat[s];
                        if (m.stats[s] < 0)
                            m.stats[s] = 0;
                    }
                }

                bool mort = true;
                for (int s = 0; s < NB_STATS; s++) {
                    if (m.stats[s] > 0) {
                        mort = false;
                        break;
                    }
                }

                if (mort) {
                    m.actif = false;
                    jeu.carte.cases[nouveauY][nouveauX] = '.';
                }
            }

            // -------------------------------------------------------------------
            // → ITEM ?
            // -------------------------------------------------------------------
            else if (id_item != -1) {

                Items &it = jeu.items[id_item];

                // On stocke un identifiant d'item (ici idConfig, mais adapte si besoin)
                if (jeu.joueur.nb_inventaire < TAILLE_MAX) {
                    jeu.joueur.inventaire[jeu.joueur.nb_inventaire] = it.idConfig;
                    jeu.joueur.nb_inventaire++;
                }

                it.actif = false;
                jeu.carte.cases[nouveauY][nouveauX] = '.';
            }
        }
    }
}

#endif
