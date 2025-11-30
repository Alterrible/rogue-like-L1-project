using namespace std;
#include <iostream>
#include <string>
#include <fstream>

int decouper(const string& ligne, string mots[], int max_mots) {
    int nb = 0;
    string mot = "";

    for (int i = 0; i <= ligne.size(); i++) {
        if (i == ligne.size() || ligne[i] == ' ') {
            if (mot != "" && (int)mot[0] != 13 && nb < max_mots) {
                mots[nb] = mot;
                nb++;
                mot = "";
            }
        } else {
            mot += ligne[i];
        }
    }

    return nb;
}

int main(){
    string fichier = "init.txt";

    fstream flux;
    flux.open(fichier, ios::in);

    if (!flux.is_open()) {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }

    string ligne;
    int compt = 0;
    int section = 0;

    while (getline(flux, ligne)) {
        cout << "ligne nb: " << compt << " text:" << ligne << endl;
        string mots[20];
        int nb = decouper(ligne, mots, 20);

        // nouvelle section -> [
        if (ligne.size() > 0 && ligne[0] == '[') {
            section++;
        }
        cout << "SECTION " << section << endl;

        compt++;
    }

    flux.close();
}

// int main(){
//     string fichier = "init.txt";

//     fstream flux;
//     flux.open(fichier, ios::in);

//     if (!flux.is_open()) {
//         cout << "Erreur : impossible d'ouvrir " << fichier << endl;
//     }

//     string ligne;
//     int compt = 0;

//     while (getline(flux, ligne)) {
//         cout << "ligne nb: " << compt << " text:" << ligne << endl;
//         string mots[20];
//         int nb = decouper(ligne, mots, 20);

//         for (int i = 0; i < nb; i++) {
//             cout << i+1 << " = " << mots[i] << ", ";
//             cout << i+1 << " ASCII = " << (int)mots[i][0] << endl;
//         }

//         cout << "nb = " << nb << endl;
//         compt++;
//     }

//     flux.close();
// }

// int main(){
//     string ligne = "des pommes de terre svp";
//     string mots[20];
//     int nb = decouper(ligne, mots, 20);

//     for (int i = 0; i < nb; i++) {
//         cout << i+1 << " = " << mots[i] << endl;
//         cout << i+1 << " ASCII = " << (int)mots[i][0] << endl;
//     }

//     cout << "nb = " << nb << endl;
// }
