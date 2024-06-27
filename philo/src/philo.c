#include "philo.h"

void *philosopher_routine(void *arg) {
    t_philosopher *philo = (t_philosopher *)arg;

    while (!philo->params->stop) {
        print_status(philo, "is thinking", COLOR_THINKING);

        pthread_mutex_t *first_fork = (philo->id % 2 == 0) ? philo->left_fork : philo->right_fork;
        pthread_mutex_t *second_fork = (philo->id % 2 == 0) ? philo->right_fork : philo->left_fork;

        pthread_mutex_lock(first_fork);
        print_status(philo, "has taken a fork", COLOR_FORK);

        // Handle the case where there is only one philosopher
        if (first_fork == second_fork) {
            ft_usleep(philo->params->time_to_die);
            print_status(philo, "died", COLOR_DEAD);
            philo->params->stop = 1;
            pthread_mutex_unlock(first_fork);
            break;
        }

        pthread_mutex_lock(second_fork);
        print_status(philo, "has taken a second fork", COLOR_FORK);

        pthread_mutex_lock(&philo->params->death_mutex);
        philo->last_meal_time = get_timestamp();
        print_status(philo, "is eating", COLOR_EATING);
        pthread_mutex_unlock(&philo->params->death_mutex);

        ft_usleep(philo->params->time_to_eat);

        pthread_mutex_lock(&philo->params->meals_mutex);
        philo->meals_eaten++;
        pthread_mutex_unlock(&philo->params->meals_mutex);

        pthread_mutex_unlock(second_fork);
        pthread_mutex_unlock(first_fork);

        print_status(philo, "is sleeping", COLOR_SLEEPING);
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
    params->forks = check_malloc(params->number_of_philosophers * sizeof(pthread_mutex_t));
    for (int i = 0; i < params->number_of_philosophers; i++) {
        check_pthread_mutex_init(pthread_mutex_init(&params->forks[i], NULL));
    }
    check_pthread_mutex_init(pthread_mutex_init(&params->print_mutex, NULL));
    check_pthread_mutex_init(pthread_mutex_init(&params->death_mutex, NULL));
    check_pthread_mutex_init(pthread_mutex_init(&params->meals_mutex, NULL));

    params->philosophers = check_malloc(params->number_of_philosophers * sizeof(t_philosopher));
    for (int i = 0; i < params->number_of_philosophers; i++) {
        params->philosophers[i].id = i + 1;
        params->philosophers[i].left_fork = &params->forks[i];
        params->philosophers[i].right_fork = &params->forks[(i + 1) % params->number_of_philosophers];
        params->philosophers[i].last_meal_time = get_timestamp();
        params->philosophers[i].meals_eaten = 0;
        params->philosophers[i].params = params;
        check_pthread_create(pthread_create(&params->philosophers[i].thread, NULL, philosopher_routine, &params->philosophers[i]));
    }
}

void *monitor_routine(void *arg) {
    t_params *params = (t_params *)arg;
    while (!params->stop) {
        int all_philosophers_done = 1;
        for (int i = 0; i < params->number_of_philosophers; i++) {
            pthread_mutex_lock(&params->death_mutex);
            if ((get_timestamp() - params->philosophers[i].last_meal_time) > params->time_to_die) {
                print_status(&params->philosophers[i], "died", COLOR_DEAD);
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

    if (!parse_arguments(argc, argv, &params)) {
        return EXIT_FAILURE;
    }

    initialize_philosophers(&params);

    pthread_t monitor_thread;
    check_pthread_create(pthread_create(&monitor_thread, NULL, monitor_routine, &params));

    pthread_join(monitor_thread, NULL);

    for (int i = 0; i < params.number_of_philosophers; i++) {
        pthread_join(params.philosophers[i].thread, NULL);
    }

    cleanup(&params);
    return EXIT_SUCCESS;
}
