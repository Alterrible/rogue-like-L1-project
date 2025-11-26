
traiter_commande


include <iostream>
using namespace std;

void traiterCommande(char cmd, Jeu &jeu) {

    if (cmd == 'z') {                 // Avancer (haut)
        jeu.joueur.y -= 1;
    }
    else if (cmd == 's') {            // Reculer (bas)
        jeu.joueur.y += 1;
    }
    else if (cmd == 'q') {            // Aller à gauche
        jeu.joueur.x -= 1;
    }
    else if (cmd == 'd') {            // Aller à droite
        jeu.joueur.x += 1;
    }
    else if (cmd == 'i') {            // Inventaire
        cout << "Inventaire :" << endl;
        if (jeu.joueur.nbinventaire == 0) {
            cout << "  (vide)" << endl;
        } else {
            for (int i = 0; i < jeu.joueur.nbinventaire; i++) {
                int idItem = jeu.joueur.inventaire[i];
                cout << "  Slot " << i << " : item " << idItem << endl;
            }
        }
    }
    else if (cmd == 'm') {            // Carte (info simple)
        cout << "Position du joueur : (" 
             << jeu.joueur.x << ", " << jeu.joueur.y << ")" << endl;
        cout << "Taille carte : " 
             << jeu.carte.largeur << " x " << jeu.carte.hauteur << endl;
    }
    else if (cmd == 'f') {            // Frapper l'adversaire (version basique)
        cout << "Vous frappez devant vous !" << endl;

        // Exemple très simple : on cherche un monstre sur la case juste au-dessus
        int cibleX = jeu.joueur.x;
        int cibleY = jeu.joueur.y - 1;

        bool touche = false;
        for (int i = 0; i < jeu.nbmonstres; i++) {
            if (jeu.monstres[i].actif &&
                jeu.monstres[i].x == cibleX &&
                jeu.monstres[i].y == cibleY) {

                // On enlève 1 PV au monstre (stat[0])
                jeu.monstres[i].stats[0] -= 1;
                cout << "Vous touchez un monstre !" << endl;

                // Si le monstre n'a plus de PV, on le désactive
                if (jeu.monstres[i].stats[0] <= 0) {
                    jeu.monstres[i].actif = false;
                    cout << "Le monstre est vaincu." << endl;
                }
                touche = true;
                break;
            }
        }
        if (!touche) {
            cout << "Il n'y a aucun monstre à portée." << endl;
        }
    }
    else if (cmd == 'h') {            // Se soigner
        cout << "Vous essayez de vous soigner." << endl;

        // Version simple : +2 PV, sans dépasser le max
        int pvActuels = jeu.joueur.stat[0];
        int pvMax     = jeu.joueur.stat[1];

        if (pvActuels >= pvMax) {
            cout << "Vous avez déjà tous vos PV." << endl;
        } else {
            jeu.joueur.stat[0] = pvActuels + 2;
            if (jeu.joueur.stat[0] > pvMax) {
                jeu.joueur.stat[0] = pvMax;
            }
            cout << "Vous regagnez des PV. PV = " 
                 << jeu.joueur.stat[0] << "/" << pvMax << endl;
        }
    }
    else if (cmd == 'e') {            // Manger
        cout << "Vous essayez de manger un objet." << endl;

        if (jeu.joueur.nbinventaire == 0) {
            cout << "Votre inventaire est vide." << endl;
        } else {
            // On consomme le dernier objet de l'inventaire
            int idItem = jeu.joueur.inventaire[jeu.joueur.nbinventaire - 1];
            cout << "Vous mangez l'objet " << idItem << "." << endl;

            // Petit bonus : +1 PV
            int pvActuels = jeu.joueur.stat[0];
            int pvMax     = jeu.joueur.stat[1];
            if (pvActuels < pvMax) {
                jeu.joueur.stat[0]++;
            }

            // Retirer l'objet du tableau
            jeu.joueur.nbinventaire--;
        }
    }
    else {
        cout << "Commande inconnue." << endl;
    }
}
