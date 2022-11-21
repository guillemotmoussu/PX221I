
#include <stdio.h>
#include <stdlib.h>

#define P1 'X'
#define P2 'O'
#define TI '-'
#define ExistP1 'N'
#define ExistP2 'N'
#define EmptyRank {TI,TI,TI,TI,TI,TI,TI,TI}
#define StartBoard {EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank}
#define MaxDepth 3
#define Infinity 120
#define EvalWin 110

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
            if (Game.Board[y+0][x+0]==P1) printf(" X");
            if (Game.Board[y+0][x+0]==P2) printf(" O");
            if (Game.Board[y+0][x+0]==TI) printf(" -");
        }
        printf(" %i\n",y+1);
    }
    printf("   # A B C D E F G H #\n");
    return;
}

void Score(struct Game Game)
{
    PrintBoard(Game);
    char Disk1=0,Disk2=0,Empty=0;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            if (Game.Board[y+0][x+0]==P1) Disk1++;
            if (Game.Board[y+0][x+0]==P2) Disk2++;
            if (Game.Board[y+0][x+0]==TI) Empty++;
        }
    }
    if (Disk1<Disk2) Disk2+=Empty;
    if (Disk2<Disk1) Disk1+=Empty;
    printf("Game finished, score: %i-%i\n",Disk1,Disk2);
    return;
}

char MoveLegal(struct Game Game, char dx, char dy)
{
    char i=1;
    while((Game.x+i<8 || dx!=1) && (Game.y+i<8 || dy!=1) && (Game.x+1>i || dx!=-1) && (Game.y+1>i || dy!=-1))
    {
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==TI) return 1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==Game.Who) return i==1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==P1+P2-Game.Who) i++;
    }
    return 1;
}

char MoveLegalAll(struct Game Game)
{
    if (Game.Board[Game.y+0][Game.x+0]!=TI) return 1; // Tile not empty
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
        printf(" - This is not even a move !");
        return 1;
    }
    if (MoveLegalAll(Game))
    {
        printf(" - This move is illegal !");
        return 1;
    }
    return 0;
}

void ClearBuffer()
{
    for(char buf=0;buf!='\n';buf=buf)
    {
        if (scanf("%c",&buf)==EOF)
        {
            printf("\n [EOF] DO YOU FEEL SMART ?!\n");
            exit(3);
        }
    }
    return;
}

void AskMove(struct Game *Game)
{
    char coords[3];
    do
    {
        printf("\nPlayer %c, place a disk, using A1 - H8 format: ",Game->Who);
        if (scanf("%2s",coords)==EOF)
        {
            printf("\n Wait, EOF ? Really ?\n One time not twice, you fool get out !\n");
            exit(3);
        }
        else ClearBuffer(); // buffer destroyed
        Game->x=coords[0]-'A'; // 'A' = 65
        Game->y=coords[1]-'1'; // '1' = 49
    } while (MoveValid(*Game));
    return;
}

void FlipMove(struct Game *Game, char dx, char dy)
{
    char i=1;
    while((Game->x+i<7 || dx!=1) && (Game->y+i<7 || dy!=1) && (Game->x>i || dx!=-1) && (Game->y>i || dy!=-1))
    {
        if (Game->Board[Game->y+i*dy][Game->x+i*dx]==Game->Who) return;
        if (Game->Board[Game->y+i*dy][Game->x+i*dx]==P1+P2-Game->Who)
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
    if (MoveLegal(*Game,+0,+1)==0) FlipMove(Game,+0,+1); // Going FN
    if (MoveLegal(*Game,+0,-1)==0) FlipMove(Game,+0,-1); // Going FS
    if (MoveLegal(*Game,+1,+1)==0) FlipMove(Game,+1,+1); // Going NE
    if (MoveLegal(*Game,-1,-1)==0) FlipMove(Game,-1,-1); // Going SW
    if (MoveLegal(*Game,-1,+1)==0) FlipMove(Game,-1,+1); // Going NW
    if (MoveLegal(*Game,+1,-1)==0) FlipMove(Game,+1,-1); // Going SE
    return;
}

char GameOver(struct Game *Game)
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
    Game->Who=P1+P2-Game->Who;
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

void BoardCopy(char BoardS[8][8], char BoardD[8][8])
{
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            BoardD[y+0][x+0]=BoardS[y+0][x+0];
        }
    }
    return;
}

char BotEval(struct Game Game)
{
    char eval=0;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            if(Game.Board[y+0][x+0]==Game.Who) eval++;
            if(Game.Board[y+0][x+0]==P1+P2-Game.Who) eval--;
        }
    }
    return eval;
}

char GrowTree(struct Game Game, char depth)
{
    if (depth<1) return BotEval(Game);
    char MaxEval=-Infinity;
    char NewEval=0;
    char BoardSave[8][8];
    BoardCopy(Game.Board,BoardSave);
    Game.Who=P1+P2-Game.Who;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            Game.x=x;
            Game.y=y;
            if (!MoveLegalAll(Game))
            {
                ExeMove(&Game);
                NewEval=GrowTree(Game,depth-1);
                if (MaxEval < NewEval) MaxEval=NewEval;
                BoardCopy(BoardSave,Game.Board);
            }
        }
    }
    if (MaxEval!=-Infinity) return -MaxEval;
    Game.Who=P1+P2-Game.Who;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            Game.x=x;
            Game.y=y;
            if (!MoveLegalAll(Game))
            {
                ExeMove(&Game);
                NewEval=GrowTree(Game,depth-1);
                if (MaxEval < NewEval) MaxEval=NewEval;
                BoardCopy(BoardSave,Game.Board);
            }
        }
    }
    if (MaxEval!=-Infinity) return MaxEval;
    NewEval=BotEval(Game);
    if (NewEval < 0) return -EvalWin;
    if (NewEval > 0) return EvalWin;
    return 0;
}

void BotMove(struct Game *Game)
{
    printf("\n Bot %c thinking...\n",Game->Who);
    char MaxEval=-Infinity;
    char NewEval=0;
    char BoardSave[8][8];
    BoardCopy(Game->Board,BoardSave);
    char BestX=9,BestY=9;
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            Game->x=x;
            Game->y=y;
            if (!MoveLegalAll(*Game))
            {
                ExeMove(Game);
                NewEval=GrowTree(*Game,MaxDepth);
                if (MaxEval < NewEval)
                {
                    MaxEval = NewEval;
                    BestX=x;
                    BestY=y;
                }
                BoardCopy(BoardSave,Game->Board);
            }
        }
    }
    printf(" Bot %c played: %c%c (eval %i)\n",Game->Who,BestX+'A',BestY+'1',MaxEval);
    Game->x=BestX;
    Game->y=BestY;
    ExeMove(Game);
    return;
}

int main()
{
    printf("\nWelcome to my playable version of Reversi !\n");
    struct Game Game={StartBoard,0,0,P1,};
    Game.Board[3][3]=P2;
    Game.Board[3][4]=P1; // 'O' = 79
    Game.Board[4][3]=P1; // 'X' = 88
    Game.Board[4][4]=P2;
    while (GameOver(&Game))
    {
        PrintBoard(Game);
        if ((Game.Who==P1 && ExistP1=='Y')||(Game.Who==P2 && ExistP2=='Y'))
        {
            AskMove(&Game);
            ExeMove(&Game);
        }
        else BotMove(&Game);
        Game.Who=P1+P2-Game.Who; // 79+88=167
    }
    Score(Game);
    return 0;
}

