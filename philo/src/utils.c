#include "philo.h"

long get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void ft_usleep(int milliseconds) {
    long start = get_timestamp();
    while ((get_timestamp() - start) < milliseconds) {
        usleep(10);
    }
}

void print_status(t_philosopher *philo, const char *status, const char *color) {
    long timestamp = get_timestamp();
    pthread_mutex_lock(&philo->params->print_mutex);
    if (!philo->params->stop) {
        printf("%s%ld %d %s%s\n", color, timestamp, philo->id, status, COLOR_RESET);
    }
    pthread_mutex_unlock(&philo->params->print_mutex);
}

void cleanup(t_params *params) {
    for (int i = 0; i < params->number_of_philosophers; i++) {
        pthread_mutex_destroy(&params->forks[i]);
    }
    pthread_mutex_destroy(&params->print_mutex);
    pthread_mutex_destroy(&params->death_mutex);
    pthread_mutex_destroy(&params->meals_mutex);
    free(params->forks);
    free(params->philosophers);
}
