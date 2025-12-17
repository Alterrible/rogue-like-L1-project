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
    int tout_valide; // 1 tout nécessaire, 2 une section nécessaire, 3 une valeur nécessaire
    int items_possede[TAILLE_MAX];
    int nb_items_possede;
    int stats_min[NB_STATS];
    bool stat_effective[NB_STATS]; // n'est utile qu'exclusivement pour les "combats"
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
    bool conso;
};

struct Config_monstre {
    int id;
    char symbole;
    string nom;
    string description;
    int typeIA;
    int stats_base[NB_STATS];
    int id_contrainte;
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
    bool utilise;
};

struct Inventaire_item {
    int id_config;
    int total;
    int utilises;
    int restants;
};

struct Carte {
    int largeur;
    int hauteur;
    char cases[TAILLE_MAP_Y][TAILLE_MAP_X];
    char ex_cases[TAILLE_MAP_Y][TAILLE_MAP_X];
    bool visible[TAILLE_MAP_Y][TAILLE_MAP_X];
    char ex_visible[TAILLE_MAP_Y][TAILLE_MAP_X];
};

struct Combat {
    bool actif;
    int id_monstre;
    bool tour_joueur;
};

// --- Structure principale du jeu ---
struct Jeu {
    // configuration
    Config_joueur cfg_joueur;
    Config_item cfg_items[TAILLE_ITEMS];
    Config_monstre cfg_monstres[TAILLE_MONSTRES];
    Config_porte cfg_portes[TAILLE_PORTE];
    Config_conditions_jeu cfgConditions;

    // données de jeu 
    Carte carte;
    Joueur joueur;

    Items items[TAILLE_MAX];
    int nb_items;

    Monstre monstres[TAILLE_MAX];
    int nb_monstres;

    string nom_stats[NB_STATS];
    int index_stat_vision;

    // état contrainte fain de jeu
    bool etat_termine;
    bool victoire;

    Combat combat;

    // état modal
    string modal_txt;
    bool modal_active;

    // état de l'inventaire
    Inventaire_item inventaire_items[TAILLE_ITEMS];
    int nb_inventaire_items;
    bool inventaire_actif;
    int inv_selection_index;
    int inv_scroll_haut;

    // compteurs pour éviter débordement lors de l'initialisation également lors ce que l'on a besoin de connaître le nombres d'éléments
    int nb_cfg_items;
    int nb_cfg_monstres;
    int nb_cfg_portes;
};

#endif