#include <stdio.h>
#include <assert.h>

#define EmptyRank {'-','-','-','-','-','-','-','-'}
#define StartBoard {EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank,EmptyRank}

struct Game
{
    char Board[8][8];
    char x;
    char y;
    char Who;
    char Flips;
};

void PrintBoard(struct Game Game)
{
    printf("\n   # A B C D E F G H #\n");
    for(char y=0;y<8;y++)
    {
        printf("   %i",8-y);
        for(char x=0;x<8;x++)
        {
            printf(" %c",Game.Board[7-y][x+0]);
        }
        printf(" %i\n",8-y);
    }
    printf("   # A B C D E F G H #\n");
    return;
}

char MoveLegal(struct Game Game, int dx, int dy)
{
    char i=1;
    while((Game.x+i<8 || dx!=1) && (Game.y+i<8 || dy!=1) && (Game.x+1>i || dx!=-1) && (Game.y+1>i || dy!=-1))
    {
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]=='-') return 1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==Game.Who) return i==1;
        if (Game.Board[Game.y+i*dy][Game.x+i*dx]==167-Game.Who) i++;
        else assert(!printf("MoveLegal failed"));
    }
    return 1;
}

char MoveLegalAll(struct Game Game)
{
    if (Game.Board[Game.y+0][Game.x+0]!='-') return 1; // Tile not empty
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

void AskMove(struct Game *Game)
{
    char coords[3];
    do
    {
        printf("\nPlayer %c, place a disk, using A1 - H8 format: ",Game->Who);
        scanf("%2s",coords);   for(char buf=0;buf!='\n';scanf("%c",&buf)); // buffer destroyed
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
        if (Game->Board[Game->y+i*dy][Game->x+i*dx]==167-Game->Who)
        {
            Game->Board[Game->y+i*dy][Game->x+i*dx]=Game->Who;
            i++;
        }
    }
    assert(!printf("FlipMove failed"));
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

char GameOver(struct Game Game)
{
    for(char y=0;y<8;y++)
    {
        for(char x=0;x<8;x++)
        {
            if (Game.Board[y+0][x+0]=='-') return 1;
        }
    }
    // Unfinished yet, only check if board is full
    return 0;
}

int main()
{
    printf("\nWelcome to my playable version of Reversi !\n");
    struct Game Game={StartBoard,0,0,'X',0};
    Game.Board[3][3]='O';
    Game.Board[3][4]='X'; // 'O' = 79
    Game.Board[4][3]='X'; // 'X' = 88
    Game.Board[4][4]='O';
    while (GameOver(Game))
    {
        PrintBoard(Game);
        AskMove(&Game);
        ExeMove(&Game);
        Game.Who=167-Game.Who; // 79+88=167
    }
    // Add conclusion
    return 0;
}
// Just because I wanted a 150th line !
