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
    [d] west/droite

Interactions :
    [i] nord/haut
    [k] sud/bas
    [j] est/gauche
    [l] west/droite

la touche [t] pour ouvrir et fermer l'inventaire
la touche [espace] pour rentrer dans le jeu, fermer les panneaux d'informations et pour utiliser un item dans l'inventaire


---- OBJECTIFS ----
→ atteindre la case $
→ posséder tout les items


---- ENTITÉS ----
- les portes sont les chiffres
- les items sont les lettres minuscule
- les monstres sont les lettres majuscule
- les mures sont les ashtag
- le sol sont les points


---- INFO UTILE ----
Les items sont traversable et sont ramassage en faisant une interaction avec.
Les portes doivent remplir les contrainte nécessaire pour les ouvrir.
Les monstres ne sont pas traversable et doivent être combattu, malgré qu'une fuite est envisageable mais difficile.
