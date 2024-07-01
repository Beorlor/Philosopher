/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:27:52 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_status(t_philosopher *philo, const char *status)
{
	long	timestamp;

	pthread_mutex_lock(&philo->params->stop_mutex);
	if (philo->params->stop)
	{
		pthread_mutex_unlock(&philo->params->stop_mutex);
		return ;
	}
	pthread_mutex_unlock(&philo->params->stop_mutex);
	timestamp = get_timestamp();
	pthread_mutex_lock(&philo->params->print_mutex);
	if (!philo->params->stop)
		printf("%s%ld %d %s%s\n", philo->color, timestamp, philo->id, status,
			COLOR_RESET);
	pthread_mutex_unlock(&philo->params->print_mutex);
}

void	itoa_rgb(int value, char *str)
{
	if (value >= 100)
	{
		*str++ = '0' + (value / 100);
		value %= 100;
	}
	if (value >= 10)
		*str++ = '0' + (value / 10);
	*str++ = '0' + (value % 10);
	*str = '\0';
}

void	adjust_brightness(int *red, int *green, int *blue, int brightness)
{
	if (*red)
		*red = (*red * brightness) / 255;
	if (*green)
		*green = (*green * brightness) / 255;
	if (*blue)
		*blue = (*blue * brightness) / 255;
	if (*red < MIN_BRIGHTNESS && *red)
		*red += (MIN_BRIGHTNESS / 2);
	if (*green < MIN_BRIGHTNESS && *green)
		*green += (MIN_BRIGHTNESS / 2);
	if (*blue < MIN_BRIGHTNESS && *blue)
		*blue += (MIN_BRIGHTNESS / 2);
}

void	construct_ansi_prefix(char **color)
{
	*(*color)++ = '\x1b';
	*(*color)++ = '[';
	*(*color)++ = '3';
	*(*color)++ = '8';
	*(*color)++ = ';';
	*(*color)++ = '2';
	*(*color)++ = ';';
}

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
