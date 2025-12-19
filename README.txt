---- COMMANDE DE DÉMARRAGE ----
Build le projet :
    g++ main.cpp -o main.exe -lncurses

Lancer le jeu :
    ./main.exe


---- TOUCHES ----
Déplacement :
    [z] nord/haut
    [s] sud/bas
    [q] est/gauche
    [d] ouest/droite

Interactions :
    [i] nord/haut
    [k] sud/bas
    [j] est/gauche
    [l] ouest/droite

La touche [t] permet d’ouvrir et de fermer l’inventaire.
La touche [espace] permet d’entrer dans le jeu, de fermer les panneaux d’informations et d’utiliser un item dans l’inventaire.


---- OBJECTIFS ----
→ atteindre la case $
→ posséder tous les items


---- ENTITÉS ----
- les portes sont les chiffres
- les items sont les lettres minuscules
- les monstres sont les lettres majuscules
- les murs sont les hashtags
- le sol est représenté par des points


---- INFO UTILE ----
Les items sont traversables et se ramassent en interagissant avec eux.
Les portes doivent remplir les contraintes nécessaires pour être ouvertes.
Les monstres ne sont pas traversables et doivent être combattus, même si une fuite est envisageable, mais difficile.


---- FICHIER DE CONFIGURATION ----
Le fichier de configuration, nommé init.txt, est découpé en plusieurs sections, chacune correspondant à un titre placé entre crochets. 
Les lignes d’en-tête des tableaux, son comme des commentaires et commençant par '|', 
elles indiquent l’ordre des champs ainsi que le type des valeurs attendu pour chaque colonne.
Les valeurs doivent être séparées par des espaces et certaines données (comme les noms ou descriptions) ne doivent pas contenir d’espaces.
Le nombre de lignes par tableau doit impérativement correspondre aux valeurs des constantes définies dans enregistrement.hpp.