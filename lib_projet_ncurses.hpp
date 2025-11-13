// LIBRAIRIE SIMPLIFIEE POUR UTILISER NCURSES - A UTILISER IMPERATIVEMENT - NE PAS MODIFIER

#pragma once
#include <string>

// Utilisés pour attendre un nombre voulu de millisecondes
#include <chrono>
#include <thread>


// ncurses est une librairie permettant une manipulation du terminal plus avancée que de simples cin et cout
#ifdef _WIN32
    #include "extlib/pdcurses/include/curses.h"
#endif

#ifdef __unix__
    #include <ncurses.h>
#endif



// ------------------------
// ----- CONSOLE SIZE -----
// ------------------------
const int CONSOLE_X_SIZE = 80;
const int CONSOLE_Y_SIZE = 24;
// -------------------

// ----- TYPES -----
// ncurses fonctionne en enregistrant une par une des paires de couleurs et en faisant référence à leur indice (commençant à 1) pour les utiliser 
struct PaireCouleur{
  short texte;  // couleur du texte
  short fond;   // couleur du fond
};


// ----- FONCTIONS ----- 

// Initialise la console de Curses avec un tableau de PaireCouleur et son nombre d'éléments
bool initialiser_console(const PaireCouleur colorlist[], int nb_couleurs );

// Efface la console
void effacer_console();

// Ferme la console de Curses
void fermer_console();


// ----- saisies et pause -----

// lire une valeur au clavier (bloquant) 
int saisie_bloquante();

// lire une valeur au clavier (non-bloquant) 
int saisie_non_bloquante();

// Pause durant ms millisecondes
void attendre_millisecondes(int ms);


// ----- curseur et écriture -----

// Bouge le curseur console à l'emplacement (x, y)
void bouger_curseur(int x, int y);

// Ecrit un caractère chr à l'emplacement (x, y)
void ecrire_char(int x, int y, char chr);

// Ecrit une chaine de caractère à partir de l'emplacement (x, y)
void ecrire_string(const std::string& message, int x, int y);

// Remet les couleurs par défaut pour les affichages (blanc sur noir)
void couleur_defaut();

// Choisi une couleur pour les affichages
void changer_couleur(int indice);






// ----- FONCTIONS - IMPLEMENTATION ----- 


bool initialiser_console(const PaireCouleur colorlist[], int nb_couleurs ) {
    // Starts ncurse
    WINDOW* win = nullptr;
    win = initscr();
    keypad(stdscr, TRUE);
    // Prevent from immediately drawing on screen
    noecho();

    // Invisible cursor
    curs_set(0);

    if (has_colors()) {
        start_color();
        PaireCouleur coul ;
     
        for (short i = 0 ; i < nb_couleurs ; i++) {
          coul = colorlist[i];
          init_pair(i+1, coul.texte, coul.fond); 
        }
        

    }

    else {
        addstr("WARNING! Your console does not support colours.");
        getch();
        // TODO: print message
    }

    return (win != nullptr);
}

void effacer_console() {
    erase();
}

void fermer_console() {
    endwin();
}

// ----- saisies et pause -----

// lire une valeur au clavier (bloquant) 
int saisie_bloquante(){
   timeout(-1);
   return getch();
}

// lire une valeur au clavier (non-bloquant) 
int saisie_non_bloquante(){
   timeout(0);
   return getch();
}


void attendre_millisecondes(int ms){
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));

}

// ----- curseur et écriture -----


void ecrire_char(int x, int y, char chr) {
    mvaddch(y, x, chr);
}

void bouger_curseur(int x, int y) {
    move(y, x);
}

void changer_couleur (int indice_paire){
   attron(COLOR_PAIR(indice_paire));
}

void couleur_defaut() {
    attron(COLOR_PAIR(1));
}


void ecrire_string(const std::string& message, int x, int y) {
    bouger_curseur(x, y);
    printw("%s", message.c_str());
}



