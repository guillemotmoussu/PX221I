/**
 * \file userNetwork.h
 * \brief Fichier d'entête de la librairie networkOthello
 * \author Q.Giorgi
 * \version 0.1
 * \date Mars 2012
 *
 * Fichier d'entêtre de la librairie networkOthello, contient les définitions de type, les prototypes de fonctions
 *
 */
#define MAXLEN 128 
#define PASSLEN 11
#define DESCLEN 41
#define CHALLEN 20


/**
 * \enum states 
 * \brief Etats possibles de la connexion au serveur. 
*/
typedef enum {
	CONNECT=1, 	/*!< Connexion en cours. */
	PLAYING, 	/*!< Partie en cours */
	WIN,		/*!< Partie gagnée */
	LOOSE,		/*!< Partie perdue */
	DRAW, 		/*!< Partie egale */
	ERROR 		/*!< Erreur durant la partie */
} states; 

/**
 * \struct ServerGame 
 * \brief Structure contenant les paramètres nécessaires pour un joueur.
 * Cette structure est passée en argument des fonctions de la librairie qui l'utilisent 
 * Soit pour mettre à jour les paramètres, 
*/

typedef struct  {
	char *address; 		/*!< Chaine de caractères contenant l'adresse IP du serveur */
	short port; 		/*!< Le port sur lequel ecoute le serveur */
	int userId; 		/*!< L'identifiant de l'utilisateur comme indiqué dans le fichier clients */
	int move;		/*!< Le coup à joué ou joué */
	short myColor; 		/*!< La couleur du joueur (black ou white) */
	short currentPlayer; 	/*!< Désigne le joueur qui doit jouer */
	char buf[MAXLEN]; 	/*!< Buffer de données */
	int len; 		/*!< longueur des données */
	states state; 		/*!< L'état de la connexion */
	int socket; 		/*!< La socket utilisée pour la connexion */
} ServerGame; 

int registerGameOthello(ServerGame *g,char *password); 
int waitMoveOthello(ServerGame *g);
ServerGame *allocateGameOthello();
int freeGameOthello(ServerGame *g);
int startGameOthello(ServerGame *g);
int doMoveOthello(ServerGame *g);
