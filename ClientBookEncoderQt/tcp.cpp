#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "tcp.h"

int ServerSocket(int port)
{
    int sEcoute;
    struct sockaddr_in adresseServeur;

    // Création de la socket
    sEcoute = socket(AF_INET, SOCK_STREAM, 0);
    if (sEcoute == -1)
    {
        perror("Erreur de ServerSocket()");
        return -1;
    }

    // --- FIX FOR "Address already in use" ---
    // Permits binding to the port instantly even if in TIME_WAIT state
    int option = 1;
    if (setsockopt(sEcoute, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
    {
        perror("Erreur de ServerSocket() (setsockopt)");
        close(sEcoute);
        return -1;
    }
    // ----------------------------------------

    // Remplissage de la structure d'adresse
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family      = AF_INET;
    adresseServeur.sin_port        = htons(port);
    adresseServeur.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(sEcoute, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur de ServerSocket()");
        close(sEcoute);
        return -1;
    }

    // Mise en écoute
    if (listen(sEcoute, 5) == -1)
    {
        perror("Erreur de ServerSocket()");
        close(sEcoute);
        return -1;
    }

    return sEcoute;
}

int Accept(int sEcoute, char *ipClient)
{
    int sService;
    struct sockaddr_in adresseClient;
    socklen_t tailleAdresseClient = sizeof(struct sockaddr_in);

    sService = accept(sEcoute, (struct sockaddr *)&adresseClient, &tailleAdresseClient);
    if (sService == -1)
    {
        perror("Erreur de Accept()");
        return -1;
    }

    // Only copy the IP if the caller provided a buffer
    if (ipClient != NULL)
        strcpy(ipClient, inet_ntoa(adresseClient.sin_addr));

    return sService;
}

int ClientSocket(char *ipServeur, int portServeur)
{
    int sClient;
    struct sockaddr_in adresseServeur;

    // Création de la socket
    sClient = socket(AF_INET, SOCK_STREAM, 0);
    if (sClient == -1)
    {
        perror("Erreur de ClientSocket()");
        return -1;
    }

    // Remplissage de la structure d'adresse du serveur
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_port   = htons(portServeur);

    // Conversion de l'adresse IP (chaîne → binaire)
    if (inet_aton(ipServeur, &adresseServeur.sin_addr) == 0)
    {
        perror("Erreur de ClientSocket() (inet_aton)");
        close(sClient);
        return -1;
    }

    // Connexion au serveur
    if (connect(sClient, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur de ClientSocket() (connect)");
        close(sClient);
        return -1;
    }

    return sClient;
}

int Send(int sSocket, char *data, int taille)
{
    int nbEnvoyes;

    nbEnvoyes = send(sSocket, data, taille, 0);
    if (nbEnvoyes == -1)
    {
        perror("Erreur de Send()");
        return -1;
    }

    return nbEnvoyes;
}

int Receive(int sSocket, char *data)
{
    int nbRecus;

    nbRecus = recv(sSocket, data, TAILLE_MAX_DATA, 0);
    if (nbRecus == -1)
    {
        perror("Erreur de Receive()");
        return -1;
    }

    // On place un '\0' à la fin pour que le buffer puisse être utilisé comme une chaîne C
    data[nbRecus] = '\0';

    return nbRecus;
}