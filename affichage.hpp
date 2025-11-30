#ifndef AFFICHAGE_HPP
#define AFFICHAGE_HPP

#include "enregistrement.hpp"
#include "lib_projet_ncurses.hpp"
#include <iostream>
using namespace std;

void afficher_jeu(const Jeu &jeu) {
    effacer_console();

    // traitement pour chaques cases de la carte
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++) {
            if (jeu.carte.visible[y][x]) {
                char c = jeu.carte.cases[y][x];
                ecrire_char(x, y, c);
            } else {
                ecrire_char(x, y, ' ');
            }
        }
    }
    // joueur (pb : recouvre potentiellement la case)
    ecrire_char(jeu.joueur.x, jeu.joueur.y, jeu.cfg_joueur.symbole);

    // les monstres actifs
    for (int i = 0; i < jeu.nb_monstres; i++) {
        if (jeu.monstres[i].actif) {

            char symbole = '?'; // symbole du monstre depuis configuration donc valeur par défaut = ?
            for (int j = 0; j < jeu.nb_cfg_monstres; j++) {
                if (jeu.cfg_monstres[j].id == jeu.monstres[i].idConfig) {
                    symbole = jeu.cfg_monstres[j].symbole;
                    break;
                }
            }
            ecrire_char(jeu.monstres[i].x, jeu.monstres[i].y, symbole);
        }
    }

    // --- hotbar ---
    string hotbar = "Stats : ";
    for (int i = 0; i < NB_STATS; i++) {
        hotbar += " [" + to_string(jeu.joueur.stat[i]) + "]";
    }
    ecrire_string(hotbar, 0, jeu.carte.hauteur);
}

// écran de fin de jeu
void afficher_game_over(const Jeu &jeu) {
    effacer_console();

    string msg;
    if (jeu.victoire) {
        msg = "gagné";
    } else {
        msg = "perdu";
    }

    ecrire_string(msg, 0, 0);
}


// ---- utilitaire pour affichage de debug ----
void afficher_liste_id_contraintes(const Liste_id_contraintes& liste, const string& titre) {
    cout << titre << " (nb = " << liste.nb << ")" << endl;
    for (int i = 0; i < liste.nb; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << endl;
    }
}

void afficher_liste_equipements(const Liste_Id_Equipements& liste, const string& titre) {
    cout << titre << " (taille = " << liste.taille << ")" << endl;
    for (int i = 0; i < liste.taille; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << endl;
    }
}

void afficher_contrainte(const Contrainte& c) {
    cout << "id : " << c.id << endl;
    cout << "description : " << c.description << endl;
    for (int i = 0; i < NB_STATS; i++)
        cout << "stats_min[" << i << "] : " << c.stats_min[i] << endl;
}

// affichage de debugage
void afficher_jeu_debug(const Jeu& jeu) {

    cout << "[DEBUG]" << endl;

    cout << "CONFIG JOUEUR" << endl;
    cout << "symbole : " << jeu.cfg_joueur.symbole << endl;
    cout << "nom : " << jeu.cfg_joueur.nom << endl;
    cout << "description : " << jeu.cfg_joueur.description << endl;
    cout << "actif : " << jeu.cfg_joueur.actif << endl;
    cout << "spawn_x : " << jeu.cfg_joueur.spawn_x << endl;
    cout << "spawn_y : " << jeu.cfg_joueur.spawn_y << endl;

    for (int i = 0; i < NB_STATS; i++)
        cout << "stats[" << i << "] : " << jeu.cfg_joueur.stats[i] << endl;

    afficher_liste_equipements(jeu.cfg_joueur.inventaire_ids, "Inventaire joueur");

    cout << "CONFIG ITEMS" << endl;
    for (int i = 0; i < jeu.nb_cfg_items; i++) {
        cout << "Item " << i << endl;
        cout << "id : " << jeu.cfg_items[i].id << endl;
        cout << "symbole : " << jeu.cfg_items[i].symbole << endl;
        cout << "nom : " << jeu.cfg_items[i].nom << endl;
        cout << "description : " << jeu.cfg_items[i].description << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "bonus[" << j << "] : " << jeu.cfg_items[i].bonus[j] << endl;
    }

    cout << "CONFIG MONSTRES" << endl;
    for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
        const auto& m = jeu.cfg_monstres[i];
        cout << "Monstre " << i << endl;
        cout << "id : " << m.id << endl;
        cout << "symbole : " << m.symbole << endl;
        cout << "nom : " << m.nom << endl;
        cout << "description : " << m.description << endl;
        cout << "typeIA : " << m.typeIA << endl;
        cout << "id_item_contrainte : " << m.id_item_contrainte << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "stats_base[" << j << "] : " << m.stats_base[j] << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "contrainte_stats[" << j << "] : " << m.contrainte_stats[j] << endl;

        afficher_liste_equipements(m.inventaire_ids, "Inventaire monstre");
    }

    cout << "CONFIG PORTES" << endl;
    for (int i = 0; i < jeu.nb_cfg_portes; i++) {
        const auto& p = jeu.cfg_portes[i];
        cout << "Porte " << i << endl;
        cout << "id : " << p.id << endl;
        cout << "symbole : " << p.symbole << endl;
        cout << "id_item_contrainte : " << p.id_item_contrainte << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "contrainte_stats[" << j << "] : " << p.contrainte_stats[j] << endl;
    }

    cout << "CONDITIONS DE JEU" << endl;
    cout << "nbContraintes : " << jeu.cfgConditions.nbContraintes << endl;

    cout << "Contraintes" << endl;
    for (int i = 0; i < jeu.cfgConditions.nbContraintes; i++) {
        cout << "Contrainte " << i << endl;
        afficher_contrainte(jeu.cfgConditions.contraintes[i]);
        cout << "---" << endl;
    }

    afficher_liste_id_contraintes(jeu.cfgConditions.victoire, "\nConditions victoire");
    afficher_liste_id_contraintes(jeu.cfgConditions.defaite, "\nConditions défaite");

    cout << "ITEMS DANS LA CARTE" << endl;
    cout << "nb_items : " << jeu.nb_items << endl;
    for (int i = 0; i < jeu.nb_items; i++) {
        const auto& it = jeu.items[i];
        cout << "Item " << i << " : id=" << it.id 
             << " x=" << it.x << " y=" << it.y 
             << " idConfig=" << it.idConfig << endl;
    }

    cout << "MONSTRES DANS LA CARTE" << endl;
    cout << "nb_monstres : " << jeu.nb_monstres << endl;
    for (int i = 0; i < jeu.nb_monstres; i++) {
        const auto& mo = jeu.monstres[i];
        cout << "Monstre " << i << " : idConfig=" << mo.idConfig
             << " x=" << mo.x << " y=" << mo.y 
             << " actif=" << mo.actif << endl;
        for (int j = 0; j < NB_STATS; j++)
            cout << "  stats[" << j << "] : " << mo.stats[j] << endl;
    }

    cout << "CARTE" << endl;
    cout << "largeur : " << jeu.carte.largeur << endl;
    cout << "hauteur : " << jeu.carte.hauteur << endl;

    cout << "Cases" << endl;
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << jeu.carte.cases[y][x];
        cout << endl;
    }

    cout << "Visibilité" << endl;
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << (jeu.carte.visible[y][x] ? "1" : "0");
        cout << endl;
    }

    cout << "JOUEUR (DYNAMIQUE)" << endl;
    cout << "x : " << jeu.joueur.x << endl;
    cout << "y : " << jeu.joueur.y << endl;
    cout << "nb_inventaire : " << jeu.joueur.nb_inventaire << endl;

    for (int i = 0; i < jeu.joueur.nb_inventaire; i++)
        cout << "inventaire[" << i << "] : " << jeu.joueur.inventaire[i] << endl;

    for (int i = 0; i < NB_STATS; i++)
        cout << "stat[" << i << "] : " << jeu.joueur.stat[i] << endl;

    cout << "ETAT DU JEU" << endl;
    cout << "etat_termine : " << jeu.etat_termine << endl;
    cout << "victoire : " << jeu.victoire << endl;

    cout << "[FIN DEBUG]" << endl;
}

#endif