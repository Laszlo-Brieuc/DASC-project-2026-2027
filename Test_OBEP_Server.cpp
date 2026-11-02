#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql/mysql.h>

#include "tcp.h"
#include "OBEP.h"
#include "configuration_loader.h"

#define MAX_QUEUE 50

// Thread Pool Task Queue
int connection_queue[MAX_QUEUE];
int queue_count = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

ServerConfig config;

void push_socket(int socket) {
    pthread_mutex_lock(&queue_mutex);
    if (queue_count < MAX_QUEUE) {
        connection_queue[queue_count++] = socket;
        pthread_cond_signal(&queue_cond);
    } else {
        printf("[WARN] Connection queue full! Dropping socket %d\n", socket);
        close(socket);
    }
    pthread_mutex_unlock(&queue_mutex);
}

int pop_socket() {
    pthread_mutex_lock(&queue_mutex);
    while (queue_count == 0) {
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    int socket = connection_queue[0];
    for (int i = 0; i < queue_count - 1; i++) {
        connection_queue[i] = connection_queue[i + 1];
    }
    queue_count--;
    pthread_mutex_unlock(&queue_mutex);
    return socket;
}

// Worker Thread Loop
void* worker_thread(void* arg) {
    // Each thread gets its own MySQL handle (MySQL connections are NOT thread-safe)
    MYSQL *mysql = mysql_init(NULL);
    if (!mysql_real_connect(mysql, config.db_host, config.db_user, config.db_pass, config.db_name, 0, NULL, 0)) {
        fprintf(stderr, "[THREAD %p] DB Connection Failed: %s\n", (void*)pthread_self(), mysql_error(mysql));
        pthread_exit(NULL);
    }

    printf("[THREAD %p] Worker thread initialized and ready.\n", (void*)pthread_self());

    while (1) {
        int sService = pop_socket();
        printf("[THREAD %p] Picked up client connection on socket %d\n", (void*)pthread_self(), sService);

        char request[2048];
        char response[2048];
        bool keepAlive = true;

        // Connection loop
        while (keepAlive) {
            int bytesReceived = Receive(sService, request);
            if (bytesReceived <= 0) {
                printf("[THREAD %p] Client disconnected (socket %d)\n", (void*)pthread_self(), sService);
                break;
            }

            printf("[THREAD %p] Request received: %s\n", (void*)pthread_self(), request);

            // Process OBEP protocol command
            keepAlive = OBEP(request, response, sService, mysql);

            // Send response back
            Send(sService, response, strlen(response));
            printf("[THREAD %p] Response sent: %s\n", (void*)pthread_self(), response);
        }

        // Clean up socket upon logout or client disconnect
        retire(sService);
        close(sService);
    }

    mysql_close(mysql);
    return NULL;
}

int main() {
    // 1. Read configuration file
    if (load_config("server.conf", &config) != 0) {
        printf("Failed to load server.conf. Exiting.\n");
        return 1;
    }

    printf("Starting Encoding Server on port %d with thread pool size %d...\n", 
           config.port, config.pool_size);

    // 2. Start thread pool

    pthread_t threads[config.pool_size];
    for (int i = 0; i < config.pool_size; i++) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
        usleep(100000); // 100ms pause to let each thread complete its DB handshake smoothly
    }

    // 3. Initialize Server Socket
    int sListen = ServerSocket(config.port);
    if (sListen == -1) {
        perror("ServerSocket failed");
        return 1;
    }

    // 4. Main accept loop
    char clientIP[50];
    while (1) {
        int sService = Accept(sListen, clientIP);
        if (sService != -1) {
            printf("[SERVER] Incoming connection from %s (socket %d)\n", clientIP, sService);
            push_socket(sService);
        }
    }

    close(sListen);
    return 0;
}