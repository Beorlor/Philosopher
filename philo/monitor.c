/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:31:52 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	monitor_philosopher(t_params *params, int i)
{
	pthread_mutex_lock(&params->death_mutex);
	if ((get_timestamp()
			- params->philosophers[i].last_meal_time) > params->time_to_die)
	{
		print_status(&params->philosophers[i], "died");
		pthread_mutex_lock(&params->stop_mutex);
		params->stop = 1;
		pthread_mutex_unlock(&params->stop_mutex);
	}
	pthread_mutex_unlock(&params->death_mutex);
}

int	monitor_meals(t_params *params)
{
	int	i;
	int	all_philosophers_done;

	all_philosophers_done = 1;
	i = 0;
	while (i < params->number_of_philosophers)
	{
		monitor_philosopher(params, i);
		if (params->number_of_times_each_philosopher_must_eat > 0)
		{
			pthread_mutex_lock(&params->meals_mutex);
			if (params->philosophers[i].meals_eaten
				< params->number_of_times_each_philosopher_must_eat)
				all_philosophers_done = 0;
			pthread_mutex_unlock(&params->meals_mutex);
		}
		else
		{
			all_philosophers_done = 0;
		}
		if (params->stop)
			break ;
		i++;
	}
	return (all_philosophers_done);
}

void	check_meals(t_params *params)
{
	pthread_mutex_lock(&params->print_mutex);
	printf("All philosophers have eaten at least %d times.\n",
		params->number_of_times_each_philosopher_must_eat);
	pthread_mutex_lock(&params->stop_mutex);
	params->stop = 1;
	pthread_mutex_unlock(&params->stop_mutex);
	pthread_mutex_unlock(&params->print_mutex);
}

void	*monitor_routine(void *arg)
{
	t_params	*params;
	int			should_stop;

	params = (t_params *)arg;
	should_stop = params->stop;
	while (!should_stop)
	{
		if (monitor_meals(params))
		{
			check_meals(params);
		}
		pthread_mutex_lock(&params->stop_mutex);
		should_stop = params->stop;
		pthread_mutex_unlock(&params->stop_mutex);
		usleep(100);
	}
	return (NULL);
}
