#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "userNetwork.h"

/*
struct Game
{
    unsigned long int Disks;        //Pions sur le plateau
    unsigned long int Color;        //Couleur sur le plateau
    unsigned long int BitMask;      //Sert à faire des opérations bit à bit
    char Coords; //Stocke le joueur actif,  les erreurs, et les coordonnées à jouer
};
*/

int main()
{
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
					//sauvegarder coup adversaire dans notre structure
				}
			}
		}
		else
		{
			Server_Game->move = 65; // si botmove correct cette valeur est modifiée, sinon cela terminera la partie.
			//Jouer notre coup 
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
