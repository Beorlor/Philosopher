/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:19:25 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:33:42 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_mutexes(t_params *params)
{
	if (!check_pthread_mutex_init(pthread_mutex_init(&params->print_mutex,
				NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->death_mutex,
				NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->meals_mutex,
				NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->stop_mutex,
				NULL)))
		return (0);
	return (1);
}

int	init_philosopher_forks(t_params *params, int i)
{
	if (params->philosophers[i].id % 2 == 0)
	{
		params->philosophers[i].left_fork = &params->forks[i];
		params->philosophers[i].right_fork = &params->forks[(i + 1)
			% params->number_of_philosophers];
	}
	else
	{
		params->philosophers[i].left_fork = &params->forks[(i + 1)
			% params->number_of_philosophers];
		params->philosophers[i].right_fork = &params->forks[i];
	}
	params->philosophers[i].last_meal_time = get_timestamp();
	params->philosophers[i].meals_eaten = 0;
	params->philosophers[i].params = params;
	return (check_pthread_create(pthread_create(&params->philosophers[i].thread,
				NULL, philosopher_routine, &params->philosophers[i])));
}

int	init_forks(t_params *params)
{
	int	i;

	params->forks = check_malloc(params->number_of_philosophers
			* sizeof(pthread_mutex_t));
	if (!params->forks)
		return (0);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		if (!check_pthread_mutex_init(pthread_mutex_init(&params->forks[i],
					NULL)))
			return (0);
		i++;
	}
	return (1);
}

int	init_philosophers(t_params *params)
{
	int	i;

	params->philosophers = check_malloc(params->number_of_philosophers
			* sizeof(t_philosopher));
	if (!params->philosophers)
		return (0);
	init_colors(params->philosophers, params->number_of_philosophers);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		params->philosophers[i].id = i + 1;
		if (!init_philosopher_forks(params, i))
			return (0);
		i++;
	}
	return (1);
}

int	initialize_philosophers(t_params *params)
{
	if (!init_forks(params) || !init_mutexes(params))
		return (0);
	if (!init_philosophers(params))
		return (0);
	return (1);
}
