
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define J1 'X'
#define J2 'O'
#define SP '-'
#define EmptyRank {SP,SP,SP,SP,SP,SP,SP,SP}
#define StartBoard {EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank}
#define depth 2

struct Game
{
    char Board[8][8];
    char x;
    char y;
    char Who;
};

void PrintBoard(struct Game Game)
{
    printf("\n   # A B C D E F G H #\n");
    for(char y=0;y<8;y++)
    {
        printf("   %i",y+1);
        for(char x=0;x<8;x++)
        {
            printf(" %c",Game.Board[y+0][x+0]);
        }
        printf(" %i\n",y+1);
    }
    printf("   # A B C D E F G H #\n");
    return;
}

char MoveLegal(struct Game Game, int dx, int dy)
{
    char i=1;
    while((Game.x+i<8 || dx!=1) && (Game.y+i<8 || dy!=1) && (Game.x+1>i || dx!=-1) && (Game.y+1>i || dy!=-1))
    {
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==SP) return 1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==Game.Who) return i==1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==J1+J2-Game.Who) i++;
    }
    return 1;
}

char MoveLegalAll(struct Game Game)
{
    if (Game.Board[Game.y+0][Game.x+0]!=SP) return 1; // Tile not empty
    if (MoveLegal(Game,+1,+0)==0) return 0; // Going FE
    if (MoveLegal(Game,-1,+0)==0) return 0; // Going FW
    if (MoveLegal(Game,+0,+1)==0) return 0; // Going FN
    if (MoveLegal(Game,+0,-1)==0) return 0; // Going FS
    if (MoveLegal(Game,+1,+1)==0) return 0; // Going NE
    if (MoveLegal(Game,-1,-1)==0) return 0; // Going SW
    if (MoveLegal(Game,-1,+1)==0) return 0; // Going NW
    if (MoveLegal(Game,+1,-1)==0) return 0; // Going SE
    return 1;
}

char MoveValid(struct Game Game)
{
    if (Game.x<0 || Game.x>7 || Game.y<0 || Game.y>7)
    {
        //printf(" - This is not even a move !");
        return 1;
    }
    if (MoveLegalAll(Game))
    {
        //printf(" - This move is illegal !");
        return 1;
    }
    return 0;
}

void AskMove(struct Game *Game,char* coords)
{
    do
    {
        //printf("\nPlayer %c, place a disk, using A1 - H8 format: ",Game->Who);
        printf("Joué : %s\n",coords);
        sleep(10);
        Game->x=coords[0]-'A'; // 'A' = 65
        Game->y=coords[1]-'1'; // '1' = 49
    } while (MoveValid(*Game));
    return;
}

void FlipMove(struct Game *Game, int dx, int dy)
{
    char i=1;
    while((Game->x+i<7 || dx!=1) && (Game->y+i<7 || dy!=1) && (Game->x>i || dx!=-1) && (Game->y>i || dy!=-1))
    {
        if (Game->Board[Game->y+i*dy][Game->x+i*dx]==Game->Who) return;
        if (Game->Board[Game->y+i*dy][Game->x+i*dx]==J1+J2-Game->Who)
        {
            Game->Board[Game->y+i*dy][Game->x+i*dx]=Game->Who;
            i++;
        }
    }
    return;
}

void ExeMove(struct Game *Game)
{
    Game->Board[Game->y+0][Game->x+0]=Game->Who; // Placing disk
    if (MoveLegal(*Game,-1,+0)==0) FlipMove(Game,-1,+0); // Going FW
    if (MoveLegal(*Game,+1,+0)==0) FlipMove(Game,+1,+0); // Going FE
    if (MoveLegal(*Game,+0,+1)==0) FlipMove(Game,+0,+1); // Going FS
    if (MoveLegal(*Game,+0,-1)==0) FlipMove(Game,+0,-1); // Going FN
    if (MoveLegal(*Game,+1,+1)==0) FlipMove(Game,+1,+1); // Going SE
    if (MoveLegal(*Game,-1,-1)==0) FlipMove(Game,-1,-1); // Going NW
    if (MoveLegal(*Game,-1,+1)==0) FlipMove(Game,-1,+1); // Going SW
    if (MoveLegal(*Game,+1,-1)==0) FlipMove(Game,+1,-1); // Going NE
    return;
}

char GameNotOver(struct Game *Game)
{
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            Game->x=x;
            Game->y=y;
            if (!MoveLegalAll(*Game)) return 1;
        }
    }
    Game->Who=J1+J2-Game->Who;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            Game->x=x;
            Game->y=y;
            if (!MoveLegalAll(*Game)) return 1;
        }
    }
    return 0;
}

void game_ended(struct Game Game)
{
    int O_disks=0,X_disks=0,empty_tiles=0;
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if (Game.Board[y][x]==J1) X_disks++;
            if (Game.Board[y][x]==J2) O_disks++;
            if (Game.Board[y][x]==SP) empty_tiles++;
        }
    }
    if (X_disks>O_disks) X_disks+=empty_tiles;
    else O_disks+=empty_tiles;
    printf("\nLa partie est terminée, score : %i - %i\n",X_disks,O_disks);
    return;
}

void fct_eval(struct Game Game)
{
    printf("\n-----FCT EVALUATION-----\n");
    struct Game current_game=Game;
    int coups_possibles=0;
    int coups_possibles_adversaire=0;
    int nb_pions=0;

    //nb de coins
    int corners=(current_game.Board[0][0]==current_game.Who)+(current_game.Board[0][7]==current_game.Who)+(current_game.Board[7][0]==current_game.Who)+(current_game.Board[7][7]==current_game.Who);

    //comptage pions
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if (current_game.Board[y][x]==current_game.Who) nb_pions++;
        }
    }

    //coups possibles
    for(int j=0;j<8;j++)
    {
        for(int i=0;i<8;i++)
        {
            current_game.x=i;
            current_game.y=j;
            if (MoveLegalAll(current_game)==0) coups_possibles++;
        }
    }

    //coups possibles adversaire
    current_game.Who=J1+J2-current_game.Who;
    for(int j=0;j<8;j++)
    {
        for(int i=0;i<8;i++)
        {
            current_game.x=i;
            current_game.y=j;
            if (MoveLegalAll(current_game)==0) coups_possibles_adversaire++;
        }
    }
    current_game.Who=J1+J2-current_game.Who;

    //force de la disposition
    //tableau tiré de http://imagine.enpc.fr/~monasse/Info/Projets/2003/othello.pdf
    int tableau_force_référence[8][8]={{500,-150,30,10,10,30,-150,500},{-150,-250,0,0,0,0,-250,-150},{30,0,1,2,2,1,0,30},{10,0,2,16,16,2,0,10},{10,0,2,16,16,2,0,10},{30,0,1,2,2,1,0,30},{-150,-250,0,0,0,0,-250,-150},{500,-150,30,10,10,30,-150,500}};
    int valeur_de_force=0;
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if (current_game.Board[y][x]==current_game.Who) valeur_de_force+=tableau_force_référence[y][x];
        }
    }

    printf("Coups possibles : %d\n",coups_possibles);
    printf("Coups possibles adv : %d\n",coups_possibles_adversaire);
    printf("Nb pions : %d\n",nb_pions);
    printf("Coins capturés : %d\n",corners);
    printf("Force : %d\n",valeur_de_force);
    return;
}

void ia_primitive(struct Game *Game)
{
    printf("\n-----IA PRIMITIVE-----\n");
    printf("Joueur : %c\n",Game->Who);
    struct Game current_game=*Game;
    int nb_coups_possibles=0;
    char tab_lettres[]={'A','B','C','D','E','F','G','H'};
    char tab_chiffres[]={'1','2','3','4','5','6','7','8'};
    //on détermine le nombre de coups possibles
    for(int j=0;j<8;j++)
    {
        for(int i=0;i<8;i++)
        {
            current_game.x=i;
            current_game.y=j;
            if (MoveLegalAll(current_game)==0) nb_coups_possibles++;
        }
    }
    //on enregistre les coups possible
    char coups_possibles[nb_coups_possibles][3];
    char coups_possibles_3[nb_coups_possibles][3];
    int index=0;
    for(int j=0;j<8;j++)
    {
        for(int i=0;i<8;i++)
        {
            current_game.x=i;
            current_game.y=j;
            if (MoveLegalAll(current_game)==0)
            {
                coups_possibles[index][0]=tab_lettres[i];
                coups_possibles[index][1]=tab_chiffres[j];
                coups_possibles_3[index][0]=i;
                coups_possibles_3[index][1]=j;
                //printf("%c %c\n",tab_lettres[i],tab_chiffres[j]);
                index++;
            }
        }
    }
    //on range tout ça dans un tableau
    char coups_possibles_2[nb_coups_possibles][3];
    printf("Coups possibles : ");
    for(int j=0;j<nb_coups_possibles;j++)
    {

        memcpy(coups_possibles_2[j], coups_possibles[j], 2);
        coups_possibles_2[j][2] = '\0'; 
        printf("%s ",coups_possibles_2[j]);
    }
    printf("\n");
    //on en choisit un au hasard
    srand(time(NULL));
    int nbr_aleat = rand()%nb_coups_possibles;
    printf("Coup choisi : %s\n",coups_possibles_2[nbr_aleat]);
    Game->x=coups_possibles_3[nbr_aleat][0];
    Game->y=coups_possibles_3[nbr_aleat][1];
    sleep(1);
    return;
}

int main()
{
    printf("\nWelcome to our playable version of Reversi !\n");
    struct Game Game={StartBoard,0,0,J1};
    Game.Board[3][3]=J2;
    Game.Board[3][4]=J1;
    Game.Board[4][3]=J1;
    Game.Board[4][4]=J2;
    while (GameNotOver(&Game))
    {
        PrintBoard(Game);
        fct_eval(Game);
        //AskMove(&Game);
        ia_primitive(&Game);
        ExeMove(&Game);
        Game.Who=J1+J2-Game.Who;
    }
    PrintBoard(Game);
    game_ended(Game);
    return 0;
}