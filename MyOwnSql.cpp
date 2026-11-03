#include "MyOwnSql.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool db_login(MYSQL *mysql, const char* user, const char* password) {
    char query[256];
    snprintf(query, sizeof(query), 
             "SELECT id FROM employees WHERE login='%s' AND password='%s'", 
             user, password);

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

bool db_get_authors(MYSQL *mysql, char *buffer, size_t max_len) {
    if (mysql_query(mysql, "SELECT id, last_name, first_name FROM authors") != 0) {
        return false;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result) return false;

    MYSQL_ROW row;
    strcpy(buffer, "GET_AUTHORS#ok");
    while ((row = mysql_fetch_row(result))) {
        char item[256];
        snprintf(item, sizeof(item), "#%s;%s;%s", row[0], row[1], row[2]);
        if (strlen(buffer) + strlen(item) < max_len) {
            strcat(buffer, item);
        }
    }
    mysql_free_result(result);
    return true;
}

bool db_get_subjects(MYSQL *mysql, char *buffer, size_t max_len) {
    if (mysql_query(mysql, "SELECT id, name FROM subjects") != 0) {
        return false;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result) return false;

    MYSQL_ROW row;
    strcpy(buffer, "GET_SUBJECTS#ok");
    while ((row = mysql_fetch_row(result))) {
        char item[256];
        snprintf(item, sizeof(item), "#%s;%s", row[0], row[1]);
        if (strlen(buffer) + strlen(item) < max_len) {
            strcat(buffer, item);
        }
    }
    mysql_free_result(result);
    return true;
}

long db_add_author(MYSQL *mysql, const char *lastName, const char *firstName) {
    char query[512];
    snprintf(query, sizeof(query), 
             "SELECT id FROM authors WHERE last_name='%s' AND first_name='%s'", 
             lastName, firstName);
    mysql_query(mysql, query);
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res && mysql_num_rows(res) > 0) {
        mysql_free_result(res);
        return -1; // Already exists
    }
    if (res) mysql_free_result(res);

    snprintf(query, sizeof(query), 
             "INSERT INTO authors (last_name, first_name) VALUES ('%s', '%s')", 
             lastName, firstName);
    if (mysql_query(mysql, query) == 0) {
        return (long)mysql_insert_id(mysql);
    }
    return -1;
}

long db_add_subject(MYSQL *mysql, const char *name) {
    char query[512];
    snprintf(query, sizeof(query), "SELECT id FROM subjects WHERE name='%s'", name);
    mysql_query(mysql, query);
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res && mysql_num_rows(res) > 0) {
        mysql_free_result(res);
        return -1; // Already exists
    }
    if (res) mysql_free_result(res);

    snprintf(query, sizeof(query), "INSERT INTO subjects (name) VALUES ('%s')", name);
    if (mysql_query(mysql, query) == 0) {
        return (long)mysql_insert_id(mysql);
    }
    return -1;
}

long db_add_book(MYSQL *mysql, const char *authorId, const char *subjectId, const char *title, 
                 const char *isbn, const char *pageCount, const char *stockQuantity, 
                 const char *price, const char *publishYear) {
    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO books (author_id, subject_id, title, isbn, page_count, stock_quantity, price, publish_year) "
             "VALUES (%s, %s, '%s', '%s', %s, %s, %s, %s)",
             authorId, subjectId, title, isbn, pageCount, stockQuantity, price, publishYear);

    if (mysql_query(mysql, query) == 0) {
        return (long)mysql_insert_id(mysql);
    }
    return -1;
}