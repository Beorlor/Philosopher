/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:00:16 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 12:00:46 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	construct_color_sequence(int red, int green, int blue, char *color)
{
	char	red_str[4];
	char	green_str[4];
	char	blue_str[4];
	int		i;

	construct_ansi_prefix(&color);
	itoa_rgb(red, red_str);
	itoa_rgb(green, green_str);
	itoa_rgb(blue, blue_str);
	i = 0;
	while (red_str[i])
		*color++ = red_str[i++];
	*color++ = ';';
	i = 0;
	while (green_str[i])
		*color++ = green_str[i++];
	*color++ = ';';
	i = 0;
	while (blue_str[i])
		*color++ = blue_str[i++];
	*color++ = 'm';
	*color = '\0';
}

void	print_status(t_philosopher *philo, const char *status)
{
	long	timestamp;

	timestamp = get_timestamp();
	sem_wait(philo->params->print_sem);
	sem_wait(philo->params->stop_sem);
	if (!philo->params->stop)
		printf("%s%ld %d %s%s\n", philo->color, timestamp, philo->id, status,
			COLOR_RESET);
	sem_post(philo->params->stop_sem);
	sem_post(philo->params->print_sem);
}
