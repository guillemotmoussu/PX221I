
#include <stdio.h>

int main()
{
    //lancement d'une partie
    printf("Hello World\n");
    return 1;
}

int partie()
{
    char plateau[10][10]=initialisation();
    int FinJeu = 0;
    char Joueur = 'B';
    while FinJeu == 0
    {
        affichage();
        //Blanc joue si possible
        Joueur = 'N';
        //Noir joue si possible
        Joueur = 'B';
    }
    return 1;
}

char initialisation()
{
    
    return plateau;
}

void affichage(char **plateau, char joueur)
{
    printf("# A B C D E F G H #\n");
    for (int i = 0; i < 7; i++)
    {
        printf("%d",i+1);
        for (int j = 0; i < 7; j++) printf("%c",plateau[i][j]);
        printf("%d\n",i+1);
    }
    printf("# A B C D E F G H #\n");
    printf("\n C'est au tour du joueur %c\n",joueur);
}