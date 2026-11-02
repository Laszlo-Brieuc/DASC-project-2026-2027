#include "configuration_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to remove trailing newlines and spaces
static void trim_newline(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\r' || str[len - 1] == '\n' || str[len - 1] == ' ')) {
        str[len - 1] = '\0';
        len--;
    }
}

int load_config(const char *filename, ServerConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening config file");
        return -1;
    }

    // Default fallbacks
    config->port = 5000;
    config->pool_size = 5;
    strcpy(config->db_host, "127.0.0.1"); // Use 127.0.0.1 to force TCP connection
    strcpy(config->db_user, "Student");
    strcpy(config->db_pass, "PassStudent1_");
    strcpy(config->db_name, "PourStudent");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        char key[64], value[128];
        if (sscanf(line, "%63[^=]=%127[^\r\n]", key, value) == 2) {
            trim_newline(value);
            if (strcmp(key, "PORT_ENCODING") == 0) config->port = atoi(value);
            else if (strcmp(key, "POOL_SIZE") == 0) config->pool_size = atoi(value);
            else if (strcmp(key, "DB_HOST") == 0) strcpy(config->db_host, value);
            else if (strcmp(key, "DB_USER") == 0) strcpy(config->db_user, value);
            else if (strcmp(key, "DB_PASS") == 0) strcpy(config->db_pass, value);
            else if (strcmp(key, "DB_NAME") == 0) strcpy(config->db_name, value);
        }
    }

    fclose(file);
    return 0;
}