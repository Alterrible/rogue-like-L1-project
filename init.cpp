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
        cout << ligne << endl;
    }

    flux.close();
    return true;
}

bool charger_carte(const string& fichier){
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }

    string ligne;
    while (getline(flux, ligne)) {
        cout << ligne << endl;
    }

    flux.close();
    return true;
}

int main(){
    charger_carte("level.txt");
}