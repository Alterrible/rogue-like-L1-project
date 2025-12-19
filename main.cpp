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
    static Jeu jeu {};
    bool ok;
    char cmd;
    bool bvn = true;

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

    // Boucle principale
    while (true) {
        if (bvn) {
            afficher_ecran_bienvenue(jeu);
        }
        else if (!jeu.etat_termine && !jeu.modal_active) {
            if (jeu.inventaire_actif) {
                afficher_inventaire(jeu);
            } else {
                afficher_jeu(jeu);
            }
        } else if (jeu.modal_active) {
            afficher_modal(jeu);
        } else {
            afficher_game_over(jeu);
        }

        int c = saisie_bloquante();
        cmd = static_cast<char>(c);

        if (cmd == 27) { // échap
            fermer_console();
            cout << "Jeu fermé correctement." << endl;
            return 0;
        }

        bool traitement = traiter_commande(cmd, jeu, bvn);
        if (traitement) mettre_a_jour_jeu(jeu);
    }

    return 0;
}
