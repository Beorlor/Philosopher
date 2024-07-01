/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:19:25 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:25:15 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	handle_error(const char *msg)
{
	write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
}

void	*check_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		handle_error("malloc failed");
	return (ptr);
}

int	check_pthread_create(int ret)
{
	if (ret != 0)
	{
		handle_error("pthread_create failed");
		return (0);
	}
	return (1);
}

int	check_pthread_mutex_init(int ret)
{
	if (ret != 0)
	{
		handle_error("pthread_mutex_init failed");
		return (0);
	}
	return (1);
}
