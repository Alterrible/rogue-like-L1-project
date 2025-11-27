#ifndef INIT_HPP
#define INIT_HPP

using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include "enregistrement.hpp"

bool charger_configuration(const string& fichier){
    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
        return false;
    }
    
    // Remise à zéro des structures de config
    // jeu.nb_cfg_items = 0;
    // jeu.nb_cfg_monstres = 0;
    // jeu.nb_cfg_portes = 0;
    // jeu.cfgConditions.nbContraintes = 0;
    // jeu.cfgConditions.victoire.nb = 0;
    // jeu.cfgConditions.defaite.nb = 0;

    int section = 0;
    string ligne;
    while (getline(flux, ligne)) {
        string mot;
        for (int i = 0; i <= ligne.size(); ++i) {
            if (i == ligne.size() || ligne[i] == ' ') {
                if (!mot.empty()) {
                    if (ligne[0] == '['){
                        section++;
                    }
                // items
                    mot.clear();
                }
            } else {
                mot += ligne[i];
            }
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
