#include "philo.h"

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    if (philo->params->number_of_philosophers == 1)
    {
        print_status(philo, "has taken a fork");
        usleep(philo->params->time_to_die * 1000);
        print_status(philo, "died");
        philo->params->stop = 1;
        return (NULL);
    }

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
        pthread_mutex_lock(&philo->params->death_mutex);
        print_status(philo, "is eating");
        philo->last_meal_time = get_timestamp();
        pthread_mutex_unlock(&philo->params->death_mutex);

        usleep(philo->params->time_to_eat * 1000);
        philo->meals_eaten++;

        // Reposer les fourchettes
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        // Dormir
        print_status(philo, "is sleeping");
        usleep(philo->params->time_to_sleep * 1000);
    }
    return (NULL);
}
