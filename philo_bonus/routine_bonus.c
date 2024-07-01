/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:02:49 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 15:21:28 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	take_forks(t_philosopher *philo)
{
	if (philo->params->number_of_philosophers == 1)
	{
		sem_wait(philo->params->forks);
		print_status(philo, "has taken a fork");
		ft_usleep(philo->params->time_to_die);
		sem_wait(philo->params->stop_sem);
		philo->params->stop = 1;
		sem_post(philo->params->stop_sem);
		sem_post(philo->params->forks);
		return ;
	}
	sem_wait(philo->params->pair_of_forks_sem);
	sem_wait(philo->params->forks);
	print_status(philo, "has taken a fork");
	sem_wait(philo->params->forks);
	print_status(philo, "has taken a second fork");
}

void	eat(t_philosopher *philo)
{
	sem_wait(philo->params->death_sem);
	philo->last_meal_time = get_timestamp();
	print_status(philo, "is eating");
	sem_post(philo->params->death_sem);
	ft_usleep(philo->params->time_to_eat);
	philo->meals_eaten++;
	sem_post(philo->params->forks);
	sem_post(philo->params->forks);
	sem_post(philo->params->pair_of_forks_sem);
}

void	philosopher_routine(t_philosopher *philo)
{
	while (1)
	{
		sem_wait(philo->params->stop_sem);
		if (philo->params->stop)
		{
			sem_post(philo->params->stop_sem);
			break ;
		}
		sem_post(philo->params->stop_sem);
		print_status(philo, "is thinking");
		take_forks(philo);
		if (philo->params->number_of_philosophers == 1)
			break ;
		eat(philo);
		print_status(philo, "is sleeping");
		ft_usleep(philo->params->time_to_sleep);
	}
	sem_close(philo->params->forks);
	sem_close(philo->params->print_sem);
	sem_close(philo->params->death_sem);
	sem_close(philo->params->pair_of_forks_sem);
	sem_close(philo->params->stop_sem);
	exit(0);
}

int	init_philosopher(t_params *params, int i)
{
	pid_t	pid;

	params->philosophers[i].id = i + 1;
	params->philosophers[i].last_meal_time = get_timestamp();
	params->philosophers[i].meals_eaten = 0;
	params->philosophers[i].params = params;
	pid = fork();
	if (pid == 0)
	{
		philosopher_routine(&params->philosophers[i]);
		exit(0);
	}
	else if (pid < 0)
	{
		perror("Failed to fork philosopher process");
		return (0);
	}
	params->philosophers[i].pid = pid;
	return (1);
}
