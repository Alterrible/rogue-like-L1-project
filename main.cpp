#include <iostream>
using namespace std;
#pragma once
#include <string>

// --- Constantes ---
const int TAILLE_MAX = 1840;

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
    std::string description;
    int stats_min[6];
};

// --- Structures de configuration ---
struct Config_item {
    int id;
    char symbole;
    std::string nom;
    std::string description;
    int bonus[6];
};

struct Config_monstre {
    int id;
    char symbole;
    std::string nom;
    std::string description;
    int stats_base[6];
    int typeIA;
    int id_item_contrainte;
    int contrainte_stats[6];
    Liste_Id_Equipements inventaire_ids;  // Equipements du monstre
    int spawn_x;
    int spawn_y;
};

struct Config_porte {
    int id;
    char symbole;
    int id_item_contrainte;
    int contrainte_stats[6];
};

struct Config_joueur {
    int id;
    char symbole;
    std::string nom;
    std::string description;
    int stats[6];
    Liste_Id_Equipements inventaire_ids;
    bool actif;
    int spawn_x;
    int spawn_y;
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
    int stat[6];
};

struct Monstre {
    int x;
    int y;
    int idConfig;
    int stats[6];
    bool actif;
};

struct Carte {
    int largeur;
    int hauteur;
    char cases[23][80];
    bool visible[23][80];
    bool actif;
};

// À définir selon ton projet :
struct TABLE_CONFIG_SYMBOLES {
    // Placeholder — remplis selon ton besoin
};

// --- Structure principale du jeu ---
struct Jeu {
    Carte carte;

    Joueur joueur;
    Config_joueur cfg_joueur;

    Config_item cfg_items[26];
    int nb_cfg_items;

    Config_monstre cfg_monstres[26];
    int nb_cfg_monstres;

    Config_porte cfg_portes[10];
    int nb_cfg_portes;

    Config_conditions_jeu cfgConditions;

    TABLE_CONFIG_SYMBOLES cfgSymboles;

    Monstre monstres[TAILLE_MAX];
    int nb_monstres;

    bool etat_termine;
    bool victoire;
};

// --- Exemple de fonction de vérification des contraintes ---
bool verifier_contraintes(const Liste_id_contraintes& possede,
                          const Liste_id_contraintes& requis)
{
    if (possede.nb < requis.nb)
        return false;

    for (int i = 0; i < requis.nb; i++) {
        if (possede.ids[i] < requis.ids[i])
            return false;
    }
    return true;
}

int main() {
    Jeu jeu;
    bool ok;
    char cmd;

    // Charger la configuration générale
    ok = charger_configuration("config.txt", jeu);
    if (!ok) {
        cout << "Erreur : config.txt introuvable ou invalide" << endl;
    }

    // Charger carte
    ok = charger_carte("level.txt", jeu);
    if (!ok) {
        cout << "Erreur : level.txt introuvable ou invalide" << endl;
    }

    // Initialisation du jeu (joueur + monstres + brouillard)
    initialiser_jeu(jeu);

    // Boucle principale
    while (!jeu.etat_termine) {
        afficher_jeu(jeu);
        cmd = saisie_bloquante();

        traiter_commande(cmd, jeu);
        mettre_a_jour_monstres(jeu);
        mettre_a_jour_visibilite(jeu);
        verifier_conditions_victoire_defaite(jeu);
    }

    // Fin de jeu
    afficher_game_over(jeu);

    return 0;
}
