#include <iostream>
using namespace std;

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
