#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

typedef struct s_params {
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    sem_t *forks;
    sem_t *print_sem;
    sem_t *death_sem;
    sem_t *philosopher_sem;
    int stop;
    struct s_philosopher *philosophers;
} t_params;

typedef struct s_philosopher {
    int id;
    long last_meal_time;
    int meals_eaten;
    t_params *params;
} t_philosopher;

long get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void ft_usleep(int milliseconds) {
    long start = get_timestamp();
    while ((get_timestamp() - start) < milliseconds) {
        usleep(100);
    }
}

void print_status(t_philosopher *philo, const char *status) {
    long timestamp = get_timestamp();
    sem_wait(philo->params->print_sem);
    if (!philo->params->stop)
        printf("%ld %d %s\n", timestamp, philo->id, status);
    sem_post(philo->params->print_sem);
}

void philosopher_routine(t_philosopher *philo) {
    while (!philo->params->stop) {
        print_status(philo, "is thinking");

        // Ensure only number_of_philosophers - 1 philosophers can attempt to pick up forks at once
        sem_wait(philo->params->philosopher_sem);

        // Pick up forks
        sem_wait(philo->params->forks);
        print_status(philo, "has taken a fork");
        sem_wait(philo->params->forks);
        print_status(philo, "has taken a second fork");

        // Eating
        sem_wait(philo->params->death_sem);
        philo->last_meal_time = get_timestamp();
        print_status(philo, "is eating");
        sem_post(philo->params->death_sem);

        ft_usleep(philo->params->time_to_eat);

        philo->meals_eaten++;

        // Put down forks
        sem_post(philo->params->forks);
        sem_post(philo->params->forks);

        // Allow another philosopher to try to pick up forks
        sem_post(philo->params->philosopher_sem);

        // Sleeping
        print_status(philo, "is sleeping");
        ft_usleep(philo->params->time_to_sleep);
    }
    // Close semaphores in the philosopher process
    sem_close(philo->params->forks);
    sem_close(philo->params->print_sem);
    sem_close(philo->params->death_sem);
    sem_close(philo->params->philosopher_sem);
}

void initialize_semaphores(t_params *params) {
    sem_unlink("/forks");
    sem_unlink("/print_sem");
    sem_unlink("/death_sem");
    sem_unlink("/philosopher_sem");

    params->forks = sem_open("/forks", O_CREAT, 0644, params->number_of_philosophers);
    params->print_sem = sem_open("/print_sem", O_CREAT, 0644, 1);
    params->death_sem = sem_open("/death_sem", O_CREAT, 0644, 1);
    params->philosopher_sem = sem_open("/philosopher_sem", O_CREAT, 0644, params->number_of_philosophers - 1);
}

void cleanup_semaphores(t_params *params) {
    sem_close(params->forks);
    sem_close(params->print_sem);
    sem_close(params->death_sem);
    sem_close(params->philosopher_sem);
    sem_unlink("/forks");
    sem_unlink("/print_sem");
    sem_unlink("/death_sem");
    sem_unlink("/philosopher_sem");
}

void create_philosophers(t_params *params) {
    params->philosophers = malloc(params->number_of_philosophers * sizeof(t_philosopher));
    for (int i = 0; i < params->number_of_philosophers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            t_philosopher *philo = &params->philosophers[i];
            philo->id = i + 1;
            philo->last_meal_time = get_timestamp();
            philo->meals_eaten = 0;
            philo->params = params;
            philosopher_routine(philo);
            exit(0);
        }
    }
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

void monitor_routine(t_params *params) {
    while (!params->stop) {
        for (int i = 0; i < params->number_of_philosophers; i++) {
            sem_wait(params->death_sem);
            long time_since_last_meal = get_timestamp() - params->philosophers[i].last_meal_time;
            if (time_since_last_meal > params->time_to_die) {
                print_status(&params->philosophers[i], "died");
                params->stop = 1;
                kill(0, SIGKILL); // Kill all philosopher processes
            }
            sem_post(params->death_sem);

            if (params->stop)
                break;
        }
        usleep(1000);
    }
}

int main(int argc, char **argv) {
    t_params params;

    if (!parse_arguments(argc, argv, &params))
        return (1);

    initialize_semaphores(&params);
    create_philosophers(&params);

    monitor_routine(&params);

    for (int i = 0; i < params.number_of_philosophers; i++) {
        waitpid(-1, NULL, 0);
    }

    cleanup_semaphores(&params);
    free(params.philosophers);
    return (0);
}