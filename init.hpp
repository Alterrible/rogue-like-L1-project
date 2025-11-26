#include <iostream>
#include <fstream>
using namespace std;

bool charger_configuration(const string& fichier){
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    string ligne;
    while (getline(flux, ligne)) {
        for (int c ; c <= ligne.length() ; c++){
            // ...
        }
    }

    flux.close();
    return true;
}

bool charger_carte(const string& fichier, Jeu& jeu){
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    string ligne;
    while (getline(flux, ligne)) {
        for (int c ; c <= ligne.length() ; c++){
            if (c == 35){ // #
                
            } else if (c == 64){ // @
    
            } else if (c == 36){ // $
    
            } else if (c >= 48 and c<=57){ // 0 à 9
                
            } else if (c >= 65 and c<=90){ // A à Z
                
            } else if (c >= 97 and c<=122){ // a à z
                
            }
        }
    }

    flux.close();
    return true;
}
