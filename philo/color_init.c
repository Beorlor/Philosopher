/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:10:35 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:28:40 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	init_red_color(int *color)
{
	color[0] = 255;
	color[1] = 0;
	color[2] = 0;
}

void	init_green_color(int *color)
{
	color[0] = 0;
	color[1] = 255;
	color[2] = 0;
}

void	init_blue_color(int *color)
{
	color[0] = 0;
	color[1] = 0;
	color[2] = 255;
}

void	init_yellow_color(int *color)
{
	color[0] = 255;
	color[1] = 255;
	color[2] = 0;
}

void	init_magenta_color(int *color)
{
	color[0] = 255;
	color[1] = 0;
	color[2] = 255;
}
