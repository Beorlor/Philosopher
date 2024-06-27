#include "philo.h"

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void *check_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        handle_error("malloc failed");
    }
    return ptr;
}

void check_pthread_create(int ret) {
    if (ret != 0) {
        handle_error("pthread_create failed");
    }
}

void check_pthread_mutex_init(int ret) {
    if (ret != 0) {
        handle_error("pthread_mutex_init failed");
    }
}
