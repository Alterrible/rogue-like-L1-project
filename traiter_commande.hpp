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

// vérifie si la stat du joueur est suffisante
bool a_stat(Jeu& jeu, int i_stat, int stat_requise) {
    return jeu.joueur.stat[i_stat] >= stat_requise;
}

// vérifie si le joueur possède nb items d’un id donné
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

    return false;
}

// vérifie une contrainte (stat, items…) à partir de cfgconditions
bool check_contrainte(Jeu& jeu, int id_contrainte) {
    int indexCtr = -1;

    // cherche la contrainte par id
    for (int c = 0; c < jeu.cfgConditions.nbContraintes; c++) {
        if (jeu.cfgConditions.contraintes[c].id == id_contrainte) {
            indexCtr = c;
            break;
        }
    }

    // aucune contrainte trouvée
    if (indexCtr == -1)
        return false;

    const Contrainte& ctr = jeu.cfgConditions.contraintes[indexCtr];

    // vérifie les items requis
    for (int i = 0; i < ctr.nb_items_possede; i++) {

        int id_item_requis = ctr.items_possede[i];

        if (!a_items(jeu, id_item_requis, 1)) {
            return false;
        }
    }

    // vérifie les stats minimales
    for (int s = 0; s < NB_STATS; s++) {

        int min_req = ctr.stats_min[s];

        if (min_req > 0) {
            if (!a_stat(jeu, s, min_req)) {
                return false;
            }
        }
    }

    // toutes les conditions sont remplies
    return true;
}

// vérifie si la porte 0..9 peut être ouverte
bool porte_valide(Jeu& jeu, char c) {
    if (!(c >= '0' && c <= '9')) return true;

    for (int p = 0; p < jeu.nb_cfg_portes; p++) {
        const Config_porte& porte = jeu.cfg_portes[p];

        if (porte.symbole == c) {
            return check_contrainte(jeu, porte.id_contrainte);
        }
    }

    return false;
}

void attaque(Jeu& jeu, int id_monstre) {
    Monstre &m = jeu.monstres[id_monstre];

    for (int m_cfg_i = 0; m_cfg_i < TAILLE_MONSTRES; m_cfg_i++) {
        Config_monstre &m_cfg = jeu.cfg_monstres[m_cfg_i];
        
        if (m_cfg.id == m.idConfig) {
            for (int s = 0; s < NB_STATS; s++) {
                if (m_cfg.stats_prit[s]) {
                    m.stats[s] -= jeu.joueur.stat[s];
                    if (m.stats[s] <= 0) m.actif = false;
                }
            }
        }
    }
}

void ramasser(Jeu& jeu, int id_item) {
    Items &it = jeu.items[id_item];

    // ajoute l’item à l’inventaire
    if (jeu.joueur.nb_inventaire < TAILLE_MAX) {
        jeu.joueur.inventaire[jeu.joueur.nb_inventaire] = it.idConfig;
        jeu.joueur.nb_inventaire++;
    }

    it.actif = false;
}

// traitement principal
void traiter_commande(char cmd, Jeu &jeu) {
    int nouveauX = jeu.joueur.x;
    int nouveauY = jeu.joueur.y;

    bool commande_deplacement = false;
    bool commande_interaction = false;

    // détection entrée
    if (cmd == 'z') { nouveauY--; commande_deplacement = true; }
    else if (cmd == 's') { nouveauY++; commande_deplacement = true; }
    else if (cmd == 'q') { nouveauX--; commande_deplacement = true; }
    else if (cmd == 'd') { nouveauX++; commande_deplacement = true; }

    else if (cmd == 'i') { nouveauY--; commande_interaction = true; }
    else if (cmd == 'k') { nouveauY++; commande_interaction = true; }
    else if (cmd == 'j') { nouveauX--; commande_interaction = true; }
    else if (cmd == 'l') { nouveauX++; commande_interaction = true; }

    // déplacement du joueur
    if (commande_deplacement) {

        // vérifie si la case est dans la carte et validité
        bool case_valide =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        if (case_valide) {
            char c = jeu.carte.cases[nouveauY][nouveauX];
            bool marchable = true; //(c != '#' && !(c >= 'A' && c <= 'Z'));
            bool est_porte_valide = porte_valide(jeu, c);

            if (marchable && est_porte_valide) {
                jeu.joueur.x = nouveauX;
                jeu.joueur.y = nouveauY;
            }
        }
    }

    // interaction i k j l
    if (commande_interaction) {

        bool case_dans_carte =
            nouveauX >= 0 && nouveauX < jeu.carte.largeur &&
            nouveauY >= 0 && nouveauY < jeu.carte.hauteur;

        bool interaction_possible = case_dans_carte;

        if (interaction_possible) {

            int id_monstre = trouver_monstre(jeu, nouveauX, nouveauY);
            int id_item = trouver_item(jeu, nouveauX, nouveauY);

            // → monstre ? || → item ?
            if (id_monstre != -1) attaque(jeu, id_monstre);
            else if (id_item != -1) ramasser(jeu, id_item);
        }
    }
}

#endif
