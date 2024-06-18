#include "philo.h"

void *monitor_routine(void *arg)
{
    t_params *params = (t_params *)arg;
    long current_time;
    int all_ate_enough;

    while (!params->stop)
    {
        all_ate_enough = 1;
        for (int i = 0; i < params->number_of_philosophers; i++)
        {
            pthread_mutex_lock(&params->death_mutex);
            current_time = get_timestamp();
            if (current_time - params->philosophers[i].last_meal_time > params->time_to_die)
            {
                print_status(&params->philosophers[i], "died");
                params->stop = 1;
            }
            if (params->number_of_times_each_philosopher_must_eat != -1 &&
                params->philosophers[i].meals_eaten < params->number_of_times_each_philosopher_must_eat)
            {
                all_ate_enough = 0;
            }
            pthread_mutex_unlock(&params->death_mutex);
            if (params->stop)
                return (NULL);
        }
        if (all_ate_enough)
        {
            params->stop = 1;
            return (NULL);
        }
        usleep(100); // Pour éviter de trop fréquentes vérifications
    }
    return (NULL);
}

int main(int argc, char **argv)
{
    t_params params;

    if (!parse_arguments(argc, argv, &params))
        return (1);

    initialize_philosophers(&params);

    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_routine, &params);

    for (int i = 0; i < params.number_of_philosophers; i++)
        pthread_join(params.philosophers[i].thread, NULL);
    pthread_join(monitor_thread, NULL);

    cleanup(&params);
    return (0);
}

void initialize_philosophers(t_params *params)
{
    params->philosophers = malloc(params->number_of_philosophers * sizeof(t_philosopher));
    params->forks = malloc(params->number_of_philosophers * sizeof(pthread_mutex_t));
    params->philosopher_mutexes = malloc(params->number_of_philosophers * sizeof(pthread_mutex_t));

    for (int i = 0; i < params->number_of_philosophers; i++) {
        pthread_mutex_init(&params->forks[i], NULL);
        pthread_mutex_init(&params->philosopher_mutexes[i], NULL);
    }

    pthread_mutex_init(&params->print_mutex, NULL);
    pthread_mutex_init(&params->death_mutex, NULL);

    for (int i = 0; i < params->number_of_philosophers; i++)
    {
        params->philosophers[i].id = i + 1;
        params->philosophers[i].left_fork = &params->forks[i];
        params->philosophers[i].right_fork = &params->forks[(i + 1) % params->number_of_philosophers];
        params->philosophers[i].last_meal_time = get_timestamp();
        params->philosophers[i].meals_eaten = 0;
        params->philosophers[i].params = params;
        pthread_create(&params->philosophers[i].thread, NULL, philosopher_routine, &params->philosophers[i]);
    }
}

void cleanup(t_params *params)
{
    for (int i = 0; i < params->number_of_philosophers; i++) {
        pthread_mutex_destroy(&params->forks[i]);
        pthread_mutex_destroy(&params->philosopher_mutexes[i]);
    }
    pthread_mutex_destroy(&params->print_mutex);
    pthread_mutex_destroy(&params->death_mutex);
    free(params->forks);
    free(params->philosophers);
    free(params->philosopher_mutexes);
}

int parse_arguments(int argc, char **argv, t_params *params)
{
    if (argc < 5 || argc > 6)
    {
        printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
        return (0);
    }

    params->number_of_philosophers = atoi(argv[1]);
    params->time_to_die = atoi(argv[2]);
    params->time_to_eat = atoi(argv[3]);
    params->time_to_sleep = atoi(argv[4]);
    params->number_of_times_each_philosopher_must_eat = (argc == 6) ? atoi(argv[5]) : -1;
    params->stop = 0;

    if (params->number_of_philosophers < 1 || params->time_to_die <= 0 || params->time_to_eat <= 0 || params->time_to_sleep <= 0)
    {
        printf("Error: Invalid arguments\n");
        return (0);
    }

    return (1);
}

long get_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void print_status(t_philosopher *philo, const char *status)
{
    long timestamp;

    timestamp = get_timestamp();
    pthread_mutex_lock(&philo->params->print_mutex);
    if (!philo->params->stop)
        printf("%ld %d %s\n", timestamp, philo->id, status);
    pthread_mutex_unlock(&philo->params->print_mutex);
}
