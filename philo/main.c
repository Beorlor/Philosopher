/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:32:07 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	cleanup(t_params *params)
{
	int	i;

	i = 0;
	while (i < params->number_of_philosophers)
	{
		pthread_join(params->philosophers[i].thread, NULL);
		pthread_mutex_destroy(&params->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&params->print_mutex);
	pthread_mutex_destroy(&params->death_mutex);
	pthread_mutex_destroy(&params->meals_mutex);
	pthread_mutex_destroy(&params->stop_mutex);
	free(params->forks);
	free(params->philosophers);
}

int	main(int argc, char **argv)
{
	t_params	params;
	pthread_t	monitor_thread;

	if (!parse_arguments(argc, argv, &params))
		return (1);
	if (!initialize_philosophers(&params))
	{
		cleanup(&params);
		return (1);
	}
	if (!check_pthread_create(pthread_create(&monitor_thread, NULL,
				monitor_routine, &params)))
	{
		cleanup(&params);
		return (1);
	}
	pthread_join(monitor_thread, NULL);
	cleanup(&params);
	return (0);
}
