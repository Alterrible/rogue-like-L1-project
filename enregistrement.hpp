#ifndef ENREGISTREMENT_HPP
#define ENREGISTREMENT_HPP

#include <string>

// --- Constantes ---
const int TAILLE_MAP_X = 80;
const int TAILLE_MAP_Y = 23;
const int TAILLE_MAX = TAILLE_MAP_X*TAILLE_MAP_Y;
const int TAILLE_ITEMS = 26;
const int TAILLE_MONSTRES = 26;
const int RAYON_ZONE_BASE = 6;
const int TAILLE_PORTE = 26;
const int NB_STATS = 6;

// --- Structures de listes génériques ---
struct Liste_id_contraintes {
    int nb;                           // Nombre d’identifiants
    int ids[TAILLE_MAX];              // Identifiants
};

struct Liste_Id_Equipements {
    int taille;                       // Nombre d’éléments
    int ids[TAILLE_MAX];              // Identifiants d’équipements
};

// --- Structure Contrainte ---
struct Contrainte {
    int id;
    int items_possede[TAILLE_MAX];
    int nb_items_possede;
    int stats_min[NB_STATS];
    bool a_symbole;
    char symbole_atteint;
};

// --- Structures de configuration ---
struct Config_item {
    int id;
    char symbole;
    string nom;
    string description;
    int bonus[NB_STATS];
};

struct Config_monstre {
    int id;
    char symbole;
    string nom;
    string description;
    int typeIA;
    int stats_base[NB_STATS];
    int stats_afflige[NB_STATS];  // représente les dégâts qui vont être affligé (nombre positif passé en négatif au traitement)
    bool stats_prit[NB_STATS];  // représente les stats qui joue sur lui
};

struct Config_porte {
    int id;
    char symbole;
    int id_contrainte;
};

struct Config_joueur {
    char symbole;
    string nom;
    string description;
    int stats[NB_STATS];
    Liste_Id_Equipements inventaire_ids;
};

struct Config_conditions_jeu {
    Contrainte contraintes[TAILLE_MAX];   // Toutes les contraintes possibles
    int nbContraintes;
    Liste_id_contraintes victoire;
    Liste_id_contraintes defaite;
};

// --- Structures d’entités dynamiques ---
struct Joueur {
    int x;
    int y;
    int inventaire[TAILLE_MAX];
    int nb_inventaire;
    int stat[NB_STATS];
};

struct Monstre {
    int x;
    int y;
    int idConfig;
    int stats[6];
    bool actif;
};

struct Items {
    int id;
    int x;
    int y;
    int idConfig;
    bool actif;
};

struct Carte {
    int largeur;
    int hauteur;
    char cases[TAILLE_MAP_Y][TAILLE_MAP_X];
    char ex_cases[TAILLE_MAP_Y][TAILLE_MAP_X];
    bool visible[TAILLE_MAP_Y][TAILLE_MAP_X];
};

// --- Structure principale du jeu ---
struct Jeu {
    Carte carte;
    Joueur joueur;

    Config_joueur cfg_joueur;
    Config_item cfg_items[TAILLE_ITEMS];
    Config_monstre cfg_monstres[TAILLE_MONSTRES];
    Config_porte cfg_portes[TAILLE_PORTE];
    Config_conditions_jeu cfgConditions;

    Items items[TAILLE_MAX];
    int nb_items;

    Monstre monstres[TAILLE_MAX];
    int nb_monstres;

    string nom_stats[NB_STATS];
    int index_stat_vision;
    bool etat_termine;
    bool victoire;
    string info;
    string modal_txt;
    bool modal_active;

    // compteurs pour éviter débordement lors de l'initialisation
    int nb_cfg_items;
    int nb_cfg_monstres;
    int nb_cfg_portes;
};

#endif
