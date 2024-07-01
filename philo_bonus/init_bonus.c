/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:12:55 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 15:25:23 by jedurand         ###   ########.fr       */
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
	if (!open_semaphore(&params->forks, "/forks",
			params->number_of_philosophers)
		|| !open_semaphore(&params->print_sem, "/print_sem", 1)
		|| !open_semaphore(&params->death_sem, "/death_sem", 1)
		|| !open_semaphore(&params->pair_of_forks_sem, "/pair_of_forks_sem",
			params->number_of_philosophers - 1)
		|| !open_semaphore(&params->stop_sem, "/stop_sem", 1))
	{
		return (0);
	}
	return (1);
}

int	open_semaphore(sem_t **sem, const char *name, int value)
{
	*sem = sem_open(name, O_CREAT, 0644, value);
	if (*sem == SEM_FAILED)
		return (0);
	return (1);
}

void	cleanup_semaphores(t_params *params)
{
	sem_close(params->forks);
	sem_close(params->print_sem);
	sem_close(params->death_sem);
	sem_close(params->pair_of_forks_sem);
	sem_close(params->stop_sem);
	sem_unlink("/forks");
	sem_unlink("/print_sem");
	sem_unlink("/death_sem");
	sem_unlink("/pair_of_forks_sem");
	sem_unlink("/stop_sem");
}

int	create_philosophers(t_params *params)
{
	int	i;

	params->philosophers = mmap(NULL, params->number_of_philosophers
			* sizeof(t_philosopher), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (params->philosophers == MAP_FAILED)
		return (0);
	init_colors(params->philosophers, params->number_of_philosophers);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		if (!init_philosopher(params, i))
			return (0);
		i++;
	}
	return (1);
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
