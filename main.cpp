#include <iostream>
using namespace std;
#include <iostream>

struct Coord{
    int x, y;
};

struct Porte{
    Coord coordonnee;
    int id_pre_requis;
};

struct Monstre{
    Coord coordonnee;
    int pv;
    int degats;
    int vision;
    int xp;
};

struct Item{
    bool est_sur_carte;
    Coord coordonnee;
    string nom;
    string description;
    int stat_item[5];
};

struct Inventaire{
    Item inv[10];
    Item armure[3];
    Item main[3];
};

struct Statistiques{
    int pv;
    int armure;
    int force;
    int vitesse;
    int vision;
    int niveau;
};

struct Joueur{
    Coord coordonnee;
    Inventaire inventaire;
    Statistiques stats;
};

struct Carte{
    Joueur joueur;
    Coord murs_liste[1840];
    Porte portes_liste[10];
    Monstre monstres_liste[26];
    Item items_liste[26];
}

void charger_fichiers(const string & level_fichier, const string & config_fichier, Carte & carte, Joueur & joueur){
    struct config_Item{
        string nom;
        string description;
        int stat_item[5];
    };
    struct config_Item{

    };
    struct config {
        Item
    }

    ifstream flux_config;
    flux_config.open(config_fichier, ios::in);

    if (flux_config.is_open()) {
        string ligne;

        while (getline(flux_config, ligne)) {
            bool est_guillemet = false;
            int indice = 0;
            int indice_liste;
            string tempMot = "";

            for (int i = 0; i < ligne.size(); i++) {
                char c = ligne[i];

                else if (i == 0 && (c >= 'a' && c <= 'z')) {
                    carte.items_liste[indice_liste];
                    indice++;
                }
                else if (i == 0 && (c >= 'A' && c <= 'Z')) {
                    carte.items_liste[indice_liste];
                    indice++;
                }
                else if (i == 0 && (c >= '0' && c <= '9')) {
                    carte.items_liste[indice_liste];
                    indice++;
                }else {
                    if (est_guillemet) {
                        tempMot
                    }
                }
            }
        }
        flux_config.close();
    }
    else {
        cout << "Erreur : impossible d'ouvrir " << config_fichier << endl;
    }

    flux_config.close();

    ifstream flux_level;
    flux_level.open(level_fichier, ios::in);
    int x = 0;
    int y = 0;
    if (flux_config.is_open()) {
        string ligne;

        while (getline(flux_config, ligne)) {
            for (int i = 0; i < ligne.size(); i++) {
                char c = ligne[i];
        }
    }
    flux_level.close();
}

int main(){
    static const string level_fichier = "level.txt";
    static const string config_fichier = "config.txt";
    Carte carte;
    Joueur Joueur;
    charger_fichiers(level_fichier, config_fichier, carte, Joueur);
}