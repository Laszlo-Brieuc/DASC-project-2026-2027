#ifndef TCP_H
#define TCP_H
#define TAILLE_MAX_DATA 10000

//Server only - creates + binds server socket to an IP and a port
int ServerSocket(int port);

//
int Accept(int sEcoute,char *ipClient);

//Client only - creates a client socket
int ClientSocket(char* ipServeur,int portServeur);

//
int Send(int sSocket,char* data,int taille);

//
int Receive(int sSocket,char* data);

#endif