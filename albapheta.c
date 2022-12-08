
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
#include <string.h>
#include "userNetwork.h"
*/

#define Infinity 120
#define EvalWin 110
#define MaxDepth 8
#define ExistP1 0
#define ExistP2 0

struct Game
{
    unsigned long int Disks;        //Pions sur le plateau
    unsigned long int Color;        //Couleur sur le plateau
    unsigned long int BitMask;      //Sert à faire des opérations bit à bit
    char Coords; //Stocke le joueur actif,  les erreurs, et les coordonnées à jouer
};

void PrintBoard(struct Game Game)
//permet d'afficher le plateau
{
    printf("\n   # A B C D E F G H #\n");
    for(char y=0;y<8;y++)
    {
        printf("   %i",y+1);
        for(char x=0;x<8;x++)
        {
            printf(" %c",!(Game.Disks&(Game.BitMask<<(y*8+x)))?'-':(Game.Color&(Game.BitMask<<(y*8+x))?'X':'O'));
        }
        printf(" %i\n",y+1);
    }
    printf("   # A B C D E F G H #\n\n");
    return;
}

char FlipMove(struct Game *Game, char dx)
{
    char i=1;
    while(  (dx!=-7||((((Game->Coords&127)+i*dx)/8>=0)&&(((Game->Coords&127)+i*dx)%8!=0)))&&
            (dx!=-8||(((Game->Coords&127)+i*dx)/8>=0))&&
            (dx!=-9||((((Game->Coords&127)+i*dx)/8>=0)&&(((Game->Coords&127)+i*dx)%8!=7)))&&
            (dx!=-1||(((Game->Coords&127)+i*dx)%8!=7))&&
            (dx!=+7||((((Game->Coords&127)+i*dx)/8<=7)&&(((Game->Coords&127)+i*dx)%8!=7)))&&
            (dx!=+8||(((Game->Coords&127)+i*dx)/8<=7))&&
            (dx!=+9||((((Game->Coords&127)+i*dx)/8<=7)&&(((Game->Coords&127)+i*dx)%8!=0)))&&
            (dx!=+1||(((Game->Coords&127)+i*dx)%8!=0))  )
    {
        if(Game->Disks&(Game->BitMask<<((Game->Coords&127)+i*dx)))
        {
            if(Game->Coords&128)
            {
                if(Game->Color&(Game->BitMask<<((Game->Coords&127)+i*dx)))
                {
                    for(char f=1;f<i;f++) Game->Color=Game->Color^(Game->BitMask<<((Game->Coords&127)+f*dx));
                    return i==1;
                }
                else i++;
            }
            else
            {
                if(Game->Color&(Game->BitMask<<((Game->Coords&127)+i*dx))) i++;
                else
                {
                    for(char f=1;f<i;f++) Game->Color=Game->Color^(Game->BitMask<<((Game->Coords&127)+f*dx));
                    return i==1;
                }
            }
        }
        else return 1;
    }
    return 1;
}

char ExeMove(struct Game *Game)
//permettra de jouer un coup
{
    if(Game->Disks&(Game->BitMask<<(Game->Coords&127))) return 1;
    char MoveLegal=0;
    if(!FlipMove(Game,-7)) MoveLegal=1; // Going NE
    if(!FlipMove(Game,-8)) MoveLegal=1; // Going FN
    if(!FlipMove(Game,-9)) MoveLegal=1; // Going NW
    if(!FlipMove(Game,-1)) MoveLegal=1; // Going FW
    if(!FlipMove(Game,+7)) MoveLegal=1; // Going SW
    if(!FlipMove(Game,+8)) MoveLegal=1; // Going FS
    if(!FlipMove(Game,+9)) MoveLegal=1; // Going SE
    if(!FlipMove(Game,+1)) MoveLegal=1; // Going FE
    if(MoveLegal==1)
    {
        Game->Disks=Game->Disks|(Game->BitMask<<(Game->Coords&127));
        if(Game->Coords&128) Game->Color=Game->Color|(Game->BitMask<<(Game->Coords&127));
    }
    return !MoveLegal;
}

char FinalEval(struct Game Game)
//permettra d'évaluer une position quand la partie est terminée
{
    char eval=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(Game.BitMask<<i))
        {
            if(Game.Coords&128)
            {
                if(Game.Color&(Game.BitMask<<i)) eval++;
                else eval--;
            }
            else
            {
                if(Game.Color&(Game.BitMask<<i)) eval--;
                else eval++;
            }
        }
    }
    return eval;
}

char GrowTree(struct Game Game, char depth, char TopEval, char CutEval)
//Calcule les branches de l'arbre pour déterminer l'évaluation du joueur actuel
{
    if (depth<1) return FinalEval(Game);
    char MaxEval=-Infinity;
    char NewEval=0;
    unsigned long int SaveDisks=Game.Disks; //sauvegardes
    unsigned long int SaveColor=Game.Color;
    Game.Coords=~(Game.Coords|127); // OPTI ?
    for(char i=0;i<64;i++)
    {
        if(!ExeMove(&Game)) //on teste si le coup est possible
        {
            NewEval=GrowTree(Game,depth-1,-CutEval,-TopEval);
            assert(NewEval!=Infinity);
            assert(NewEval!=-Infinity);
            if(CutEval<=NewEval) return -NewEval; //On coupe la branche actuelle
            if(TopEval<NewEval) TopEval=NewEval; //On vient de couper une branche
            if(MaxEval<NewEval) MaxEval=NewEval; //On a trouvé un meilleur coup, mais pas de coupe supplémentaire
            Game.Disks=SaveDisks; //On annule le coup joué
            Game.Color=SaveColor;
        }
        Game.Coords++;
    }
    if(MaxEval!=-Infinity) return -MaxEval; //si on a trouvé une valeur max pour le joueur adverse on la retourne
    Game.Coords=Game.Coords^192; // OPTI ?
    for(char i=0;i<64;i++)
    {
        if(!ExeMove(&Game)) //cette fois on regarde les coups du joueur actuel
        {
            NewEval=GrowTree(Game,depth-1,TopEval,CutEval);
            assert(NewEval!=Infinity);
            assert(NewEval!=-Infinity);
            if(TopEval<NewEval) TopEval=NewEval;
            if(MaxEval<NewEval) MaxEval=NewEval;
            Game.Disks=SaveDisks;
            Game.Color=SaveColor;
        }
        Game.Coords++;
    }
    if(MaxEval!=-Infinity) return MaxEval;
    NewEval=FinalEval(Game);
    if (NewEval<0) return -EvalWin;
    if (NewEval>0) return EvalWin;
    return 0;
}

char BotMove(struct Game *Game)
{
    printf(" Bot %c is thinking...\n",Game->Coords&128?'X':'O');
    char MaxEval=-Infinity;
    char NewEval=0;
    unsigned long int SaveDisks=Game->Disks;
    unsigned long int SaveColor=Game->Color;
    char BestMove=64;
    Game->Coords=Game->Coords&128; // OPTI ?
    for(char i=0;i<64;i++)
    {
        if(!ExeMove(Game))
        {
            NewEval=GrowTree(*Game,MaxDepth,-Infinity,Infinity);
            assert(NewEval!=Infinity);
            assert(NewEval!=-Infinity);
            if(MaxEval<=NewEval)
            {
                MaxEval=NewEval;
                BestMove=Game->Coords;
            }
            Game->Disks=SaveDisks;
            Game->Color=SaveColor;
        }
        Game->Coords++;
    }
    if(BestMove==64)
    {
        printf(" Uh oh ! Bot %c couldn't find any moves !\n",Game->Coords&128?'X':'O');
        return 1;
    }
    printf(" Bot %c played: %c%c (eval %i)\n",Game->Coords&128?'X':'O',((BestMove&127)%8)+'A',((BestMove&127)/8)+'1',MaxEval);
    Game->Coords=BestMove;
    ExeMove(Game);
    return 0;
}

char GameOver(struct Game Game)
{
    Game.Coords=Game.Coords&128; // OPTI ?
    for(char i=0;i<64;i++)
    {
        if(!ExeMove(&Game)) return 0;
        Game.Coords++;
    }
    printf("Seems like player %c can't play...\n",Game.Coords&128?'X':'O');
    Game.Coords=Game.Coords^192; // OPTI ?
    for(char i=0;i<64;i++)
    {
        if(!ExeMove(&Game)) return 0;
        Game.Coords++;
    }
    printf("Seems like player %c can't play either !\n",Game.Coords&128?'X':'O');
    return 1;
}

void Score(struct Game Game)
{
    PrintBoard(Game);
    char Disk1=0,Disk2=0,Empty=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(Game.BitMask<<i))
        {
            if(Game.Color&(Game.BitMask<<i)) Disk1++;
            else Disk2++;
        }
        else Empty++;
    }
    if (Disk1<Disk2) Disk2+=Empty;
    if (Disk2<Disk1) Disk1+=Empty;
    printf("Game finished, score: %i-%i\n",Disk1,Disk2);
    return;
}

int main()
{
    printf("\nWelcome to our playable version of Reversi !\n");
    struct Game Game={0,0,0,128};
    Game.BitMask=1;
    Game.Disks=Game.Disks| (Game.BitMask<<(3*8+3));
    Game.Color=Game.Color&~(Game.BitMask<<(3*8+3));
    Game.Disks=Game.Disks| (Game.BitMask<<(3*8+4));
    Game.Color=Game.Color| (Game.BitMask<<(3*8+4));
    Game.Disks=Game.Disks| (Game.BitMask<<(4*8+3));
    Game.Color=Game.Color| (Game.BitMask<<(4*8+3));
    Game.Disks=Game.Disks| (Game.BitMask<<(4*8+4));
    Game.Color=Game.Color&~(Game.BitMask<<(4*8+4));
    while(!GameOver(Game))
    {
        PrintBoard(Game);
        BotMove(&Game);
        Game.Coords=Game.Coords^128;
    }
    Score(Game);
    return 0;
}

