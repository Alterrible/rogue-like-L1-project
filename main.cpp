using namespace std;
#include <iostream>
#include <string>
#include "enregistrement.hpp"
#include "init.hpp"

void afficher_jeu(const Jeu& jeu){
    cout << "[DEBUG]" << endl;
    cout << "items" << endl;
    for (int i = 0; i < jeu.nb_cfg_items; i++) {
        const Config_item& it = jeu.cfg_items[i];
        cout << it.id << " " << it.symbole << " " << it.nom << " ";
        for (int k = 0; k < 6; k++) cout << it.bonus[k] << " ";
        cout << endl;
    }

    cout << "monstres config" << endl;
    for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
        const Config_monstre& m = jeu.cfg_monstres[i];
        cout << m.id << " " << m.symbole << " " << m.nom << " ";
        for (int k = 0; k < 6; k++) cout << m.stats_base[k] << " ";
        cout << endl;
    }

    cout << "portes" << endl;
    for (int i = 0; i < jeu.nb_cfg_portes; i++) {
        const Config_porte& p = jeu.cfg_portes[i];
        cout << p.id << " " << p.symbole << " ";
        for (int k = 0; k < 6; k++) cout << p.contrainte_stats[k] << " ";
        cout << endl;
    }

    cout << "joueur config" << endl;
    cout << "nom " << jeu.cfg_joueur.nom << endl;
    cout << "stats ";
    for (int i = 0; i < 6; i++) cout << jeu.cfg_joueur.stats[i] << " ";
    cout << endl;
    cout << "spawn " << jeu.cfg_joueur.spawn_x << " " << jeu.cfg_joueur.spawn_y << endl;

    cout << "contraintes" << endl;
    for (int i = 0; i < jeu.cfgConditions.nbContraintes; i++) {
        const Contrainte& c = jeu.cfgConditions.contraintes[i];
        cout << c.id << " ";
        for (int k = 0; k < 6; k++) cout << c.stats_min[k] << " ";
        cout << endl;
    }

    cout << "victoire" << endl;
    for (int i = 0; i < jeu.cfgConditions.victoire.nb; i++)
        cout << jeu.cfgConditions.victoire.ids[i] << " ";
    cout << endl;

    cout << "defaite" << endl;
    for (int i = 0; i < jeu.cfgConditions.defaite.nb; i++)
        cout << jeu.cfgConditions.defaite.ids[i] << " ";
    cout << endl;

    cout << "carte" << endl;
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << jeu.carte.cases[y][x];
        cout << endl;
    }

    cout << "monstres carte" << endl;
    for (int i = 0; i < jeu.nb_monstres; i++) {
        const Monstre& m = jeu.monstres[i];
        cout << m.idConfig << " " << m.x << " " << m.y << endl;
    }

    cout << "joueur" << endl;
    cout << jeu.joueur.x << " " << jeu.joueur.y << endl;
}

int main() {
    Jeu jeu;
    bool ok;
    // char cmd;

    // Charger la configuration générale
    ok = charger_configuration("config.txt", jeu);
    if (!ok) {
        cout << "Erreur : config.txt introuvable ou invalide" << endl;
    }

    afficher_jeu(jeu);

    // // Charger carte
    // ok = charger_carte("level.txt", jeu);
    // if (!ok) {
    //     cout << "Erreur : level.txt introuvable ou invalide" << endl;
    // }

    // // Initialisation du jeu (joueur + monstres + brouillard)
    // initialiser_jeu(jeu);

    // // Boucle principale
    // while (!jeu.etat_termine) {
    //     afficher_jeu(jeu);
    //     cmd = saisie_bloquante();

    //     traiter_commande(cmd, jeu);
    //     mettre_a_jour_monstres(jeu);
    //     mettre_a_jour_visibilite(jeu);
    //     verifier_conditions_victoire_defaite(jeu);
    // }

    // // Fin de jeu
    // afficher_game_over(jeu);

    return 0;
}
