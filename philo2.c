/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo copy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 12:00:00 by assistant         #+#    #+#             */
/*   Updated: 2024/06/29 06:12:59 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define COLOR_RESET "\x1b[0m"
#define MIN_BRIGHTNESS 120
#define MAX_BRIGHTNESS 255
#define BASE_COLORS_COUNT 6
#define RED_INDEX 0
#define GREEN_INDEX 1
#define BLUE_INDEX 2

#define RED_COLOR {255, 0, 0}
#define GREEN_COLOR {0, 255, 0}
#define BLUE_COLOR {0, 0, 255}
#define YELLOW_COLOR {255, 255, 0}
#define MAGENTA_COLOR {255, 0, 255}
#define CYAN_COLOR {0, 255, 255}

typedef struct s_philosopher
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	long			last_meal_time;
	int				meals_eaten;
	struct s_params	*params;
	char			color[20];
}	t_philosopher;

typedef struct s_params
{
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	int				stop;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	meals_mutex;
	pthread_mutex_t	stop_mutex;
	t_philosopher	*philosophers;
}	t_params;

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

long	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_usleep(int milliseconds)
{
	long	start;

	start = get_timestamp();
	while ((get_timestamp() - start) < milliseconds)
		usleep(10);
}

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
	*red = *red < MIN_BRIGHTNESS && *red ? *red + (MIN_BRIGHTNESS / 2) : *red;
	*green = *green < MIN_BRIGHTNESS && *green ? *green + (MIN_BRIGHTNESS / 2) : *green;
	*blue = *blue < MIN_BRIGHTNESS && *blue ? *blue + (MIN_BRIGHTNESS / 2) : *blue;
}

void	construct_color_sequence(int red, int green, int blue, char *color)
{
	char	red_str[4];
	char	green_str[4];
	char	blue_str[4];
	int		i;

	*color++ = '\x1b';
	*color++ = '[';
	*color++ = '3';
	*color++ = '8';
	*color++ = ';';
	*color++ = '2';
	*color++ = ';';
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

void	init_colors(t_philosopher *philosophers, int number_of_philosophers)
{
	const int	base_colors[BASE_COLORS_COUNT][3] = {
		RED_COLOR, GREEN_COLOR, BLUE_COLOR, YELLOW_COLOR,
		MAGENTA_COLOR, CYAN_COLOR
	};
	int			i;
	int			base_index;
	int			red;
	int			green;
	int			blue;
	int			brightness;

	i = 0;
	while (i < number_of_philosophers)
	{
		base_index = (i * 5) % BASE_COLORS_COUNT;
		red = base_colors[base_index][RED_INDEX];
		green = base_colors[base_index][GREEN_INDEX];
		blue = base_colors[base_index][BLUE_INDEX];
		brightness = MAX_BRIGHTNESS - ((i * (MAX_BRIGHTNESS - MIN_BRIGHTNESS))
				/ (number_of_philosophers));
		adjust_brightness(&red, &green, &blue, brightness);
		construct_color_sequence(red, green, blue, philosophers[i].color);
		i++;
	}
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	int	should_stop;

	philo = (t_philosopher *)arg;
	should_stop = philo->params->stop;
	while (!should_stop)
	{
		print_status(philo, "is thinking");
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
		if (philo->left_fork == philo->right_fork)
		{
			ft_usleep(philo->params->time_to_die);
			print_status(philo, "died");
			pthread_mutex_lock(&philo->params->stop_mutex);
			should_stop = philo->params->stop = 1;
			pthread_mutex_unlock(&philo->params->stop_mutex);
			pthread_mutex_unlock(philo->left_fork);
			break ;
		}
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a second fork");
		pthread_mutex_lock(&philo->params->death_mutex);
		philo->last_meal_time = get_timestamp();
		print_status(philo, "is eating");
		pthread_mutex_unlock(&philo->params->death_mutex);
		ft_usleep(philo->params->time_to_eat);
		pthread_mutex_lock(&philo->params->meals_mutex);
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->params->meals_mutex);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		print_status(philo, "is sleeping");
		ft_usleep(philo->params->time_to_sleep);
		pthread_mutex_lock(&philo->params->stop_mutex);
		should_stop = philo->params->stop;
		pthread_mutex_unlock(&philo->params->stop_mutex);
	}
	return (NULL);
}

int	ft_atoi_absolute(char *s)
{
	int res;

	res = 0;
	while (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r' || *s == '+' || *s == '-')
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
			"time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (0);
	}
	params->number_of_philosophers = ft_atoi_absolute(argv[1]);
	params->time_to_die = ft_atoi_absolute(argv[2]);
	params->time_to_eat = ft_atoi_absolute(argv[3]);
	params->time_to_sleep = ft_atoi_absolute(argv[4]);
	params->number_of_times_each_philosopher_must_eat = (argc == 6) ? ft_atoi_absolute(argv[5]) : -1;
	params->stop = 0;
	return (1);
}

int	initialize_philosophers(t_params *params)
{
	int	i;

	params->forks = check_malloc(params->number_of_philosophers
			* sizeof(pthread_mutex_t));
	if (!params->forks)
		return (0);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		if (!check_pthread_mutex_init(pthread_mutex_init(&params->forks[i], NULL)))
			return (0);
		i++;
	}
	if (!check_pthread_mutex_init(pthread_mutex_init(&params->print_mutex, NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->death_mutex, NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->meals_mutex, NULL))
		|| !check_pthread_mutex_init(pthread_mutex_init(&params->stop_mutex, NULL)))
		return (0);
	params->philosophers = check_malloc(params->number_of_philosophers
			* sizeof(t_philosopher));
	if (!params->philosophers)
		return (0);
	init_colors(params->philosophers, params->number_of_philosophers);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		params->philosophers[i].id = i + 1;
		if (params->philosophers[i].id % 2 == 0)
		{
			params->philosophers[i].left_fork = &params->forks[i];
			params->philosophers[i].right_fork = &params->forks[(i + 1) % params->number_of_philosophers];
		}
		else
		{
			params->philosophers[i].left_fork = &params->forks[(i + 1) % params->number_of_philosophers];
			params->philosophers[i].right_fork = &params->forks[i];
		}
		params->philosophers[i].last_meal_time = get_timestamp();
		params->philosophers[i].meals_eaten = 0;
		params->philosophers[i].params = params;
		if (!check_pthread_create(pthread_create(&params->philosophers[i].thread,
				NULL, philosopher_routine, &params->philosophers[i])))
			return (0);
		i++;
	}
	return (1);
}

void	cleanup(t_params *params)
{
	int	i;

	i = 0;
	while (i < params->number_of_philosophers)
	{
		pthread_mutex_destroy(&params->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&params->print_mutex);
	pthread_mutex_destroy(&params->death_mutex);
	pthread_mutex_destroy(&params->meals_mutex);
	free(params->forks);
	free(params->philosophers);
}

void	*monitor_routine(void *arg)
{
	t_params	*params;
	int			i;
	int			all_philosophers_done;
	int			should_stop;

	params = (t_params *)arg;
	should_stop = params->stop;
	while (!should_stop)
	{
		all_philosophers_done = 1;
		i = 0;
		while (i < params->number_of_philosophers)
		{
			pthread_mutex_lock(&params->death_mutex);
			if ((get_timestamp() - params->philosophers[i].last_meal_time)
				> params->time_to_die)
			{
				print_status(&params->philosophers[i], "died");
				pthread_mutex_lock(&params->stop_mutex);
				params->stop = 1;
				pthread_mutex_unlock(&params->stop_mutex);
			}
			pthread_mutex_unlock(&params->death_mutex);
			if (params->number_of_times_each_philosopher_must_eat > 0)
			{
				pthread_mutex_lock(&params->meals_mutex);
				if (params->philosophers[i].meals_eaten
					< params->number_of_times_each_philosopher_must_eat)
					all_philosophers_done = 0;
				pthread_mutex_unlock(&params->meals_mutex);
			}
			else
				all_philosophers_done = 0;
			if (params->stop)
				break ;
			i++;
		}
		if (params->number_of_times_each_philosopher_must_eat > 0
			&& all_philosophers_done)
		{
			pthread_mutex_lock(&params->print_mutex);
			printf("All philosophers have eaten at least %d times.\n",
				params->number_of_times_each_philosopher_must_eat);
			pthread_mutex_lock(&params->stop_mutex);
			params->stop = 1;
			pthread_mutex_unlock(&params->stop_mutex);
			pthread_mutex_unlock(&params->print_mutex);
		}
		pthread_mutex_lock(&params->stop_mutex);
		should_stop = params->stop;
		pthread_mutex_unlock(&params->stop_mutex);
		usleep(100);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_params	params;
	pthread_t	monitor_thread;
	int			i;

	if (!parse_arguments(argc, argv, &params))
		return (1);
	if (!initialize_philosophers(&params))
	{
		cleanup(&params);
		return (1);
	}
	if (!check_pthread_create(pthread_create(&monitor_thread, NULL,
			monitor_routine, &params)))
	{
		cleanup(&params);
		return (1);
	}
	pthread_join(monitor_thread, NULL);
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_join(params.philosophers[i].thread, NULL);
		i++;
	}
	cleanup(&params);
	return (0);
}
