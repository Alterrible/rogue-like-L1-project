#ifndef AFFICHAGE_HPP
#define AFFICHAGE_HPP

#include "enregistrement.hpp"
#include "lib_projet_ncurses.hpp"
#include <iostream>
using namespace std;

string underscore_espace(const string& input) {
    string result = input;
    for (char& c : result) {
        if (c == '_') {
            c = ' ';
        }
    }
    return result;
}


// ---- AFFICHAGE JEU ----

// affiche la carte selon visible / ex_visible
void afficher_carte(const Jeu &jeu) {
    // boucle sur les cases
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++) {

            // case visible ce tour
            if (jeu.carte.visible[y][x]) {
                char c = jeu.carte.cases[y][x];
                ecrire_char(x, y, c);
            }
            else {
                // case déjà vue
                bool deja_vu = (jeu.carte.ex_visible[y][x] != '\0');

                if (deja_vu) {
                    char c = jeu.carte.ex_visible[y][x];
                    ecrire_char(x, y, c);
                }
                else {
                    // jamais vue
                    ecrire_char(x, y, ' ');
                }
            }
        }
    }
}


void afficher_jeu(const Jeu &jeu) {
    effacer_console();
    afficher_carte(jeu);

    // affiche la hotbar
    string hotbar;
    for (int i = 0; i < NB_STATS; i++) {
        hotbar += jeu.nom_stats[i] + "[" + to_string(jeu.joueur.stat[i]) + "] ";
    }
    ecrire_string(hotbar, 0, jeu.carte.hauteur);
}


// écran d'affichage contrainte
void afficher_modal (Jeu& jeu) {
    effacer_console();
    afficher_carte(jeu);

    string& msg = jeu.modal_txt;
    size_t msg_len = msg.size();
    int cadre_largeur = static_cast<int>(msg_len) + 4;
    int cadre_hauteur = 3;

    int x = (jeu.carte.largeur - cadre_largeur) / 2;
    int y = (jeu.carte.hauteur - cadre_hauteur) / 2;

    // ligne supérieure
    for (int i = 0; i < cadre_largeur; i++) {
        ecrire_char(x + i, y, '-');
    }

    // ligne centrale
    ecrire_char(x, y + 1, '|');

    for (size_t i = 0; i < msg_len; i++) {
        ecrire_char(x + 2 + static_cast<int>(i), y + 1, msg[i]);
    }

    ecrire_char(x + cadre_largeur - 1, y + 1, '|');

    // ligne inférieure
    for (int i = 0; i < cadre_largeur; i++) {
        ecrire_char(x + i, y + 2, '-');
    }
}


// ---- AFFICHAGE INVENTAIRE ----
void afficher_inventaire(Jeu &jeu) {
    effacer_console();

    const Joueur &j = jeu.joueur;

    // agrégation : on regroupe les idConfig identiques en (id, quantité)
    int ids[TAILLE_ITEMS];
    int quantites[TAILLE_ITEMS];
    int nb_types = 0;

    for (int i = 0; i < j.nb_inventaire; i++) {
        int idConf = j.inventaire[i];
        int idx = -1;
        for (int t = 0; t < nb_types; t++) {
            if (ids[t] == idConf) {
                idx = t;
                break;
            }
        }
        if (idx == -1 && nb_types < TAILLE_ITEMS) {
            ids[nb_types] = idConf;
            quantites[nb_types] = 1;
            nb_types++;
        } else if (idx != -1) {
            quantites[idx]++;
        }
    }

    int largeur = jeu.carte.largeur;
    int hauteur = jeu.carte.hauteur;

    // Titre
    string titre = "INVENTAIRE (T = fermer, ESPACE = utiliser)";
    int x_titre = (largeur - static_cast<int>(titre.size())) / 2;
    if (x_titre < 0) x_titre = 0;
    ecrire_string(titre, x_titre, 0);

    if (nb_types == 0) {
        string vide = "Inventaire vide.";
        int x_vide = (largeur - static_cast<int>(vide.size())) / 2;
        if (x_vide < 0) x_vide = 0;
        ecrire_string(vide, x_vide, hauteur / 2);
        return;
    }

    // zone de liste
    int y_liste = 2;
    int hauteur_liste = hauteur - 7; // laisse de la place pour détails en bas
    if (hauteur_liste < 1) hauteur_liste = 1;
    int max_visible = hauteur_liste;

    // Clamp de la sélection
    if (jeu.inv_selection_index < 0) jeu.inv_selection_index = 0;
    if (jeu.inv_selection_index >= nb_types) jeu.inv_selection_index = nb_types - 1;

    // Mise à jour du scroll pour que la sélection reste visible
    if (jeu.inv_scroll_haut > jeu.inv_selection_index) {
        jeu.inv_scroll_haut = jeu.inv_selection_index;
    }
    if (jeu.inv_scroll_haut < 0) jeu.inv_scroll_haut = 0;
    if (jeu.inv_selection_index >= jeu.inv_scroll_haut + max_visible) {
        jeu.inv_scroll_haut = jeu.inv_selection_index - max_visible + 1;
    }
    if (jeu.inv_scroll_haut > nb_types - 1) {
        jeu.inv_scroll_haut = max(0, nb_types - max_visible);
    }

    // Affichage de la liste
    for (int ligne = 0; ligne < max_visible; ligne++) {
        int idx = jeu.inv_scroll_haut + ligne;
        if (idx >= nb_types) break;

        // retrouver la config de l'item
        Config_item cfg;
        // on peut utiliser trouver_config_item_par_id si tu préfères
        bool found = false;
        for (int i = 0; i < jeu.nb_cfg_items; i++) {
            if (jeu.cfg_items[i].id == ids[idx]) {
                cfg = jeu.cfg_items[i];
                found = true;
                break;
            }
        }
        if (!found) continue;

        string nom = underscore_espace(cfg.nom);
        string ligne_str;

        bool est_selectionne = (idx == jeu.inv_selection_index);
        if (est_selectionne) {
            ligne_str += "[" + nom + "]";
        } else {
            ligne_str += " " + nom + " ";
        }

        ligne_str += "  x" + to_string(quantites[idx]);

        // On tronque si trop long
        if ((int)ligne_str.size() > largeur - 2) {
            ligne_str = ligne_str.substr(0, largeur - 2);
        }

        ecrire_string(ligne_str, 1, y_liste + ligne);
    }

    // Affichage des détails de l'item sélectionné
    int idx_sel = jeu.inv_selection_index;
    Config_item cfg_sel;
    bool found_sel = false;
    for (int i = 0; i < jeu.nb_cfg_items; i++) {
        if (jeu.cfg_items[i].id == ids[idx_sel]) {
            cfg_sel = jeu.cfg_items[i];
            found_sel = true;
            break;
        }
    }

    if (found_sel) {
        int y_details = y_liste + max_visible + 1;
        if (y_details < hauteur) {

            string nom_detail = underscore_espace(cfg_sel.nom);
            string desc = underscore_espace(cfg_sel.description);

            if ((int)desc.size() > largeur - 2) {
                desc = desc.substr(0, largeur - 2);
            }

            ecrire_string("Détails : " + nom_detail, 1, y_details);
            ecrire_string(desc, 1, y_details + 1);

            // stats bonus
            int y_stats = y_details + 3;
            for (int s = 0; s < NB_STATS && y_stats + s < hauteur; s++) {
                int bonus = cfg_sel.bonus[s];
                string ligne_stat = jeu.nom_stats[s] + " : ";
                if (bonus >= 0) ligne_stat += "+";
                ligne_stat += to_string(bonus);
                ecrire_string(ligne_stat, 1, y_stats + s);
            }
        }
    }
}


// ---- AFFICHAGE DEBUT/FIN ----

const string ASCII_GAGNE[] = {
"  __  __   __ __  _ ___ ",
" / _]/  \\ / _]  \\| | __|",
"| [/\\ /\\ | [/\\ | ' | _| ",
" \\__/_||_|\\__/_|\\__|___|"
};

const string ASCII_PERDU[] = {
" ___ ___ ___ __  _  _ ",
"| _,\\ __| _ \\ _\\| || |", 
"| v_/ _|| v / v | \\/ |", 
"|_| |___|_|_\\__/ \\__/ "
};

const string ascii_bienvenue[] = {
" __ _ ___ __  _  _   _  ___ __  _ _  _ ___ ",
"|  \\ | __|  \\| || \\ / || __|  \\| | || | __|",
"| -< | _|| | ' |`\\ V /'| _|| | ' | \\/ | _| ",
"|__/_|___|_|\\__|  \\_/  |___|_|\\__|\\__/|___|",
};

const string infos_bienvenue[] = {
    "Utilisez Z Q S D pour vous deplacer.",
    "Utilisez I J K L pour interagir.",
    "Appuyez sur ESPACE pour fermer les ecrans d'information.",
    "Vous pouvez fermer le jeu a tout moment avec ECHAP.",
    "",
    "Appuyez sur ESPACE pour commencer la partie."
};

void afficher_ecran_bienvenue(Jeu &jeu) {
    effacer_console();

    // sélection de l'ASCII
    const string* ascii = ascii_bienvenue;
    int ascii_hauteur = sizeof(ascii_bienvenue) / sizeof(string);

    const string* infos = infos_bienvenue;
    int infos_hauteur = sizeof(infos_bienvenue) / sizeof(string);

    // calcul de la largeur max
    int largeur = 0;

    for (int i = 0; i < ascii_hauteur; i++) {
        largeur = max(largeur, static_cast<int>(ascii[i].size()));
    }

    for (int i = 0; i < infos_hauteur; i++) {
        largeur = max(largeur, static_cast<int>(infos[i].size()));
    }

    // placement centré
    int total_hauteur = ascii_hauteur + 2 + infos_hauteur;

    int x = (jeu.carte.largeur  - largeur) / 2;
    int y = (jeu.carte.hauteur - total_hauteur) / 2;

    // affichage ASCII art
    for (int j = 0; j < ascii_hauteur; j++) {
        for (size_t col = 0; col < ascii[j].size(); col++) {
            ecrire_char(x + static_cast<int>(col), y + j, ascii[j][col]);
        }
    }

    // affichage du texte mouvement/interactions: zqsd, ijkl, espace
    int base = y + ascii_hauteur + 2;
    for (int j = 0; j < infos_hauteur; j++) {
        for (size_t col = 0; col < infos[j].size(); col++) {
            ecrire_char(x + static_cast<int>(col), base + j, infos[j][col]);
        }
    }
}


void afficher_game_over(Jeu &jeu) {
    effacer_console();

    const string* ascii;
    int hauteur;

    if (jeu.victoire) {
        ascii = ASCII_GAGNE;
        hauteur = sizeof(ASCII_GAGNE) / sizeof(string);
    } else {
        ascii = ASCII_PERDU;
        hauteur = sizeof(ASCII_PERDU) / sizeof(string);
    }

    int largeur = 0;
    for (int i = 0; i < hauteur; i++) {
        largeur = max(largeur, static_cast<int>(ascii[i].size()));
    }

    int x = (jeu.carte.largeur - largeur) / 2;
    int y = (jeu.carte.hauteur - hauteur) / 2;

    // afficher ASCII art
    for (int j = 0; j < hauteur; j++) {
        for (size_t col = 0; col < ascii[j].size(); col++) {
            ecrire_char(x + static_cast<int>(col), y + j, ascii[j][col]);
        }
    }

    ecrire_string("Appuyer sur [ECHAP] pour sortir", x, y + hauteur + 1);
}


// ---- utilitaire pour affichage de debug ----
void afficher_liste_id_contraintes(const Liste_id_contraintes& liste, const string& titre) {
    cout << titre << " (nb = " << liste.nb << ")" << endl;
    for (int i = 0; i < liste.nb; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << endl;
    }
}

void afficher_liste_equipements(const Liste_Id_Equipements& liste, const string& titre) {
    cout << titre << " (taille = " << liste.taille << ")" << endl;
    for (int i = 0; i < liste.taille; i++) {
        cout << "  ids[" << i << "] : " << liste.ids[i] << endl;
    }
}

void afficher_contrainte(const Contrainte& c) {
    cout << "id : " << c.id << endl;

    for (int i = 0; i < c.nb_items_possede; i++)
        cout << "items_possede[" << i << "] : " << c.items_possede[i] << endl;
    cout << "nb_items_possede = " << c.nb_items_possede << endl;

    for (int i = 0; i < NB_STATS; i++)
        cout << "stats_min[" << i << "] : " << c.stats_min[i] << endl;

    cout << to_string(c.a_symbole) << endl;
    cout << c.symbole_atteint << endl;
}

// affichage de debugage
void afficher_jeu_debug(const Jeu& jeu) {

    cout << "[DEBUG]" << endl;

 
    // CONFIG JOUEUR
    cout << "CONFIG JOUEUR" << endl;
    cout << "symbole : " << jeu.cfg_joueur.symbole << endl;
    cout << "nom : " << jeu.cfg_joueur.nom << endl;
    cout << "description : " << jeu.cfg_joueur.description << endl;

    for (int i = 0; i < NB_STATS; i++)
        cout << "stats[" << i << "] : " << jeu.cfg_joueur.stats[i] << endl;

    afficher_liste_equipements(jeu.cfg_joueur.inventaire_ids, "Inventaire joueur");

 
    // CONFIG ITEMS
    cout << "\\nCONFIG ITEMS" << endl;
    for (int i = 0; i < jeu.nb_cfg_items; i++) {
        cout << "\\nItem " << i << endl;
        cout << "id : " << jeu.cfg_items[i].id << endl;
        cout << "symbole : " << jeu.cfg_items[i].symbole << endl;
        cout << "nom : " << jeu.cfg_items[i].nom << endl;
        cout << "description : " << jeu.cfg_items[i].description << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "bonus[" << j << "] : " << jeu.cfg_items[i].bonus[j] << endl;
    }

 
    // CONFIG MONSTRES
    cout << "\\nCONFIG MONSTRES" << endl;
    for (int i = 0; i < jeu.nb_cfg_monstres; i++) {
        const auto& m = jeu.cfg_monstres[i];
        cout << "\\nMonstre " << i << endl;

        cout << "id : " << m.id << endl;
        cout << "symbole : " << m.symbole << endl;
        cout << "nom : " << m.nom << endl;
        cout << "description : " << m.description << endl;
        cout << "type IA : " << m.typeIA << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "stats_base[" << j << "] : " << m.stats_base[j] << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "stats_afflige[" << j << "] : " << m.stats_afflige[j] << endl;
    }

 
    // CONFIG PORTES
    cout << "\\nCONFIG PORTES" << endl;
    for (int i = 0; i < jeu.nb_cfg_portes; i++) {
        const auto& p = jeu.cfg_portes[i];

        cout << "\\nPorte " << i << endl;
        cout << "id : " << p.id << endl;
        cout << "symbole : " << p.symbole << endl;
        cout << "id_contrainte : " << p.id_contrainte << endl;
    }

 
    // CONDITIONS
    cout << "\\nCONDITIONS DE JEU" << endl;
    cout << "nbContraintes : " << jeu.cfgConditions.nbContraintes << endl;

    cout << "\\nContraintes" << endl;
    for (int i = 0; i < jeu.cfgConditions.nbContraintes; i++) {
        cout << "\\nContrainte " << i << endl;
        afficher_contrainte(jeu.cfgConditions.contraintes[i]);
        cout << "---" << endl;
    }

    afficher_liste_id_contraintes(jeu.cfgConditions.victoire, "\\nConditions victoire");
    afficher_liste_id_contraintes(jeu.cfgConditions.defaite, "\\nConditions défaite");

 
    // ITEMS SUR LA CARTE
    cout << "\\nITEMS DANS LA CARTE" << endl;
    cout << "nb_items : " << jeu.nb_items << endl;

    for (int i = 0; i < jeu.nb_items; i++) {
        const auto& it = jeu.items[i];
        cout << "Item " << i << " : id=" << it.id
             << " x=" << it.x << " y=" << it.y
             << " idConfig=" << it.idConfig << endl;
    }

 
    // MONSTRES SUR LA CARTE
    cout << "\\nMONSTRES DANS LA CARTE" << endl;
    cout << "nb_monstres : " << jeu.nb_monstres << endl;

    for (int i = 0; i < jeu.nb_monstres; i++) {
        const auto& mo = jeu.monstres[i];

        cout << "\\nMonstre " << i
             << " : idConfig=" << mo.idConfig
             << " x=" << mo.x << " y=" << mo.y
             << " actif=" << mo.actif << endl;

        for (int j = 0; j < NB_STATS; j++)
            cout << "  stats[" << j << "] : " << mo.stats[j] << endl;
    }

 
    // CARTE
    cout << "\\nCARTE" << endl;
    cout << "largeur : " << jeu.carte.largeur << endl;
    cout << "hauteur : " << jeu.carte.hauteur << endl;

    cout << "\\nCases :" << endl;
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << jeu.carte.cases[y][x];
        cout << endl;
    }

    cout << "\\nVisibilité :" << endl;
    for (int y = 0; y < jeu.carte.hauteur; y++) {
        for (int x = 0; x < jeu.carte.largeur; x++)
            cout << (jeu.carte.visible[y][x] ? "1" : "0");
        cout << endl;
    }

 
    // JOUEUR DYNAMIQUE

    cout << "\\nJOUEUR (DYNAMIQUE)" << endl;
    cout << "x : " << jeu.joueur.x << endl;
    cout << "y : " << jeu.joueur.y << endl;
    cout << "nb_inventaire : " << jeu.joueur.nb_inventaire << endl;

    for (int i = 0; i < jeu.joueur.nb_inventaire; i++) {
        cout << "inventaire[" << i << "] : " << jeu.joueur.inventaire[i] << endl;
    }

    for (int i = 0; i < NB_STATS; i++) {
        cout << "stat[" << i << "] : " << jeu.joueur.stat[i] << endl;
    }
 
    // ETAT DU JEU

    cout << "\\nETAT DU JEU" << endl;
    cout << "etat_termine : " << jeu.etat_termine << endl;
    cout << "victoire : " << jeu.victoire << endl;

    cout << "\\n[FIN DEBUG]" << endl;
}

#endif
