/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:06:17 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 12:06:28 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	monitor_philosopher(t_params *params, int i)
{
	sem_wait(params->death_sem);
	if ((get_timestamp()
			- params->philosophers[i].last_meal_time) > params->time_to_die)
	{
		print_status(&params->philosophers[i], "died");
		sem_wait(params->stop_sem);
		params->stop = 1;
		sem_post(params->stop_sem);
		kill_all_philosophers(params, 0);
	}
	sem_post(params->death_sem);
}

int	monitor_meals_recursive(t_params *params, int i, int all_philosophers_done)
{
	if (i >= params->number_of_philosophers)
		return (all_philosophers_done);
	monitor_philosopher(params, i);
	if (params->number_of_times_each_philosopher_must_eat > 0)
	{
		if (params->philosophers[i].meals_eaten
			< params->number_of_times_each_philosopher_must_eat)
			all_philosophers_done = 0;
	}
	else
	{
		all_philosophers_done = 0;
	}
	sem_wait(params->stop_sem);
	if (params->stop)
	{
		sem_post(params->stop_sem);
		return (all_philosophers_done);
	}
	sem_post(params->stop_sem);
	return (monitor_meals_recursive(params, i + 1, all_philosophers_done));
}

int	monitor_meals(t_params *params)
{
	return (monitor_meals_recursive(params, 0, 1));
}

void	check_meals(t_params *params)
{
	int	i;

	sem_wait(params->print_sem);
	printf("All philosophers have eaten at least %d times.\n",
		params->number_of_times_each_philosopher_must_eat);
	sem_wait(params->stop_sem);
	params->stop = 1;
	sem_post(params->stop_sem);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		kill(params->philosophers[i].pid, SIGTERM);
		i++;
	}
	sem_post(params->print_sem);
}

void	monitor_routine(t_params *params)
{
	while (1)
	{
		sem_wait(params->stop_sem);
		if (params->stop)
		{
			sem_post(params->stop_sem);
			break ;
		}
		sem_post(params->stop_sem);
		if (monitor_meals(params))
			check_meals(params);
		usleep(1000);
	}
}
