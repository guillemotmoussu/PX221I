
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "userNetwork.h"

// Configuration pour la connexion
#define IPADDRESS "127.0.0.1"
#define PORT 8080
#define USERID 5
#define PASSWORD "binome5"

//Variables de jeu
#define Infinity 120
#define EvalWin 110
#define MaxDepth 7
#define BitMask 0x1UL //Pour faire des opérations bit à bit dans le plateau

struct Game
{
    unsigned long int Disks; //Pions présents sur le plateau
    unsigned long int Color; //Couleur des pions présents
    char Coords;             //Stocke le joueur actif, les erreurs, et des coordonnées
};

/**
 * @brief Affiche le plateau dans le terminal
 * @param Game
 */
void PrintBoard(struct Game Game)
{
    printf("\n   # A B C D E F G H #\n");
    for(char y=0;y<8;y++)
    {
        printf("   %i",y+1);
        for(char x=0;x<8;x++)
        {
            printf(" %c",!(Game.Disks&(BitMask<<(y*8+x)))?'-':(Game.Color&(BitMask<<(y*8+x))?'X':'O'));
        }
        printf(" %i\n",y+1);
    }
    printf("   # A B C D E F G H #\n\n");
    return;
}

/**
 * @brief Joue un coup sur le plateau
 * @param Game
 * @param dx
 * @return 0 si il y a quelque chose à retourner, 1 sinon
 */
char FlipMove(struct Game *Game, char dx)
{
    char i=1;
    while   (
            ((Game->Coords&127)+i*dx>=0)&&((Game->Coords&127)+i*dx<=63)
            &&((dx!=-7&&dx!=+1&&dx!=+9)||(((Game->Coords&127)+i*dx)%8!=0))
            &&((dx!=+7&&dx!=-1&&dx!=-9)||(((Game->Coords&127)+i*dx)%8!=7))
            )
    {
        if(Game->Disks&(BitMask<<((Game->Coords&127)+i*dx)))
        {
            if(Game->Coords&128)
            {
                if(Game->Color&(BitMask<<((Game->Coords&127)+i*dx)))
                {
                    for(char f=1;f<i;f++) Game->Color=Game->Color^(BitMask<<((Game->Coords&127)+f*dx));
                    return i==1;
                }
                else i++;
            }
            else
            {
                if(Game->Color&(BitMask<<((Game->Coords&127)+i*dx))) i++;
                else
                {
                    for(char f=1;f<i;f++) Game->Color=Game->Color^(BitMask<<((Game->Coords&127)+f*dx));
                    return i==1;
                }
            }
        }
        else return 1;
    }
    return 1;
}

/**
 * @brief Permet de jouer un coup
 * @param Game
 * @return 0 si le coup est légal, 1 sinon
 */
char ExeMove(struct Game *Game)
{
    if(Game->Disks&(BitMask<<(Game->Coords&127))) return 1;
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
        Game->Disks=Game->Disks|(BitMask<<(Game->Coords&127));
        if(Game->Coords&128) Game->Color=Game->Color|(BitMask<<(Game->Coords&127));
    }
    return MoveLegal;
}

/**
 * @brief Permet d'évaluer une position (plus rapidement) quand la partie est terminée
 * @param Game
 * @return Différence entre le nombre de pions du joueur actuel et celui de l'adversaire
 */
char FinalEval(struct Game Game)
{
    char eval=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(BitMask<<i))
        {
            if(Game.Coords&128)
            {
                if(Game.Color&(BitMask<<i)) eval++;
                else eval--;
            }
            else
            {
                if(Game.Color&(BitMask<<i)) eval--;
                else eval++;
            }
        }
    }
    return eval;
}

/**
 * @brief Fonction d'évaluation du plateau
 * @param Game
 * @return Score du plateau à un moment donné (entre -100 et 100)
 */
char BotEval(struct Game Game)
{
    static const int TabForce[]=
    {
         700,-30,30 ,10 ,10 ,30 ,-30,700,
         -30,-40, 0 , 0 , 0 , 0 ,-40,-30,
          30, 0 , 1 , 2 , 2 , 1 , 0 , 30,
          10, 0 , 2 ,15 ,15 , 2 , 0 , 10,
          10, 0 , 2 ,15 ,15 , 2 , 0 , 10,
          30, 0 , 1 , 2 , 2 , 1 , 0 , 30,
         -30,-40, 0 , 0 , 0 , 0 ,-40,-30,
         700,-30,30 ,10 ,10 ,30 ,-30,700
    };
    char eval=0;
    int YouCorners=0;
    int AdvCorners=0;
    int YouNumber=0;
    int AdvNumber=0;
    int YouForce=0;
    int AdvForce=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(BitMask<<i))
        {
            if(Game.Coords&128)
            {
                if(Game.Color&(BitMask<<i))
                {
                    YouForce+=TabForce[i+0];
                    YouNumber++;
                    if(i==0||i==7||i==56||i==63) YouCorners++;
                }
                else
                {
                    AdvForce+=TabForce[i+0];
                    AdvNumber++;
                    if(i==0||i==7||i==56||i==63) AdvCorners++;
                }
            }
            else
            {
                if(Game.Color&(BitMask<<i))
                {
                    AdvForce+=TabForce[i+0];
                    AdvNumber++;
                    if(i==0||i==7||i==56||i==63) AdvCorners++;
                }
                else
                {
                    YouForce+=TabForce[i+0];
                    YouNumber++;
                    if(i==0||i==7||i==56||i==63) YouCorners++;
                }
            }
        }
    }
    if(YouForce<0) YouForce=0;
    if(AdvForce<0) AdvForce=0;
    eval=
        (
        (20*((YouCorners+AdvCorners==0)?0:((100*(YouCorners-AdvCorners))/(YouCorners+AdvCorners))))+
        (10*((YouNumber+AdvNumber==0)?0:((100*(YouNumber-AdvNumber))/(YouNumber+AdvNumber))))+
        (70*((YouForce+AdvForce==0)?0:((100*(YouForce-AdvForce))/(YouForce+AdvForce))))
        )/100;
    if(eval<-100) eval=-80;
    if(eval>100) eval=80;
    return eval;
}

/**
 * @brief Calcule les branches de l'arbre pour déterminer l'évaluation du joueur actuel
 * @param Game
 * @param depth
 * @param TopEval
 * @param CutEval
 * @return ??
 */
char GrowTree(struct Game Game, char depth, char TopEval, char CutEval)
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

/**
 * @brief Détermine le coup à jouer et le joue
 * @param Game 
 * @return ??
 */
char BotMove(struct Game *Game)
{
    printf("Our bot %c is thinking...\n",Game->Coords&128?'X':'O');
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
        return BestMove;
    }
    printf(" Bot %c played: %c%c (eval %i)\n",Game->Coords&128?'X':'O',((BestMove&127)%8)+'A',((BestMove&127)/8)+'1',MaxEval);
    Game->Coords=BestMove;
    ExeMove(Game);
    return (BestMove&63);
}

/**
 * @brief Fonction qui détermine si la partie est terminée (personne ne peut jouer)
 * Cette fonction n'est plus utilisée dans les dernières versions du programme
 * @param Game 
 * @return ??
 */
char GameOver(struct Game Game)
{
    for(Game.Coords=Game.Coords&128;!(Game.Coords&64);Game.Coords++) if(!ExeMove(&Game)) return 0;
    printf("Seems like player %c can't play...\n",Game.Coords&128?'X':'O');
    for(Game.Coords=Game.Coords^192;!(Game.Coords&64);Game.Coords++) if(!ExeMove(&Game)) return 0;
    printf("Seems like player %c can't play either !\n",Game.Coords&128?'X':'O');
    return 1;
}

/**
 * @brief Fonction qui calcule le score final du plateau
 * 
 * @param Game 
 */
void Score(struct Game Game)
{
    PrintBoard(Game);
    char Disk1=0,Disk2=0,Empty=0;
    for(char i=0;i<64;i++)
    {
        if(Game.Disks&(BitMask<<i))
        {
            if(Game.Color&(BitMask<<i)) Disk1++;
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
    int Wins=0;
    for(int i=0;i<100;i++)
    {
        printf("Starting Game n°%i\n",i+1);
        struct Game Game={0,0,128};
        Game.Disks=Game.Disks| (BitMask<<(3*8+3));
        Game.Color=Game.Color&~(BitMask<<(3*8+3));
        Game.Disks=Game.Disks| (BitMask<<(3*8+4));
        Game.Color=Game.Color| (BitMask<<(3*8+4));
        Game.Disks=Game.Disks| (BitMask<<(4*8+3));
        Game.Color=Game.Color| (BitMask<<(4*8+3));
        Game.Disks=Game.Disks| (BitMask<<(4*8+4));
        Game.Color=Game.Color&~(BitMask<<(4*8+4));
        srand(time(NULL));

        // Old server config: {IP: "192.168.130.9" - PT: 8010 - ID: 5 - PW: "KZB46g"}
        game *Server_Game;
        Server_Game = allocateGameOthello();
        Server_Game->userId = USERID;
        Server_Game->address = IPADDRESS;
        Server_Game->port = PORT;
        if (registerGameOthello(Server_Game,PASSWORD) < 0)
        {exit(-1);} // test de l'authentification auprès du serveur
        if (startGameOthello(Server_Game) < 0)
        { // cet appel est bloquant en attendant un adversaire
            printf("error Starting Game\n");
            exit(-1);
        }

        printf("I am player %s !\n",(Server_Game->myColor==0)?"black":"white");
        // debut de partie
        while (Server_Game->state == PLAYING && !feof(stdin))
        {
            PrintBoard(Game); //On affiche notre plateau
            if (Server_Game->myColor != Server_Game->currentPlayer)
            { // attente du coup de l'adversaire
            printf("Distant bot %c is thinking...\n",(Server_Game->myColor)?'X':'O');
                if (waitMoveOthello(Server_Game) == 0)
                {
                    printf("Game status %d: \t", Server_Game->state);
                    if (Server_Game->state == PLAYING)
                    {
                        printf("Received move from server %d (x=%d,y=%d)\n",Server_Game->move,Server_Game->move%8,Server_Game->move/8); 
                        int move = Server_Game->move; //sauvegarder coup adversaire dans notre structure
                        if (move != 64)
                        {
                            Game.Coords=(Game.Coords&128)+move;
                            ExeMove(&Game);
                        }
                    }
                }
            }
            else
            {
                Server_Game->move = 65; //Si botmove correct cette valeur est modifiée, sinon cela terminera la partie.
                Server_Game->move = BotMove(&Game); //Jouer notre coup et le sauvegarder notre coup dans la structure du serveur
                doMoveOthello(Server_Game);
            }
            Game.Coords=Game.Coords^128;
            Server_Game->currentPlayer = !(Server_Game->currentPlayer); // on change de joueur
        }
        // fin de partie
        printf("Final game status = %d\n", Server_Game->state);
        if(Server_Game->state==3) Wins++;
        Score(Game); //Afficher score
        freeGameOthello(Server_Game);
        printf("Parties: %i, Victoires: %i, Defaites: %i\n", i+1, Wins, i+1-Wins);
    }
    return 0;
}
