/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_util_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 12:02:07 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 15:11:20 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	init_cyan_color(int *color)
{
	color[0] = 0;
	color[1] = 255;
	color[2] = 255;
}

void	init_base_colors(int base_colors[BASE_COLORS_COUNT][3])
{
	init_red_color(base_colors[0]);
	init_green_color(base_colors[1]);
	init_blue_color(base_colors[2]);
	init_yellow_color(base_colors[3]);
	init_magenta_color(base_colors[4]);
	init_cyan_color(base_colors[5]);
}

void	set_brightness(int *brightness, int i, int number_of_philosophers)
{
	*brightness = MAX_BRIGHTNESS - ((i * (MAX_BRIGHTNESS - MIN_BRIGHTNESS))
			/ number_of_philosophers);
}

void	set_philosopher_color(t_philosopher *philosopher, int base_colors[3],
		int brightness)
{
	int	red;
	int	green;
	int	blue;

	red = base_colors[0];
	green = base_colors[1];
	blue = base_colors[2];
	adjust_brightness(&red, &green, &blue, brightness);
	construct_color_sequence(red, green, blue, philosopher->color);
}

void	init_colors(t_philosopher *philosophers, int number_of_philosophers)
{
	int	base_colors[BASE_COLORS_COUNT][3];
	int	brightness;
	int	i;

	i = 0;
	init_base_colors(base_colors);
	while (i < number_of_philosophers)
	{
		set_brightness(&brightness, i, number_of_philosophers);
		set_philosopher_color(&philosophers[i], base_colors[(i * 5)
			% BASE_COLORS_COUNT], brightness);
		i++;
	}
}
