/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:57:45 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 15:06:49 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

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
	munmap(params->philosophers, params->number_of_philosophers
		* sizeof(t_philosopher));
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
