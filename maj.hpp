#ifndef MAJ_HPP
#define MAJ_HPP

#include "enregistrement.hpp"
#include <iostream>
using namespace std;
// dans les algo suivant on suppose que c'est stat[0] qui est la vie donc on joue dessus pour la mort

// système d'ia
void mettre_a_jour_monstres(Jeu &jeu) {
    // tous les monstres actif se déplace vers le joueur
    for (int i = 0; i < jeu.nb_monstres; i++) {        
        int mx = jeu.monstres[i].x;
        int my = jeu.monstres[i].y;

        // calcule direction vers le joueur si le monstre est à gauche → il va vers la droite et idem pour les autres directions
        int dx = 0;
        int dy = 0;
    
        if (mx < jeu.joueur.x) dx = 1;
        else if (mx > jeu.joueur.x) dx = -1;
        if (my < jeu.joueur.y) dy = 1;
        else if (my > jeu.joueur.y) dy = -1;
        int nx = mx + dx;
        int ny = my + dy;

        if (nx >= 0 && nx < jeu.carte.largeur && ny >= 0 && ny < jeu.carte.hauteur) {
            if (jeu.carte.cases[ny][nx] != '#') {
                if (nx == jeu.joueur.x && ny == jeu.joueur.y) {
                    if (jeu.joueur.stat[0] > 0) {
                        jeu.joueur.stat[0]--;
                    }
                } else {
                    jeu.monstres[i].x = nx;
                    jeu.monstres[i].y = ny;
                }
            }
        }
    }
}
// BUG REPPORT: les monstres passent dans les murs et comme ils sont actifs tout le temps il n'y a pas de mécanisme d'activation random quand ils sont loins

void mettre_a_jour_visibilite(Jeu &jeu) {
}

void verifier_conditions_victoire_defaite(Jeu &jeu) {
    // cas de la mort
    if (jeu.joueur.stat[0] <= 0) {
        jeu.etat_termine = true;
        jeu.victoire = false;
        return;
    }
}

void mettre_a_jour_jeu(Jeu &jeu){
    mettre_a_jour_monstres(jeu);
    mettre_a_jour_visibilite(jeu);
    verifier_conditions_victoire_defaite(jeu);

    // mettre à jour les données de joueurs : 
    // - recalcule de stats suivant les items possédé
    // - calcule des contraintes apporté par un monstre par exemple
    // - calcule de l'état de la carte
}

#endif