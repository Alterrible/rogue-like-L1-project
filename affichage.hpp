#ifndef AFFICHAGE_HPP
#define AFFICHAGE_HPP

#include "enregistrement.hpp"
#include <iostream>
using namespace std;

void afficher_liste_id_contraintes(const Liste_id_contraintes& liste, const string& titre) {
    cout << titre << " (nb = " << liste.nb << ")\n";
    for (int i = 0; i < liste.nb; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << "\n";
    }
}

void afficher_liste_equipements(const Liste_Id_Equipements& liste, const string& titre) {
    cout << titre << " (taille = " << liste.taille << ")\n";
    for (int i = 0; i < liste.taille; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << "\n";
    }
}

void afficher_contrainte(const Contrainte& c) {
    cout << "id : " << c.id << "\n";
    cout << "description : " << c.description << "\n";
    for (int i = 0; i < NB_STATS; i++)
        cout << "stats_min[" << i << "] : " << c.stats_min[i] << "\n";
}

void afficher_jeu(const Jeu& jeu) {

    cout << "[DEBUG]\n";

    // ------------------------------------------------------------
    cout << "\n=== CONFIG JOUEUR ===\n";
    cout << "id : " << jeu.cfg_joueur.id << "\n";
    cout << "symbole : " << jeu.cfg_joueur.symbole << "\n";
    cout << "nom : " << jeu.cfg_joueur.nom << "\n";
    cout << "description : " << jeu.cfg_joueur.description << "\n";
    cout << "actif : " << jeu.cfg_joueur.actif << "\n";
    cout << "spawn_x : " << jeu.cfg_joueur.spawn_x << "\n";
    cout << "spawn_y : " << jeu.cfg_joueur.spawn_y << "\n";

    for (int i = 0; i < NB_STATS; i++)
        cout << "stats[" << i << "] : " << jeu.cfg_joueur.stats[i] << "\n";

    afficher_liste_equipements(jeu.cfg_joueur.inventaire_ids, "Inventaire joueur");

    // ------------------------------------------------------------
    cout << "\n=== CONFIG ITEMS ===\n";
    for (int i = 0; i < jeu.nb_cfg_items; i++) {
        cout << "\n-- Item " << i << " --\n";
        cout << "id : " << jeu.cfg_items[i].id << "\n";
        cout << "symbole : " << jeu.cfg_items[i].symbole << "\n";
        cout << "nom : " << jeu.cfg_items[i].nom << "\n";
        cout << "description : " << jeu.cfg_items[i].description << "\n";

        for (int j = 0; j < NB_STATS; j++)
            cout << "bonus[" << j << "] : " << jeu.cfg_items[i].bonus[j] << "\n";
    }

    // ------------------------------------------------------------
    cout << "\n=== CONFIG MONSTRES ===\n";
    for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
        const auto& m = jeu.cfg_monstres[i];
        cout << "\n-- Monstre " << i << " --\n";
        cout << "id : " << m.id << "\n";
        cout << "symbole : " << m.symbole << "\n";
        cout << "nom : " << m.nom << "\n";
        cout << "description : " << m.description << "\n";
        cout << "typeIA : " << m.typeIA << "\n";
        cout << "id_item_contrainte : " << m.id_item_contrainte << "\n";

        for (int j = 0; j < NB_STATS; j++)
            cout << "stats_base[" << j << "] : " << m.stats_base[j] << "\n";

        for (int j = 0; j < NB_STATS; j++)
            cout << "contrainte_stats[" << j << "] : " << m.contrainte_stats[j] << "\n";

        afficher_liste_equipements(m.inventaire_ids, "Inventaire monstre");
    }

    // ------------------------------------------------------------
    cout << "\n=== CONFIG PORTES ===\n";
    for (int i = 0; i < jeu.nb_cfg_portes; i++) {
        const auto& p = jeu.cfg_portes[i];
        cout << "\n-- Porte " << i << " --\n";
        cout << "id : " << p.id << "\n";
        cout << "symbole : " << p.symbole << "\n";
        cout << "id_item_contrainte : " << p.id_item_contrainte << "\n";

        for (int j = 0; j < NB_STATS; j++)
            cout << "contrainte_stats[" << j << "] : " << p.contrainte_stats[j] << "\n";
    }

    // ------------------------------------------------------------
    cout << "\n=== CONDITIONS DE JEU ===\n";
    cout << "nbContraintes : " << jeu.cfgConditions.nbContraintes << "\n";

    cout << "\n-- Contraintes --\n";
    for (int i = 0; i < jeu.cfgConditions.nbContraintes; i++) {
        cout << "Contrainte " << i << "\n";
        afficher_contrainte(jeu.cfgConditions.contraintes[i]);
        cout << "---\n";
    }

    afficher_liste_id_contraintes(jeu.cfgConditions.victoire, "\nConditions victoire");
    afficher_liste_id_contraintes(jeu.cfgConditions.defaite, "\nConditions défaite");

    // ------------------------------------------------------------
    cout << "\n=== ITEMS DANS LA CARTE ===\n";
    cout << "nb_items : " << jeu.nb_items << "\n";
    for (int i = 0; i < jeu.nb_items; i++) {
        const auto& it = jeu.items[i];
        cout << "Item " << i << " : id=" << it.id 
             << " x=" << it.x << " y=" << it.y 
             << " idConfig=" << it.idConfig << "\n";
    }

    // ------------------------------------------------------------
    cout << "\n=== MONSTRES DANS LA CARTE ===\n";
    cout << "nb_monstres : " << jeu.nb_monstres << "\n";
    for (int i = 0; i < jeu.nb_monstres; i++) {
        const auto& mo = jeu.monstres[i];
        cout << "Monstre " << i << " : idConfig=" << mo.idConfig
             << " x=" << mo.x << " y=" << mo.y 
             << " actif=" << mo.actif << "\n";
        for (int j = 0; j < NB_STATS; j++)
            cout << "  stats[" << j << "] : " << mo.stats[j] << "\n";
    }

    // ------------------------------------------------------------
    cout << "\n=== CARTE ===\n";
    cout << "largeur : " << jeu.carte.largeur << "\n";
    cout << "hauteur : " << jeu.carte.hauteur << "\n";
    cout << "actif : " << jeu.carte.actif << "\n";

    cout << "\n-- Cases --\n";
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << jeu.carte.cases[y][x];
        cout << "\n";
    }

    cout << "\n-- Visibilité --\n";
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << (jeu.carte.visible[y][x] ? "1" : "0");
        cout << "\n";
    }

    // ------------------------------------------------------------
    cout << "\n=== JOUEUR (DYNAMIQUE) ===\n";
    cout << "x : " << jeu.joueur.x << "\n";
    cout << "y : " << jeu.joueur.y << "\n";
    cout << "nb_inventaire : " << jeu.joueur.nb_inventaire << "\n";

    for (int i = 0; i < jeu.joueur.nb_inventaire; i++)
        cout << "inventaire[" << i << "] : " << jeu.joueur.inventaire[i] << "\n";

    for (int i = 0; i < NB_STATS; i++)
        cout << "stat[" << i << "] : " << jeu.joueur.stat[i] << "\n";

    // ------------------------------------------------------------
    cout << "\n=== ETAT DU JEU ===\n";
    cout << "etat_termine : " << jeu.etat_termine << "\n";
    cout << "victoire : " << jeu.victoire << "\n";

    cout << "\n[FIN DEBUG]\n";
}

#endif