
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "userNetwork.h"

#define Infinity 120
#define EvalWin 110
#define MaxDepth 7
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
    while   (
            ((Game->Coords&127)+i*dx>=0)&&((Game->Coords&127)+i*dx<=63)
            &&((dx!=-7&&dx!=+1&&dx!=+9)||(((Game->Coords&127)+i*dx)%8!=0))
            &&((dx!=+7&&dx!=-1&&dx!=-9)||(((Game->Coords&127)+i*dx)%8!=7))
            )
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
    char MoveLegal=1;
    if(!FlipMove(Game,-7)) MoveLegal=0; // Going NE
    if(!FlipMove(Game,-8)) MoveLegal=0; // Going FN
    if(!FlipMove(Game,-9)) MoveLegal=0; // Going NW
    if(!FlipMove(Game,-1)) MoveLegal=0; // Going FW
    if(!FlipMove(Game,+7)) MoveLegal=0; // Going SW
    if(!FlipMove(Game,+8)) MoveLegal=0; // Going FS
    if(!FlipMove(Game,+9)) MoveLegal=0; // Going SE
    if(!FlipMove(Game,+1)) MoveLegal=0; // Going FE
    if(MoveLegal==0)
    {
        Game->Disks=Game->Disks|(Game->BitMask<<(Game->Coords&127));
        if(Game->Coords&128) Game->Color=Game->Color|(Game->BitMask<<(Game->Coords&127));
    }
    return MoveLegal;
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

char BotEval(struct Game Game)
{
    char eval=0;
    int TabForce[]=
    {
       600,-150,30 ,10 ,10 ,30 ,-150, 600,
      -150,-250, 0 , 0 , 0 , 0 ,-250,-150,
       30 ,  0 , 1 , 2 , 2 , 1 ,  0 , 30 ,
       10 ,  0 , 2 ,15 ,15 , 2 ,  0 , 10 ,
       10 ,  0 , 2 ,15 ,15 , 2 ,  0 , 10 ,
       30 ,  0 , 1 , 2 , 2 , 1 ,  0 , 30 ,
      -150,-250, 0 , 0 , 0 , 0 ,-250,-150,
       600,-150,30 ,10 ,10 ,30 ,-150, 600
    };
    char YouCorners=0;
    char AdvCorners=0;
    char YouNumber=0;
    char AdvNumber=0;
    int force=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(Game.BitMask<<i))
        {
            if(Game.Coords&128)
            {
                if(Game.Color&(Game.BitMask<<i))
                {
                    force+=TabForce[i+0];
                    YouNumber++;
                    if(i==0||i==7||i==56||i==63) YouCorners++;
                }
                else
                {
                    force-=TabForce[i+0];
                    AdvNumber++;
                    if(i==0||i==7||i==56||i==63) AdvCorners++;
                }
            }
            else
            {
                if(Game.Color&(Game.BitMask<<i))
                {
                    force-=TabForce[i+0];
                    AdvNumber++;
                    if(i==0||i==7||i==56||i==63) AdvCorners++;
                }
                else
                {
                    force+=TabForce[i+0];
                    YouNumber++;
                    if(i==0||i==7||i==56||i==63) YouCorners++;
                }
            }
        }
    }
    eval=
        (
        (50*((YouCorners+AdvCorners==0)?0:((YouCorners-AdvCorners)/(YouCorners+AdvCorners))))+
        (20*((YouNumber+AdvNumber==0)?0:((YouNumber-AdvNumber)/(YouNumber+AdvNumber))))+
        (30*(force/50))
        )/100;
    assert(eval<100 && eval >-100);
    return eval;
}

char GrowTree(struct Game Game, char depth, char TopEval, char CutEval)
//Calcule les branches de l'arbre pour déterminer l'évaluation du joueur actuel
{
    if (depth<1) return BotEval(Game);
    char MaxEval=-Infinity;
    char NewEval=0;
    unsigned long int SaveDisks=Game.Disks; //sauvegardes
    unsigned long int SaveColor=Game.Color;
    for(Game.Coords=~(Game.Coords|127);!(Game.Coords&64);Game.Coords++)
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
    }
    if(MaxEval!=-Infinity) return -MaxEval; //si on a trouvé une valeur max pour le joueur adverse on la retourne
    for(Game.Coords=Game.Coords^192;!(Game.Coords&64);Game.Coords++)
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
    for(Game->Coords=Game->Coords&128;!(Game->Coords&64);Game->Coords++)
    {
        if(!ExeMove(Game))
        {
            NewEval=GrowTree(*Game,MaxDepth,-Infinity,Infinity);
            assert(NewEval!=Infinity);
            assert(NewEval!=-Infinity);
            if(MaxEval<NewEval||((MaxEval==NewEval)&&(0b11!=(0b11&rand()))))
            {
                MaxEval=NewEval;
                BestMove=Game->Coords;
            }
            Game->Disks=SaveDisks;
            Game->Color=SaveColor;
        }
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
    for(Game.Coords=Game.Coords&128;!(Game.Coords&64);Game.Coords++) if(!ExeMove(&Game)) return 0;
    printf("Seems like player %c can't play...\n",Game.Coords&128?'X':'O');
    for(Game.Coords=Game.Coords^192;!(Game.Coords&64);Game.Coords++) if(!ExeMove(&Game)) return 0;
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

    // ajout du réseau

    game *Server_Game;
	int move;

	Server_Game = allocateGameOthello();
	Server_Game->userId=9;
	Server_Game->address="192.168.132.18";
	Server_Game->port = 8080;

	if (registerGameOthello(Server_Game, "binome9") < 0)
	{ // test de l'authentification auprès du serveur
		exit(-1);
	}

	if (startGameOthello(Server_Game) < 0)
	{ // cet appel est bloquant en attendant un adversaire
		printf("error Starting Game\n");
		exit(-1);
	}

	printf("I am player %s\n",(Server_Game->myColor==0)?"black":"white"); 

	// debut de partie
	while (Server_Game->state == PLAYING && !feof(stdin))
	{
		//afficher notre plateau

		if (Server_Game->myColor != Server_Game->currentPlayer)
		{ // attente du coup de l'adversaire
			if (waitMoveOthello(Server_Game) == 0)
			{
				printf("Game status %d: \t", Server_Game->state);
				if (Server_Game->state == PLAYING)
				{
					//printf("Received move from server %d (x=%d,y=%d)\n",g->move,g->move%8,g->move/8); 
					//sauvegarder coup adversaire dans notre structure
				}
			}
		}
		else
		{
			Server_Game->move = 65; // si botmove correct cette valeur est modifiée, sinon cela terminera la partie.
			//Jouer notre coup 
			/*
			printf("Enter your move:\n");
			scanf("%d",&(g->move)); 
			printf("playing move %d (x=%d,y=%d)\n",g->move,g->move%8,g->move/8);
			*/
			doMoveOthello(Server_Game);
		}
		Server_Game->currentPlayer = !(Server_Game->currentPlayer); // on change de joueur
	}
	// fin de partie
	printf("Final game status = %d\n", Server_Game->state);
	//Afficher score
	freeGameOthello(Server_Game);

    
    return 0;
}