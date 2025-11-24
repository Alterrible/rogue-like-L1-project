#include <iostream>
#include <fstream>
using namespace std;

bool charger_configuration(const string& fichier, Jeu& jeu){
    fstream flux;

    flux.open(fichier, ios::in);
    if (flux.is_open()){
        cout << flux;
        while (flux.good()){
            cout << flux;
        }
        flux.close();
    }
    else{
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }
    return total;
}

bool charger_carte(const string& fichier, Jeu& jeu){
    fstream flux;

    flux.open(fichier, ios::in);
    if (flux.is_open()){
        cout << flux;
        while (flux.good()){
            cout << flux;
        }
        flux.close();
    }
    else{
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }
    return total;
}