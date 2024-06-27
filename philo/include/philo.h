#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define COLOR_RESET    "\x1b[0m"
#define COLOR_THINKING "\x1b[34m"
#define COLOR_FORK     "\x1b[33m"
#define COLOR_EATING   "\x1b[32m"
#define COLOR_SLEEPING "\x1b[36m"
#define COLOR_DEAD     "\x1b[31m"

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
    volatile int stop;
    pthread_mutex_t *forks;
    pthread_mutex_t print_mutex;
    pthread_mutex_t death_mutex;
    pthread_mutex_t meals_mutex;
    t_philosopher *philosophers;
} t_params;

#endif
