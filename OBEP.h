#ifndef OBEP_H
#define OBEP_H

#include <stdbool.h>
#include <mysql/mysql.h>

#define NB_MAX_CLIENTS 100

// Initialize database connection for the protocol thread
void OBEP_Init(MYSQL *dbConnection);

// Main request router
bool OBEP(char* requete, char* reponse, int socket, MYSQL *mysql);

// Helper state management functions
int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);
void OBEP_Close();

#endif