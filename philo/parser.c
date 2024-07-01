/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:30:09 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi_absolute(char *s)
{
	int	res;

	res = 0;
	while (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r' || *s == '+'
		|| *s == '-')
		s++;
	while ('0' <= *s && *s <= '9')
		res = res * 10 + (*s++ - '0');
	return (res);
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
	params->number_of_philosophers = ft_atoi_absolute(argv[1]);
	params->time_to_die = ft_atoi_absolute(argv[2]);
	params->time_to_eat = ft_atoi_absolute(argv[3]);
	params->time_to_sleep = ft_atoi_absolute(argv[4]);
	if (argc == 6)
		params->number_of_times_each_philosopher_must_eat
			= ft_atoi_absolute(argv[5]);
	else
		params->number_of_times_each_philosopher_must_eat = -1;
	params->stop = 0;
	return (1);
}
