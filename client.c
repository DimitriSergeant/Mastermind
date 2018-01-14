/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'�cran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "fon.h"   		/* primitives de la boite a outils */


#define WHITE "\033[00m"
#define NOIR "\033[00;30m"
#define ROUGE "\033[01;31m"
#define VERT "\033[01;32m"
#define JAUNE "\033[01;33m"
#define BLEU "\033[01;34m"
#define PURPLE "\033[01;35m"
#define CYAN "\033[01;36m"

#define NORMAL "\033[01;30m"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define PROTOCOLE_DEFAUT "TCP"

#define NB_COULEURS_MAX 50

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur,service);
}


int connection(char *serveur,char *service,char *protocole){
	struct sockaddr_in *adr_serveur;
	adr_serveur = (struct sockaddr_in *) malloc (sizeof(struct sockaddr_in));
	adr_serveur->sin_family = AF_INET;

	int num_socket = h_socket(AF_INET,SOCK_STREAM);

	adr_socket(service, NULL, SOCK_STREAM, &adr_serveur);

	h_connect(num_socket,adr_serveur);

	return num_socket;
}


void ecrire_en_couleur(char car){
	switch(car){
		case('W') : printf(WHITE"W");	break;
		case('R') : printf(ROUGE"R");	break;
		case('V') : printf(VERT"V");	break;
		case('J') : printf(JAUNE"J");	break;
		case('B') : printf(BLEU"B");	break;
		case('P') : printf(PURPLE"P");	break;
		case('C') : printf(CYAN"C");	break;
		default :	printf(NORMAL" ");	break;
	}
}

void init_plateau(char* plateau, int nb_coup){
	int i;
	for(i=0;i<(nb_coup*6);i++){
		plateau[i]=' ';
	}
}

void init_tampon(char * tampon){
	tampon[0]=' ';
	tampon[1]=' ';
	tampon[2]=' ';
	tampon[3]=' ';
}

int entree_valide_7(char car){
	char pions_disponibles[7] ={'R','V','B','P','J','W','C'};

	int i;
	for(i=0;i<7;i++){
		if(car == pions_disponibles[i]){
			return 1;
		}
	}
	return 0;
}


int entree_valide_4(char car){
	char pions_disponibles[4] ={'R','V','B','P'};

	int i;
	for(i=0;i<4;i++){
		if(car == pions_disponibles[i]){
			return 1;
		}
	}
	return 0;
}

int valide_7(char * plateau, int coup){
	return (entree_valide_7(plateau[coup*6]) &&
			entree_valide_7(plateau[(coup*6)+1]) &&
			entree_valide_7(plateau[(coup*6)+2]) &&
			entree_valide_7(plateau[(coup*6)+3]));
}

int valide_4(char * plateau, int coup){
	return (entree_valide_4(plateau[coup*6]) &&
			entree_valide_4(plateau[(coup*6)+1]) &&
			entree_valide_4(plateau[(coup*6)+2]) &&
			entree_valide_4(plateau[(coup*6)+3]));
}

void affichage(char* plateau, int nb_coup){
	int i;
	printf(WHITE"        Mal placés"NORMAL"  |? ? ? ?|   "ROUGE"Bien placés \n");
	printf(NORMAL"                    ---------                                       \n");
	for(i=0;i<(nb_coup*6);i+=6){
		printf(WHITE"              %c "NORMAL"    |",plateau[i+4]);
		ecrire_en_couleur(plateau[i]);	printf(" ");
		ecrire_en_couleur(plateau[i+1]);printf(" ");
		ecrire_en_couleur(plateau[i+2]);printf(" ");
		ecrire_en_couleur(plateau[i+3]);
		printf(NORMAL"|  "ROUGE"   %c \n"NORMAL, plateau[i+5]);
	}
}



/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */
{
	char *protocole = PROTOCOLE_DEFAUT; /* protocole par defaut */
	char tampon[50]; // On a défini n max à 10
	char plateau[80]; //12 coups*(4 combinaisons + 2 languettes) = 72, 80 au cas o�
	char reponse;
	int coup, nb_coup,nb_couleurs;
	int gagne = 0;
	int num_socket = connection(serveur,service,protocole);

		printf("1 -> (Recrue) Combinaison de 4 pions parmi 4 couleurs à deviner , 12 coups pour trouver ! \n");
		printf("2 -> (Soldat)  Combinaison de 4 pions parmi 4 couleurs à deviner , 8 coups pour trouver ! \n");
		printf("3 -> (Commandot) Combinaison de 4 pions parmi 7 couleurs à deviner, 12 coups pour trouver ! \n");
		printf("4 -> (Vétéran) Combinaison de 4 pions parmi 7 couleurs à deviner, 8 coups pour trouver ! \n");
		printf("Quel est votre grade? \n");
		scanf(" %c", tampon);

		h_writes(num_socket, tampon, 1);
		if((tampon[0]=='2') || (tampon[0]=='4')){
			nb_coup = 8;
		}
		else{
			nb_coup = 12;
		}

		if((tampon[0]=='3') || (tampon[0]=='4')){
			nb_couleurs = 7;
		}else{
			nb_couleurs = 4;
		}

		init_plateau(plateau, nb_coup);
		affichage(plateau, nb_coup);
		printf("%s\n","Formation autorisee : R R B V et non RRBV" );
		for(coup=0;coup<nb_coup;coup++){
			if(nb_couleurs == 7){
				printf("Soldats disponibles: "ROUGE"R"VERT" V"BLEU" B"PURPLE" P"JAUNE" J"WHITE" W"CYAN" C\n" NORMAL);
				printf("Choisissez votre formation! ");
				scanf("%s %s %s %s",&plateau[coup*6], &plateau[(coup*6)+1], &plateau[(coup*6)+2], &plateau[(coup*6)+3]);
				while(!valide_7(plateau, coup)){
					printf("Formation inconnue au bataillon: Choisissez votre formation! ");
					scanf("%s %s %s %s",&plateau[coup*6], &plateau[(coup*6)+1], &plateau[(coup*6)+2], &plateau[(coup*6)+3]);
				}
			}else{
				printf("Soldats disponibles: "ROUGE"R"VERT" V"BLEU" B"PURPLE" P \n" NORMAL);
				printf("Choisissez votre formation! ");
				scanf("%s %s %s %s",&plateau[coup*6], &plateau[(coup*6)+1], &plateau[(coup*6)+2], &plateau[(coup*6)+3]);
				while(!valide_4(plateau, coup)){
					printf("Formation inconnue au bataillon: Choisissez votre formation! ");
					scanf("%s %s %s %s",&plateau[coup*6], &plateau[(coup*6)+1], &plateau[(coup*6)+2], &plateau[(coup*6)+3]);
				}
			}


			//Envois de la combinaison au r�seau
			tampon[0]=plateau[coup*6];
			tampon[1]=plateau[(coup*6)+1];
			tampon[2]=plateau[(coup*6)+2];
			tampon[3]=plateau[(coup*6)+3];
			h_writes(num_socket, tampon, 4 );

			//Lecture du verdict
			init_tampon(tampon);
			h_reads(num_socket,tampon,2);
			plateau[(coup*6)+4] = tampon[0]; //languette_B
			plateau[(coup*6)+5] = tampon[1]; //languette_R

			affichage(plateau, nb_coup);
			if (plateau[(coup*6)+5]=='4'){   //si languette_R == 4 cad que la combinaison est trouvée
				printf( VERT  "----------TANGO CHARLIE,ON RENTRE A LA BASE!----------\n");
				printf( WHITE "******** INSERT COIN *******\n" NORMAL);
				coup=nb_coup;
				gagne = 1;
			}

		}
		if(!gagne){
			printf( ROUGE  "-----------LA DEFAITE N'EST PAS ENVISAGEABLE SOLDAT----------\n");
			printf( WHITE  "******** INSERT COIN *******\n" NORMAL);
		}

	h_close(num_socket);
 }

/*****************************************************************************/
