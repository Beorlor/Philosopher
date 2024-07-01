/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:29:46 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philosopher *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	if (philo->left_fork == philo->right_fork)
	{
		ft_usleep(philo->params->time_to_die);
		print_status(philo, "died");
		pthread_mutex_lock(&philo->params->stop_mutex);
		philo->params->stop = 1;
		pthread_mutex_unlock(&philo->params->stop_mutex);
		pthread_mutex_unlock(philo->left_fork);
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a second fork");
	}
}

void	eat(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->params->death_mutex);
	philo->last_meal_time = get_timestamp();
	print_status(philo, "is eating");
	pthread_mutex_unlock(&philo->params->death_mutex);
	ft_usleep(philo->params->time_to_eat);
	pthread_mutex_lock(&philo->params->meals_mutex);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->params->meals_mutex);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	int				should_stop;

	philo = (t_philosopher *)arg;
	should_stop = philo->params->stop;
	while (!should_stop)
	{
		print_status(philo, "is thinking");
		take_forks(philo);
		if (philo->left_fork == philo->right_fork)
			break ;
		eat(philo);
		print_status(philo, "is sleeping");
		ft_usleep(philo->params->time_to_sleep);
		pthread_mutex_lock(&philo->params->stop_mutex);
		should_stop = philo->params->stop;
		pthread_mutex_unlock(&philo->params->stop_mutex);
	}
	return (NULL);
}
