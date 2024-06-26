#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_philosopher {
    int id;
    pthread_t thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    long last_meal_time;
    int meals_eaten;
    struct s_params *params;
} t_philosopher;

typedef struct s_params {
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    int stop;
    pthread_mutex_t *forks;
    pthread_mutex_t print_mutex;
    pthread_mutex_t death_mutex;
    t_philosopher *philosophers;
} t_params;

long get_timestamp();
void print_status(t_philosopher *philo, const char *status);
void *philosopher_routine(void *arg);
void initialize_philosophers(t_params *params);
void cleanup(t_params *params);
int parse_arguments(int argc, char **argv, t_params *params);
void *monitor_routine(void *arg);
void ft_usleep(int milliseconds);

#endif
