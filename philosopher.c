#include "philo.h"

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    while (!philo->params->stop)
    {
        // Penser
        print_status(philo, "is thinking");

        // Prendre les fourchettes
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");

        // Manger
        pthread_mutex_lock(&philo->params->philosopher_mutexes[philo->id - 1]);
        print_status(philo, "is eating");
        philo->last_meal_time = get_timestamp();
        usleep(philo->params->time_to_eat * 1000);
        philo->meals_eaten++;
        pthread_mutex_unlock(&philo->params->philosopher_mutexes[philo->id - 1]);

        // Reposer les fourchettes
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        // Dormir
        print_status(philo, "is sleeping");
        usleep(philo->params->time_to_sleep * 1000);
    }
    return (NULL);
}
