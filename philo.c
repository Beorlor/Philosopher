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
    volatile int stop;
    pthread_mutex_t *forks;
    pthread_mutex_t print_mutex;
    pthread_mutex_t death_mutex;
    pthread_mutex_t meals_mutex;
    t_philosopher *philosophers;
} t_params;

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

void print_status(t_philosopher *philo, const char *status) {
    long timestamp = get_timestamp();
    pthread_mutex_lock(&philo->params->print_mutex);
    if (!philo->params->stop)
        printf("%ld %d %s\n", timestamp, philo->id, status);
    pthread_mutex_unlock(&philo->params->print_mutex);
}

void *philosopher_routine(void *arg) {
    t_philosopher *philo = (t_philosopher *)arg;

    while (!philo->params->stop) {
        print_status(philo, "is thinking");

        pthread_mutex_t *first_fork = (philo->id % 2 == 0) ? philo->left_fork : philo->right_fork;
        pthread_mutex_t *second_fork = (philo->id % 2 == 0) ? philo->right_fork : philo->left_fork;

        pthread_mutex_lock(first_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(second_fork);
        print_status(philo, "has taken a second fork");

        pthread_mutex_lock(&philo->params->death_mutex);
        philo->last_meal_time = get_timestamp();
        print_status(philo, "is eating");
        pthread_mutex_unlock(&philo->params->death_mutex);

        ft_usleep(philo->params->time_to_eat);

        pthread_mutex_lock(&philo->params->meals_mutex);
        philo->meals_eaten++;
        pthread_mutex_unlock(&philo->params->meals_mutex);

        pthread_mutex_unlock(second_fork);
        pthread_mutex_unlock(first_fork);

        print_status(philo, "is sleeping");
        ft_usleep(philo->params->time_to_sleep);
    }
    return NULL;
}

int parse_arguments(int argc, char **argv, t_params *params) {
    if (argc < 5 || argc > 6) {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
        return 0;
    }
    params->number_of_philosophers = atoi(argv[1]);
    params->time_to_die = atoi(argv[2]);
    params->time_to_eat = atoi(argv[3]);
    params->time_to_sleep = atoi(argv[4]);
    params->number_of_times_each_philosopher_must_eat = (argc == 6) ? atoi(argv[5]) : -1;
    params->stop = 0;

    return 1;
}

void initialize_philosophers(t_params *params) {
    params->forks = malloc(params->number_of_philosophers * sizeof(pthread_mutex_t));
    for (int i = 0; i < params->number_of_philosophers; i++) {
        pthread_mutex_init(&params->forks[i], NULL);
    }
    pthread_mutex_init(&params->print_mutex, NULL);
    pthread_mutex_init(&params->death_mutex, NULL);
    pthread_mutex_init(&params->meals_mutex, NULL);

    params->philosophers = malloc(params->number_of_philosophers * sizeof(t_philosopher));
    for (int i = 0; i < params->number_of_philosophers; i++) {
        params->philosophers[i].id = i + 1;
        params->philosophers[i].left_fork = &params->forks[i];
        params->philosophers[i].right_fork = &params->forks[(i + 1) % params->number_of_philosophers];
        params->philosophers[i].last_meal_time = get_timestamp();
        params->philosophers[i].meals_eaten = 0;
        params->philosophers[i].params = params;
        pthread_create(&params->philosophers[i].thread, NULL, philosopher_routine, &params->philosophers[i]);
    }
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

void *monitor_routine(void *arg) {
    t_params *params = (t_params *)arg;
    while (!params->stop) {
        int all_philosophers_done = 1;
        for (int i = 0; i < params->number_of_philosophers; i++) {
            pthread_mutex_lock(&params->death_mutex);
            if ((get_timestamp() - params->philosophers[i].last_meal_time) > params->time_to_die) {
                print_status(&params->philosophers[i], "died");
                params->stop = 1;
            }
            pthread_mutex_unlock(&params->death_mutex);

            if (params->number_of_times_each_philosopher_must_eat > 0) {
                pthread_mutex_lock(&params->meals_mutex);
                if (params->philosophers[i].meals_eaten < params->number_of_times_each_philosopher_must_eat) {
                    all_philosophers_done = 0;
                }
                pthread_mutex_unlock(&params->meals_mutex);
            } else {
                all_philosophers_done = 0;
            }

            if (params->stop)
                break;
        }
        if (params->number_of_times_each_philosopher_must_eat > 0 && all_philosophers_done) {
            pthread_mutex_lock(&params->print_mutex);
            printf("All philosophers have eaten at least %d times.\n", params->number_of_times_each_philosopher_must_eat);
            params->stop = 1;
            pthread_mutex_unlock(&params->print_mutex);
        }
        usleep(1000);  // Check roughly every 1ms
    }
    return NULL;
}

int main(int argc, char **argv) {
    t_params params;

    if (!parse_arguments(argc, argv, &params))
        return (1);

    initialize_philosophers(&params);

    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_routine, &params);

    pthread_join(monitor_thread, NULL);

    for (int i = 0; i < params.number_of_philosophers; i++) {
        pthread_join(params.philosophers[i].thread, NULL);
    }

    cleanup(&params);
    return (0);
}
