#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "tcp.h"
int sClient;
void HandlerSIGINT(int s);
void Echange(char* requete, char* reponse);
bool SMOP_Login(const char* user,const char* password);
void SMOP_Logout();
void SMOP_Operation(char op,int a,int b);
int main(int argc,char* argv[])
{
if (argc != 3)
{
printf("Erreur...\n");
printf("USAGE : Client ipServeur portServeur\n");
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
// Connexion sur le serveur
if ((sClient = ClientSocket(argv[1],atoi(argv[2]))) == -1)
{
perror("Erreur de ClientSocket");
exit(1);
}
printf("Connecte sur le serveur.\n");
// Phase de login
char user[50],password[50];
printf("user: "); fgets(user,50,stdin);
user[strlen(user)-1] = 0;
printf("password: "); fgets(password,50,stdin);
password[strlen(password)-1] = 0;
if (!SMOP_Login(user,password))
exit(1);
while(1)
{
int a,b;
char op;
printf("Operation (<CTRL-C> four fin) : ");
fflush(stdin);
scanf("%d %c %d",&a,&op,&b); // pas ouf !
SMOP_Operation(op,a,b);
}
}
//***** Fin de connexion ********************************************
void HandlerSIGINT(int s)
{
printf("\nArret du client.\n");
SMOP_Logout();
close(sClient);
exit(0);
}
//***** Gestion du protocole SMOP ***********************************
bool SMOP_Login(const char* user,const char* password)
{
char requete[200],reponse[200];
bool onContinue = true;
// ***** Construction de la requete *********************
sprintf(requete,"LOGIN#%s#%s",user,password);
// ***** Envoi requete + réception réponse **************
Echange(requete,reponse);
// ***** Parsing de la réponse **************************
char *ptr = strtok(reponse,"#"); // entête = LOGIN (normalement...)
ptr = strtok(NULL,"#"); // statut = ok ou ko
if (strcmp(ptr,"ok") == 0) printf("Login OK.\n");
else
{
ptr = strtok(NULL,"#"); // raison du ko
printf("Erreur de login: %s\n",ptr);
onContinue = false;
}
return onContinue;
}
//*******************************************************************
void SMOP_Logout()
{
char requete[200],reponse[200];
int nbEcrits, nbLus;
// ***** Construction de la requete *********************
sprintf(requete,"LOGOUT");
// ***** Envoi requete + réception réponse **************
Echange(requete,reponse);
// ***** Parsing de la réponse **************************
// pas vraiment utile...
}
//*******************************************************************
void SMOP_Operation(char op,int a,int b)
{
char requete[200],reponse[200];
// ***** Construction de la requete *********************
sprintf(requete,"OPER#%c#%d#%d",op,a,b);
// ***** Envoi requete + réception réponse **************
Echange(requete,reponse);
// ***** Parsing de la réponse **************************
char *ptr = strtok(reponse,"#"); // entête = OPER (normalement...)
ptr = strtok(NULL,"#"); // statut = ok ou ko
if (strcmp(ptr,"ok") == 0)
{
ptr = strtok(NULL,"#"); // résultat du calcul
printf("Résultat = %s\n",ptr);
}
else
{
ptr = strtok(NULL,"#"); // raison du ko
printf("Erreur: %s\n",ptr);
}
}
//***** Echange de données entre client et serveur ******************
void Echange(char* requete, char* reponse)
{
int nbEcrits, nbLus;
// ***** Envoi de la requete ****************************
if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
{
perror("Erreur de Send");
close(sClient);
exit(1);
}
// ***** Attente de la reponse **************************
if ((nbLus = Receive(sClient,reponse)) < 0)
{
perror("Erreur de Receive");
close(sClient);
exit(1);
}
if (nbLus == 0)
{
printf("Serveur arrete, pas de reponse reçue...\n");
close(sClient);
exit(1);
}
reponse[nbLus] = 0;
}