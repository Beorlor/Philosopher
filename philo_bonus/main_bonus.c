/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:57:45 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 12:18:18 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	kill_all_philosophers(t_params *params, int j)
{
	if (j < params->number_of_philosophers)
	{
		kill(params->philosophers[j].pid, SIGTERM);
		kill_all_philosophers(params, j + 1);
	}
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

void	cleanup(t_params *params)
{
	int	i;

	i = 0;
	while (i < params->number_of_philosophers)
	{
		waitpid(params->philosophers[i].pid, NULL, 0);
		i++;
	}
	cleanup_semaphores(params);
	free(params->philosophers);
}

int	main(int argc, char **argv)
{
	t_params	params;

	if (!parse_arguments(argc, argv, &params))
		return (1);
	if (!initialize_semaphores(&params))
	{
		cleanup_semaphores(&params);
		return (1);
	}
	if (!create_philosophers(&params))
	{
		cleanup_semaphores(&params);
		return (1);
	}
	monitor_routine(&params);
	cleanup(&params);
	return (0);
}
