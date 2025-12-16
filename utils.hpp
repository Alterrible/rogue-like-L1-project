#ifndef UTILS_HPP
#define UTILS_HPP

#include "enregistrement.hpp"
#include "affichage.hpp"
#include <iostream>
#include <cmath>

// ---- FONCTIONS UTILES ----

// permet d'avoir un string avec des espaces
inline string underscore_espace(const string& input) {
    string result = input;
    for (char& c : result) {
        if (c == '_') {
            c = ' ';
        }
    }
    return result;
}


// découpe une ligne en mots
int decouper(const string& ligne, string mots[], int max_mots) {
    int nb = 0;
    string mot = "";

    for (size_t i = 0; i <= ligne.size(); i++) {
        if (i == ligne.size() || ligne[i] == ' ') {
            if (!mot.empty() && mot[0] != 13 && nb < max_mots) {
                mots[nb++] = mot;
                mot.clear();
            }
        } else {
            mot += ligne[i];
        }
    }
    return nb;
}

// def : la somme des différences absolues de leurs coordonnées cartésiennes (méthode pour avoir "la distance en diag suivant les carrées")
int distance_manhattan(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool bloque_par_coin(const Jeu& jeu, int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    // diagonale ?
    if (abs(dx) == 1 && abs(dy) == 1) {
        // mur en horizontal ou vertical qui bloque la diagonale
        bool mur1 = (jeu.carte.cases[y0][x1] == '#');
        bool mur2 = (jeu.carte.cases[y1][x0] == '#');

        if (mur1 && mur2) return true;
    }
    return false;
}


// ---- ITEMS ----

// recherche config item
bool trouver_config_item_par_id(const Jeu& jeu, int idItem, Config_item& out) {
    for (int i = 0; i < jeu.nb_cfg_items; i++) { 
        if (jeu.cfg_items[i].id == idItem) {
            out = jeu.cfg_items[i];
            return true;
        }
    }
    return false;
}

// trouver un item à une position
int trouver_item(const Jeu& jeu, int x, int y) {
    for (int i = 0; i < jeu.nb_items; i++) {
        const Items& it = jeu.items[i];
        if (it.actif && it.x == x && it.y == y) return i;
    }
    return -1;
}

// vérifier si joueur possède nb items d’un id donné
bool a_items(Jeu& jeu, int id_item_requis) {
    for (int inv = 0; inv < jeu.joueur.nb_inventaire; inv++) {
        if (jeu.joueur.inventaire[inv] == id_item_requis) {
            return true;
        }
    }
    return false;
}

int item_inventaire(Jeu& jeu, int id_config_item, bool config_trouvee) {
    int idx = -1;
    if (config_trouvee) {
        for (int i = 0; i < jeu.nb_inventaire_items; i++) {
            if (jeu.inventaire_items[i].id_config == id_config_item && idx == -1) {
                idx = i;
            }
        }
    }
    return idx;
}

// consomme un item et applique les bonus
void utiliser_item(Jeu& jeu, int id_config_item) {
    Config_item cfg;
    bool config_trouvee = trouver_config_item_par_id(jeu, id_config_item, cfg);

    // recherche de l'item dans l'inventaire
    int idx = item_inventaire(jeu, id_config_item, config_trouvee);

    // verification de la presence de l'item
    bool item_trouve = (idx != -1);

    // verification des quantites restantes
    bool item_utilisable = false;
    if (item_trouve) {
        item_utilisable = (jeu.inventaire_items[idx].restants > 0);
    }

    // application des bonus sur le joueur
    if (config_trouvee && item_trouve && item_utilisable) {
        for (int s = 0; s < NB_STATS; s++) {
            jeu.joueur.stat[s] += cfg.bonus[s];
        }
    }

    // mise a jour des donnees d'inventaire
    if (config_trouvee && item_trouve && item_utilisable) {
        Inventaire_item& inv = jeu.inventaire_items[idx];
        inv.utilises++;
        inv.restants--;
    }
}

// ramasser un item
#include "inv.hpp"
void ramasser(Jeu& jeu, int id_item, bool id_est_config = false) {
    Joueur& j = jeu.joueur;

    int idConfig;
    bool desactiver_item = false;

    if (id_est_config) {
        idConfig = id_item;
    } else {
        Items& it = jeu.items[id_item];
        idConfig = it.idConfig;
        desactiver_item = true;
    }

    Config_item cfg;
    bool config_trouvee = trouver_config_item_par_id(jeu, idConfig, cfg);

    // ajout à l’inventaire
    if (j.nb_inventaire < TAILLE_MAX) {
        j.inventaire[j.nb_inventaire++] = idConfig;
        maj_inv(jeu, idConfig);
    }

    if (config_trouvee && !cfg.conso) {
        int idx = item_inventaire(jeu, idConfig, config_trouvee);
        if (idx >= 0) {
            Inventaire_item& inv = jeu.inventaire_items[idx];
            inv.utilises++;
            inv.restants--;
        }
    }

    if (desactiver_item) {
        jeu.items[id_item].actif = false;
    }
}


// ---- STATS ----

// vérifier si joueur a une stat suffisante
bool a_stat(Jeu& jeu, int i_stat, int stat_requise) {
    return jeu.joueur.stat[i_stat] >= stat_requise;
}


// ---- MONSTRES ----

// recherche config monstre
bool trouver_config_monstre_par_id(const Jeu& jeu, int idMonstre, Config_monstre& out) {
    for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
        if (jeu.cfg_monstres[i].id == idMonstre) {
            out = jeu.cfg_monstres[i];
            return true;
        }
    }
    return false;
}

// trouver monstre à une position
int trouver_monstre(const Jeu& jeu, int x, int y) {
    for (int i = 0; i < jeu.nb_monstres; i++) {
        const Monstre &m = jeu.monstres[i];
        if (m.actif && m.x == x && m.y == y) return i;
    }
    return -1;
}

// vérifier si case est praticable pour monstre
bool case_praticable_pour_monstre(const Jeu& jeu, int x, int y, int indexMonstre) {
    if (x < 0 || x >= jeu.carte.largeur || y < 0 || y >= jeu.carte.hauteur) return false;
    if (jeu.carte.cases[y][x] != '.') return false;

    for (int j = 0; j < jeu.nb_monstres; ++j) {
        if (j != indexMonstre &&
            jeu.monstres[j].actif &&
            jeu.monstres[j].stats[0] > 0 &&
            jeu.monstres[j].x == x &&
            jeu.monstres[j].y == y)
            return false;
    }
    return true;
}

// appliquer les effets d’un monstre
void appliquer_contraintes_monstre_sur_joueur(const Jeu& jeu, const Monstre& m, Joueur& joueur) {
    Config_monstre cfg;

    if (trouver_config_monstre_par_id(jeu, m.idConfig, cfg)) {
        for (int s = 0; s < NB_STATS; ++s) {
            int delta = cfg.stats_afflige[s];
            if (delta > 0) {
                joueur.stat[s] = max(0, joueur.stat[s] - delta);
            }
        }
    }
}

// attaque du joueur vers le monstre
void attaque(Jeu& jeu, int id_monstre) {
    Monstre &m = jeu.monstres[id_monstre];

    for (int i = 0; i < TAILLE_MONSTRES; i++) {
        Config_monstre &cfg = jeu.cfg_monstres[i];

        if (cfg.id == m.idConfig) {
            for (int s = 0; s < NB_STATS; s++) {
                if (cfg.stats_prit[s]) {
                    m.stats[s] -= jeu.joueur.stat[s];
                    if (m.stats[s] <= 0) m.actif = false;
                }
            }
        }
    }
}



// ---- CONTRAINTES & PORTES ----

// vérifier une contrainte (items + stats)
bool check_contrainte(Jeu& jeu, int id_contrainte, string& context, bool defaite = false) {
    int indexCtr = -1;

    // rechercher la contrainte
    for (int c = 0; c < jeu.cfgConditions.nbContraintes; c++) {
        if (jeu.cfgConditions.contraintes[c].id == id_contrainte) indexCtr = c;
    }

    if (indexCtr == -1) {
        context = "Contrainte inexistante.";
        return false;
    }

    const Contrainte& ctr = jeu.cfgConditions.contraintes[indexCtr];

    // MODE VICTOIRE/defaut
    if (!defaite) {
        // items requis
        for (int i = 0; i < ctr.nb_items_possede; i++) {
            int idItem = ctr.items_possede[i];

            if (!a_items(jeu, idItem)) {
                // trouver config pour nom, si existante
                Config_item cfg;
                if (trouver_config_item_par_id(jeu, idItem, cfg)) {
                    context = "Il manque l'item requis : " + cfg.nom + " [" + cfg.symbole + "]";
                } else {
                    context = "Item requis manquant (ID=" + to_string(idItem) + ").";
                }

                return false;
            }
        }

        // stats minimales
        for (int s = 0; s < NB_STATS; s++) {
            if (ctr.stats_min[s] > 0 && !a_stat(jeu, s, ctr.stats_min[s])) {
                context = "Statistique insuffisante : '" + jeu.nom_stats[s] +"' doit être au moins " + to_string(ctr.stats_min[s]) + ".";
                return false;
            }
        }

        // symbole
        if (ctr.a_symbole) {
            int x = jeu.joueur.x;
            int y = jeu.joueur.y;

            if (jeu.carte.ex_cases[y][x] != ctr.symbole_atteint) {
                context = string("Vous devez atteindre une case marquée '") + ctr.symbole_atteint + "'.";
                return false;
            }
        }

        return true;
    }

    // MODE DÉFAITE
    // (pas de context ici — tu ne l'utilisais pas)

    for (int i = 0; i < ctr.nb_items_possede; i++) {
        if (a_items(jeu, ctr.items_possede[i])) return true;
    }

    int nb_stat_valide = 0;
    for (int s = 0; s < NB_STATS; s++) {
        if (ctr.stats_min[s] >= jeu.joueur.stat[s]) nb_stat_valide++;
    }
    if (nb_stat_valide == NB_STATS) return true;

    if (ctr.a_symbole) {
        int x = jeu.joueur.x;
        int y = jeu.joueur.y;

        if (jeu.carte.ex_cases[y][x] == ctr.symbole_atteint) return true;
    }

    return false;
}


// vérifier si une porte peut être ouverte
bool porte_valide(Jeu& jeu, char c) {
    string& context = jeu.modal_txt;
    if (!(c >= '0' && c <= '9')) return true;

    for (int p = 0; p < jeu.nb_cfg_portes; p++) {
        const Config_porte& porte = jeu.cfg_portes[p];
        if (porte.symbole == c) {
            bool check = check_contrainte(jeu, porte.id_contrainte, context);
            if (check == false) jeu.modal_active = true;
            return check;
        }
    }

    context = "Porte inconnue.";
    jeu.modal_active = true;
    return false;
}

#endif
