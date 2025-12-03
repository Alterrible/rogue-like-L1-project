using namespace std;
#include <iostream>
#include <string>
#include "enregistrement.hpp"
#include "init.hpp"
#include "affichage.hpp"
#include "maj.hpp"
#include "traiter_commande.hpp"

void mettre_a_jour_jeu(Jeu &jeu){
    mettre_a_jour_monstres(jeu);
    mettre_a_jour_visibilite(jeu);
    mettre_a_jour_carte(jeu);
    verifier_conditions_victoire_defaite(jeu);
}


int main() {
    static Jeu jeu;
    bool ok;
    char cmd;

    // Charger la configuration générale
    ok = charger_configuration("init.txt", jeu);
    if (!ok) {
        cout << "Erreur : config.txt introuvable ou invalide" << endl;
    }

    // Charger carte
    ok = charger_carte("level.txt", jeu);
    if (!ok) {
        cout << "Erreur : level.txt introuvable ou invalide" << endl;
    }

    initialiser_jeu(jeu);
    // afficher_jeu_debug(jeu);

    // Boucle principale
    while (!jeu.etat_termine) {
        afficher_jeu(jeu);
        cmd = saisie_bloquante();
        
        if (cmd == 27) { // ESC
            fermer_console();
            cout << "Sortie par ESC.\n";
            return 0;
        }

        traiter_commande(cmd, jeu);
        mettre_a_jour_jeu(jeu);
    }

    // Fin de jeu
    afficher_jeu(jeu);
    afficher_game_over(jeu);
    return 0;
}

// g++ main.cpp -o main.exe -lncurses
// g++ main.cpp -o main.exe -lncurses -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wundef -Winline -O2 -g -fsanitize=address,undefined -fstack-protector-strong 
