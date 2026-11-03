#include "OBEP.h"
#include "MyOwnSql.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Helper to remove trailing newlines, carriage returns, and spaces
/// Ncessary addition otherwise the code wouldn't handle telnet requests.
static void clean_string(char *str) {
    if (!str) return;
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\r' || str[len - 1] == '\n' || str[len - 1] == ' ')) {
        str[len - 1] = '\0';
        len--;
    }
}

// Connected clients tracking
int clients[NB_MAX_CLIENTS];
int nbClients = 0;
pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

int estPresent(int socket)
{
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
    {
        if (clients[i] == socket)
        { 
            indice = i; 
            break; 
        }
    }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}

void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClients);
    if (nbClients < NB_MAX_CLIENTS)
    {
        clients[nbClients] = socket;
        nbClients++;
    }
    pthread_mutex_unlock(&mutexClients);
}

void retire(int socket)
{
    pthread_mutex_lock(&mutexClients);
    int pos = -1;
    for (int i = 0; i < nbClients; i++)
    {
        if (clients[i] == socket)
        {
            pos = i;
            break;
        }
    }
    if (pos != -1)
    {
        for (int i = pos; i < nbClients - 1; i++)
        {
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

bool OBEP_Login(MYSQL *mysql, const char* user, const char* password)
{
    char query[256];
    snprintf(query, sizeof(query), "SELECT id FROM employees WHERE login='%s' AND password='%s'", user, password);

    if (mysql_query(mysql, query) != 0) {
        fprintf(stderr, "MySQL Query Error: %s\n", mysql_error(mysql));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result) return false;

    int rows = mysql_num_rows(result);
    mysql_free_result(result);

    return (rows > 0);
}

////////////////////////////////
////////////////////////////////

bool OBEP(char* requete, char* reponse, int socket, MYSQL *mysql) {
    // 1. Clean incoming string from formatting (\r\n)
    clean_string(requete);

    char reqCopy[2048];
    strncpy(reqCopy, requete, sizeof(reqCopy));

    char *ptr = strtok(reqCopy, "#");
    if (!ptr) {
        sprintf(reponse, "ERROR#Invalid request format");
        return true;
    }

    // 2. LOGIN
    if (strcmp(ptr, "LOGIN") == 0) {
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

        if (OBEP_Login(mysql, user, password)) {
            sprintf(reponse, "LOGIN#ok");
            ajoute(socket);
        } else {
            sprintf(reponse, "LOGIN#ko#Mauvais identifiants !");
        }
        return true;
    }

    if (estPresent(socket) < 0) {
        sprintf(reponse, "ERROR#Non authentifié");
        return true;
    }

    // 3. LOGOUT
    if (strcmp(ptr, "LOGOUT") == 0) {
        retire(socket);
        sprintf(reponse, "LOGOUT#ok");
        return false;
    }

    // 4. GET_AUTHORS
    if (strcmp(ptr, "GET_AUTHORS") == 0) {
        if (mysql_query(mysql, "SELECT id, last_name, first_name FROM authors") != 0) {
            sprintf(reponse, "GET_AUTHORS#ko#%s", mysql_error(mysql));
            return true;
        }

        MYSQL_RES *result = mysql_store_result(mysql);
        MYSQL_ROW row;

        char buffer[2048] = "GET_AUTHORS#ok";
        while ((row = mysql_fetch_row(result))) {
            char item[256];
            snprintf(item, sizeof(item), "#%s;%s;%s", row[0], row[1], row[2]);
            strcat(buffer, item);
        }
        mysql_free_result(result);
        strcpy(reponse, buffer);
        return true;
    }

    // 5. GET_SUBJECTS
    if (strcmp(ptr, "GET_SUBJECTS") == 0) {
        if (mysql_query(mysql, "SELECT id, name FROM subjects") != 0) {
            sprintf(reponse, "GET_SUBJECTS#ko#%s", mysql_error(mysql));
            return true;
        }

        MYSQL_RES *result = mysql_store_result(mysql);
        MYSQL_ROW row;

        char buffer[2048] = "GET_SUBJECTS#ok";
        while ((row = mysql_fetch_row(result))) {
            char item[256];
            snprintf(item, sizeof(item), "#%s;%s", row[0], row[1]);
            strcat(buffer, item);
        }
        mysql_free_result(result);
        strcpy(reponse, buffer);
        return true;
    }

    // 6. ADD_AUTHOR
    if (strcmp(ptr, "ADD_AUTHOR") == 0) {
        char *lastName = strtok(NULL, "#");
        char *firstName = strtok(NULL, "#");
        clean_string(lastName);
        clean_string(firstName);

        if (!lastName || !firstName) {
            sprintf(reponse, "ADD_AUTHOR#ko#Missing fields");
            return true;
        }

        char query[512];
        snprintf(query, sizeof(query), 
                 "SELECT id FROM authors WHERE last_name='%s' AND first_name='%s'", 
                 lastName, firstName);
        mysql_query(mysql, query);
        MYSQL_RES *res = mysql_store_result(mysql);
        if (res && mysql_num_rows(res) > 0) {
            mysql_free_result(res);
            sprintf(reponse, "ADD_AUTHOR#ok#-1");
            return true;
        }
        if (res) mysql_free_result(res);

        snprintf(query, sizeof(query), 
                 "INSERT INTO authors (last_name, first_name) VALUES ('%s', '%s')", 
                 lastName, firstName);
        if (mysql_query(mysql, query) == 0) {
            long newId = (long)mysql_insert_id(mysql);
            sprintf(reponse, "ADD_AUTHOR#ok#%ld", newId);
        } else {
            sprintf(reponse, "ADD_AUTHOR#ko#-1");
        }
        return true;
    }

    // 7. ADD_SUBJECT
    if (strcmp(ptr, "ADD_SUBJECT") == 0) {
        char *name = strtok(NULL, "#");
        clean_string(name);

        if (!name) {
            sprintf(reponse, "ADD_SUBJECT#ko#Missing name");
            return true;
        }

        char query[512];
        snprintf(query, sizeof(query), "SELECT id FROM subjects WHERE name='%s'", name);
        mysql_query(mysql, query);
        MYSQL_RES *res = mysql_store_result(mysql);
        if (res && mysql_num_rows(res) > 0) {
            mysql_free_result(res);
            sprintf(reponse, "ADD_SUBJECT#ok#-1");
            return true;
        }
        if (res) mysql_free_result(res);

        snprintf(query, sizeof(query), "INSERT INTO subjects (name) VALUES ('%s')", name);
        if (mysql_query(mysql, query) == 0) {
            long newId = (long)mysql_insert_id(mysql);
            sprintf(reponse, "ADD_SUBJECT#ok#%ld", newId);
        } else {
            sprintf(reponse, "ADD_SUBJECT#ko#-1");
        }
        return true;
    }

    // 8. ADD_BOOK
    if (strcmp(ptr, "ADD_BOOK") == 0) {
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

        char query[1024];
        snprintf(query, sizeof(query),
                 "INSERT INTO books (author_id, subject_id, title, isbn, page_count, stock_quantity, price, publish_year) "
                 "VALUES (%s, %s, '%s', '%s', %s, %s, %s, %s)",
                 authorId, subjectId, title, isbn, pageCount, stockQuantity, price, publishYear);

        if (mysql_query(mysql, query) == 0) {
            long newId = (long)mysql_insert_id(mysql);
            sprintf(reponse, "ADD_BOOK#ok#%ld", newId);
        } else {
            sprintf(reponse, "ADD_BOOK#ok#-1");
        }
        return true;
    }

    sprintf(reponse, "ERROR#Unknown command");
    return true;
}