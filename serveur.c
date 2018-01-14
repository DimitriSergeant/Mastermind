/******************************************************************************/
/*			Application: MasterMind			              */
/******************************************************************************/
/*									      																										*/
/*			 programme  SERVEUR 				      																		*/
/*									      																										*/
/******************************************************************************/
/*									      																										*/
/*		Auteurs :  Valette Léo & Sergeant Dimitri             						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/

#include<stdio.h>
#include <curses.h>


#include <sys/signal.h>
#include <sys/wait.h>
#include <string.h> 		//pour faire la comparaison de 2 tableaux
#include <stdlib.h>			//pour le random
#include <time.h>
#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "TCP"


void serveur_appli (char *service);   /* programme serveur */
void connection(char *service, int * socket);


/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou num�ro de port) auquel sera affect�
	ce serveur*/

	serveur_appli(service);
}

void connection(char *service, int * socket){
	struct sockaddr_in *adr_serveur, adr_client;
	adr_client.sin_family = AF_INET;

	//création de la socket
	int num_socket = h_socket(AF_INET,SOCK_STREAM);

	adr_socket(service, NULL, SOCK_STREAM, &adr_serveur);
	//association de la socket à  son adresse
	h_bind(num_socket, adr_serveur);

	//ecoute de la socket
	h_listen(num_socket,50);

	//acceptation de la socket (attente)
	int num_socket_talk = h_accept(num_socket, &adr_client);

	socket[0]=num_socket;
	socket[1]=num_socket_talk;
}

int contenu_dans_tab(int nb, int * tab, int taille_tab){

	int i;
	for(i=0;i<taille_tab;i++){
		if(nb == tab[i]){
			return 1;
		}
	}
	return 0;
}

void init_tab(char * tab){
	tab[0]= ' ';
	tab[1]= ' ';
	tab[2]= ' ';
	tab[3]= ' ';
}

void evaluer_combinaison(char * combinaison_secrette, char * combinaison_utilisateur, int  * languette_B, int  * languette_R){
	int i,j;
	int B=0;
	int R=0;
	int inter_combi[4]={0,0,0,0};

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(combinaison_utilisateur[i]==combinaison_secrette[i]){ 	//son vis à vis est de la mm couleur
				R++;													//couleur && emplacement
				inter_combi[j] = 0; //si jamais il y a eu couleur && !emplacement avant
				j=4;
			}
			else{ //son vis à vis n'est pas de la mm couleur
				if((combinaison_utilisateur[i]==combinaison_secrette[j]) && (inter_combi[j] != 1) && !(combinaison_utilisateur[j]==combinaison_secrette[j])){ //
						//si il y a une correspondance couleur && ce pion n'a pas déjà été répéré && ni trouvé
						inter_combi[j] = 1; //si -> couleur && !emplacement
						j=4;
				}
			}
		}
	}
	B = inter_combi[0]+inter_combi[1]+inter_combi[2]+inter_combi[3];
	languette_B[0] = B;
	languette_R[0] = R;
}


void init_combinaison_secrette_4(char * combinaison){

	char pions_disponibles[4] ={'R','V','B','P'};

	srand(time(NULL)); // initialisation de rand
	int i;
	for(i=0;i<4;i++){
		int indice = rand()%4;
		combinaison[i] = pions_disponibles[indice];
	}

}

void init_combinaison_secrette_7(char * combinaison,int diff){

	char pions_disponibles[7] ={'R','V','B','P','J','W','C'};

	srand(time(NULL)); // initialisation de rand
	int i;
	int tab_indice[4]={7,7,7,7}; //valeur inateignable
	for(i=0;i<4;i++){
		int indice = rand()%7;
		if((diff == '1') || (diff == '3')){
			while(contenu_dans_tab(indice, tab_indice, 4)){
				indice = rand()%7;
			}
		}
		combinaison[i]=pions_disponibles[indice];
		tab_indice[i] = indice;
	}

}

/******************************************************************************/
/* Procedure correspondant au traitemnt du serveur de votre application */

void serveur_appli(char *service){

	char *protocole = PROTOCOLE_DEFAUT;
	int socket[2];
	int num_socket, num_socket_talk;
	//création de la socket et connection
	connection(service, socket);

	num_socket=socket[0];
	num_socket_talk=socket[1];

	//---------------début du mastermind----------------
	char tampon[10];
	int FIN=0;

	//acceptation de la socket (attente)
	h_reads(num_socket_talk, tampon, 1);
	char difficulte = tampon[0];

	char combinaison_secrette[4];
	if(difficulte == '4' || difficulte == '3'){
		init_combinaison_secrette_7(combinaison_secrette,difficulte);
	}else{
		init_combinaison_secrette_4(combinaison_secrette);
	}
	printf("Solution: %s\n", combinaison_secrette);

	while(FIN !=1){
		int languette_B[1];
		int languette_R[1];
		languette_B[1]=0;
		languette_B[1]=0;

		//reception de la combinaison
		init_tab(tampon);
		h_reads(num_socket_talk, tampon, 4);

		//si la combinaison à été trouvé
		evaluer_combinaison(combinaison_secrette,tampon, languette_B, languette_R);

		//prévenir le client
		init_tab(tampon);
		tampon[0]=languette_B[0]+'0';
		tampon[1]=languette_R[0]+'0';
		h_writes(num_socket_talk,tampon,2);

		//savoir si c'est la fin
		if(languette_R[0]==4){
			char replay[1];
			h_reads(num_socket_talk, replay, 1);
			if(replay[0]=='O'){
				serveur_appli(service);
			}
			FIN=1;
		}
	}


	h_close(num_socket_talk);
	h_close(num_socket);
}

/******************************************************************************/
