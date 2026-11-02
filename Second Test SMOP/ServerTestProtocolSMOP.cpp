#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "tcp.h"
#include "SMOP.h"
void HandlerSIGINT(int s);
void TraitementConnexion(int sService);
void* FctThreadClient(void* p);
int sEcoute;
int main(int argc,char* argv[])
{
if (argc != 2)
{
printf("Erreur...\n");
printf("USAGE : Serveur portServeur\n");
exit(1);
}
// Armement des signaux
struct sigaction A;
A.sa_flags = 0;
sigemptyset(&A.sa_mask);
A.sa_handler = HandlerSIGINT;
if (sigaction(SIGINT,&A,NULL) == -1)
{
perror("Erreur de sigaction");
exit(1);
}
// Creation de la socket d'écoute
if ((sEcoute = ServerSocket(atoi(argv[1]))) == -1)
{
perror("Erreur de ServeurSocket");
exit(1);
}
// Mise en boucle du serveur
int sService;
pthread_t th;
char ipClient[50];
printf("Demarrage du serveur.\n");
while(1)
{
printf("Attente d'une connexion...\n");
if ((sService = Accept(sEcoute,ipClient)) == -1)
{
perror("Erreur de Accept");
close(sEcoute);
SMOP_Close();
exit(1);
}
printf("Connexion acceptée : IP=%s socket=%d\n",ipClient,sService);
// Creation d'un thread "client" s'occupant du client connecté
int *p = (int*)malloc(sizeof(int));
*p = sService;
pthread_create(&th,NULL,FctThreadClient,(void*)p);
}
}
void* FctThreadClient(void* p)
{
int sService = *((int*)p);
free(p);
printf("\t[THREAD %p] Je m'occupe de la socket%d\n",pthread_self(),sService);
TraitementConnexion(sService);
pthread_exit(NULL);
}
void HandlerSIGINT(int s)
{
printf("\nArret du serveur.\n");
close(sEcoute);
SMOP_Close();
exit(0);
}
void TraitementConnexion(int sService)
{
char requete[200], reponse[200];
int nbLus, nbEcrits;
bool onContinue = true;
while (onContinue)
{
printf("\t[THREAD %p] Attente requete...\n",pthread_self());
// ***** Reception Requete ******************
if ((nbLus = Receive(sService,requete)) < 0)
{
perror("Erreur de Receive");
close(sService);
HandlerSIGINT(0);
}
// ***** Fin de connexion ? *****************
if (nbLus == 0)
{
printf("\t[THREAD %p] Fin de connexion du client.\n",pthread_self());
close(sService);
return;
}
requete[nbLus] = 0;
printf("\t[THREAD %p] Requete recue = %s\n",pthread_self(),requete);
// ***** Traitement de la requete ***********
onContinue = SMOP(requete,reponse,sService);
// ***** Envoi de la reponse ****************
if ((nbEcrits = Send(sService,reponse,strlen(reponse))) < 0)
{
perror("Erreur de Send");
close(sService);
HandlerSIGINT(0);
}
printf("\t[THREAD %p] Reponse envoyee = %s\n",pthread_self(),reponse);
if (!onContinue)
printf("\t[THREAD %p] Fin de connexion de la socket %d\n",pthread_self(),sService);
}
}