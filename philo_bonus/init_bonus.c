/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:12:55 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 12:17:18 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

int	initialize_semaphores(t_params *params)
{
	sem_unlink("/forks");
	sem_unlink("/print_sem");
	sem_unlink("/death_sem");
	sem_unlink("/pair_of_forks_sem");
	sem_unlink("/stop_sem");
	params->forks = sem_open("/forks", O_CREAT, 0644,
			params->number_of_philosophers);
	if (params->forks == SEM_FAILED)
		return (0);
	params->print_sem = sem_open("/print_sem", O_CREAT, 0644, 1);
	if (params->print_sem == SEM_FAILED)
		return (0);
	params->death_sem = sem_open("/death_sem", O_CREAT, 0644, 1);
	if (params->death_sem == SEM_FAILED)
		return (0);
	params->pair_of_forks_sem = sem_open("/pair_of_forks_sem", O_CREAT, 0644,
			params->number_of_philosophers - 1);
	if (params->pair_of_forks_sem == SEM_FAILED)
		return (0);
	params->stop_sem = sem_open("/stop_sem", O_CREAT, 0644, 1);
	if (params->stop_sem == SEM_FAILED)
		return (0);
	return (1);
}

int	create_philosophers_recursive(t_params *params, int i)
{
	pid_t	pid;

	if (i >= params->number_of_philosophers)
		return (1);
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
	else
		params->philosophers[i].pid = pid;
	return (create_philosophers_recursive(params, i + 1));
}

int	create_philosophers(t_params *params)
{
	params->philosophers = malloc(params->number_of_philosophers
			* sizeof(t_philosopher));
	if (params->philosophers == NULL)
		return (0);
	init_colors(params->philosophers, params->number_of_philosophers);
	return (create_philosophers_recursive(params, 0));
}

int	parse_arguments(int argc, char **argv, t_params *params)
{
	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat "
			"time_to_sleep [number_of_times_each_philosopher_must_eat]\n",
			argv[0]);
		return (0);
	}
	params->number_of_philosophers = atoi(argv[1]);
	params->time_to_die = atoi(argv[2]);
	params->time_to_eat = atoi(argv[3]);
	params->time_to_sleep = atoi(argv[4]);
	if (argc == 6)
		params->number_of_times_each_philosopher_must_eat = atoi(argv[5]);
	else
		params->number_of_times_each_philosopher_must_eat = -1;
	params->stop = 0;
	return (1);
}
