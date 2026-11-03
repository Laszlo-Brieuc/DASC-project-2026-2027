#ifndef MY_OWN_SQL_H
#define MY_OWN_SQL_H

#include <stdbool.h>
#include <mysql/mysql.h> // Keep MySQL internal to the database layer

// Connection helpers
MYSQL* db_connect(const char *host, const char *user, const char *pass, const char *name);
void db_close(MYSQL *mysql);

// Business logic database functions
bool db_login(MYSQL *mysql, const char* user, const char* password);
bool db_get_authors(MYSQL *mysql, char *buffer, size_t max_len);
bool db_get_subjects(MYSQL *mysql, char *buffer, size_t max_len);
long db_add_author(MYSQL *mysql, const char *lastName, const char *firstName);
long db_add_subject(MYSQL *mysql, const char *name);
long db_add_book(MYSQL *mysql, const char *authorId, const char *subjectId, const char *title, const char *isbn, const char *pageCount, const char *stockQuantity,const char *price, const char* publishYear);

#endif