#include "OBEP.h"
#include "MyOwnSql.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Helper to remove trailing newlines, carriage returns, and spaces
static void clean_string(char *str) {
    if (!str) return;
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\r' || str[len - 1] == '\n' || str[len - 1] == ' ')) {
        str[len - 1] = '\0';
        len--;
    }
}

//////////////////////////////////////////////////

////////////////////////////////////////
//// MUTEX FOR SECURITY ////////////////
////////////////////////////////////////

// Connected clients tracking
int clients[NB_MAX_CLIENTS];
int nbClients = 0;
pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

int estPresent(int socket) {
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++) {
        if (clients[i] == socket) { 
            indice = i; 
            break; 
        }
    }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}

void ajoute(int socket) {
    pthread_mutex_lock(&mutexClients);
    if (nbClients < NB_MAX_CLIENTS)
    {
        clients[nbClients] = socket;
        nbClients++;
    }
    pthread_mutex_unlock(&mutexClients);
}

void retire(int socket) {
    pthread_mutex_lock(&mutexClients);
    int pos = -1;
    for (int i = 0; i < nbClients; i++) {
        if (clients[i] == socket) {
            pos = i;
            break;
        }
    }
    if (pos != -1) {
        for (int i = pos; i < nbClients - 1; i++) {
            clients[i] = clients[i + 1];
        }
        nbClients--;
    }
    pthread_mutex_unlock(&mutexClients);
}

void OBEP_Close() {
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++) {
        close(clients[i]);
    }
    nbClients = 0;
    pthread_mutex_unlock(&mutexClients);
}


///////////////////////////////////////////
///////////////////////////////////////////




// Command enumeration for switch-case //

typedef enum {
    CMD_UNKNOWN = 0,
    CMD_LOGIN = 1,
    CMD_LOGOUT = 2,
    CMD_GET_AUTHORS = 3,
    CMD_GET_SUBJECTS = 4,
    CMD_ADD_AUTHOR = 5,
    CMD_ADD_SUBJECT = 6,
    CMD_ADD_BOOK = 7
} CommandType;

// Maps string commands to numeric enums
static CommandType parse_command(const char *cmd_str) {
    if (strcmp(cmd_str, "LOGIN") == 0) return CMD_LOGIN;
    if (strcmp(cmd_str, "LOGOUT") == 0) return CMD_LOGOUT;
    if (strcmp(cmd_str, "GET_AUTHORS") == 0) return CMD_GET_AUTHORS;
    if (strcmp(cmd_str, "GET_SUBJECTS") == 0) return CMD_GET_SUBJECTS;
    if (strcmp(cmd_str, "ADD_AUTHOR") == 0) return CMD_ADD_AUTHOR;
    if (strcmp(cmd_str, "ADD_SUBJECT") == 0) return CMD_ADD_SUBJECT;
    if (strcmp(cmd_str, "ADD_BOOK") == 0) return CMD_ADD_BOOK;
    return CMD_UNKNOWN;
}

bool OBEP(char* requete, char* reponse, int socket, MYSQL *mysql) {
    // 1. Clean incoming string from Telnet formatting (\r\n)
    clean_string(requete);

    char reqCopy[2048];
    strncpy(reqCopy, requete, sizeof(reqCopy));

    char *ptr = strtok(reqCopy, "#");
    if (!ptr) {
        sprintf(reponse, "ERROR#Invalid request format");
        return true;
    }

    CommandType cmd = parse_command(ptr);

    // 2. LOGIN (Handled before auth check since client isn't logged in yet)
    
    if (cmd == CMD_LOGIN) {
        char *user = strtok(NULL, "#");
        char *password = strtok(NULL, "#");

        clean_string(user);
        clean_string(password);

        if (!user || !password) {
            sprintf(reponse, "LOGIN#ko#Missing parameters");
            return true;
        }

        if (estPresent(socket) >= 0) {
            sprintf(reponse, "LOGIN#ko#Client déjà loggé !");
            return true;
        }

        if (db_login(mysql, user, password)) {
            sprintf(reponse, "LOGIN#ok");
            ajoute(socket);
        } else {
            sprintf(reponse, "LOGIN#ko#Mauvais identifiants !");
        }
        return true;
    }

    // Verify authentication for all subsequent commands
    if (estPresent(socket) < 0) {
        sprintf(reponse, "ERROR#Non authentifié");
        return true;
    }

    // 3. Clean switch-case router for all other commands
    switch (cmd) {
        case CMD_LOGOUT:
            retire(socket);
            sprintf(reponse, "LOGOUT#ok");
            return false;

        case CMD_GET_AUTHORS:
            if (!db_get_authors(mysql, reponse, sizeof(reponse))) {
                sprintf(reponse, "GET_AUTHORS#ko#Database error");
            }
            return true;

        case CMD_GET_SUBJECTS:
            if (!db_get_subjects(mysql, reponse, sizeof(reponse))) {
                sprintf(reponse, "GET_SUBJECTS#ko#Database error");
            }
            return true;

        case CMD_ADD_AUTHOR: {
            char *lastName = strtok(NULL, "#");
            char *firstName = strtok(NULL, "#");
            clean_string(lastName);
            clean_string(firstName);

            if (!lastName || !firstName) {
                sprintf(reponse, "ADD_AUTHOR#ko#Missing fields");
                return true;
            }

            long newId = db_add_author(mysql, lastName, firstName);
            sprintf(reponse, "ADD_AUTHOR#ok#%ld", newId);
            return true;
        }

        case CMD_ADD_SUBJECT: {
            char *name = strtok(NULL, "#");
            clean_string(name);

            if (!name) {
                sprintf(reponse, "ADD_SUBJECT#ko#Missing name");
                return true;
            }

            long newId = db_add_subject(mysql, name);
            sprintf(reponse, "ADD_SUBJECT#ok#%ld", newId);
            return true;
        }

        case CMD_ADD_BOOK: {
            char *authorId = strtok(NULL, "#");
            char *subjectId = strtok(NULL, "#");
            char *title = strtok(NULL, "#");
            char *isbn = strtok(NULL, "#");
            char *pageCount = strtok(NULL, "#");
            char *stockQuantity = strtok(NULL, "#");
            char *price = strtok(NULL, "#");
            char *publishYear = strtok(NULL, "#");

            clean_string(authorId);
            clean_string(subjectId);
            clean_string(title);
            clean_string(isbn);
            clean_string(pageCount);
            clean_string(stockQuantity);
            clean_string(price);
            clean_string(publishYear);

            if (!authorId || !subjectId || !title || !isbn || 
                !pageCount || !stockQuantity || !price || !publishYear) {
                sprintf(reponse, "ADD_BOOK#ko#Missing arguments");
                return true;
            }

            long newId = db_add_book(mysql, authorId, subjectId, title, isbn, 
                                      pageCount, stockQuantity, price, publishYear);
            sprintf(reponse, "ADD_BOOK#ok#%ld", newId);
            return true;
        }

        case CMD_UNKNOWN:
        default:
            sprintf(reponse, "ERROR#Unknown command");
            return true;
    }
}