/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:06:17 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 15:15:00 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	monitor_philosopher(t_params *params, int i)
{
	long	time_diff;

	sem_wait(params->death_sem);
	time_diff = get_timestamp() - params->philosophers[i].last_meal_time;
	if (time_diff > params->time_to_die)
	{
		print_status(&params->philosophers[i], "died");
		sem_wait(params->stop_sem);
		params->stop = 1;
		sem_post(params->stop_sem);
		i = 0;
		while (i < params->number_of_philosophers)
		{
			kill(params->philosophers[i].pid, SIGTERM);
			i++;
		}
	}
	sem_post(params->death_sem);
}

int	enough_food(t_params *params, int i)
{
	return (params->philosophers[i].meals_eaten
		< params->number_of_times_each_philosopher_must_eat);
}

int	monitor_meals(t_params *params)
{
	int	i;
	int	all_philosophers_done;

	i = 0;
	all_philosophers_done = 1;
	while (i < params->number_of_philosophers)
	{
		monitor_philosopher(params, i);
		if (params->number_of_times_each_philosopher_must_eat > 0)
		{
			if (enough_food(params, i))
				all_philosophers_done = 0;
		}
		else
			all_philosophers_done = 0;
		sem_wait(params->stop_sem);
		if (params->stop)
		{
			sem_post(params->stop_sem);
			break ;
		}
		sem_post(params->stop_sem);
		i++;
	}
	return (all_philosophers_done);
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
	int	should_stop;

	should_stop = 0;
	while (!should_stop)
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
		usleep(100);
	}
}
