#ifndef CONFIGURATION_LOADER_H
#define CONFIGURATION_LOADER_H

typedef struct {
    int port;
    int pool_size;
    char db_host[64];
    char db_user[64];
    char db_pass[64];
    char db_name[64];
} ServerConfig;

// Loads key-value pairs from filename into config struct.
// Returns 0 on success, -1 on failure.
int load_config(const char *filename, ServerConfig *config);

#endif