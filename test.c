#include "philo_bonus.h"

long	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_usleep(int milliseconds)
{
	long	start;
	long	current_time;

	start = get_timestamp();
	current_time = start;
	while ((current_time - start) < milliseconds)
	{
		usleep(10);
		current_time = get_timestamp();
	}
}

void	print_status(t_philosopher *philo, const char *status)
{
	long	timestamp;

	timestamp = get_timestamp();
	sem_wait(philo->params->print_sem);
	sem_wait(philo->params->stop_sem);
	if (!philo->params->stop)
	{
		printf("%s%ld %d %s%s\n", philo->color, timestamp, philo->id, status,
			COLOR_RESET);
	}
	sem_post(philo->params->stop_sem);
	sem_post(philo->params->print_sem);
}

void	take_forks(t_philosopher *philo)
{
	if (philo->params->number_of_philosophers == 1)
	{
		sem_wait(philo->params->forks);
		print_status(philo, "has taken a fork");
		ft_usleep(philo->params->time_to_die);
		sem_wait(philo->params->stop_sem);
		philo->params->stop = 1;
		sem_post(philo->params->stop_sem);
		sem_post(philo->params->forks);
		return ;
	}
	sem_wait(philo->params->pair_of_forks_sem);
	sem_wait(philo->params->forks);
	print_status(philo, "has taken a fork");
	sem_wait(philo->params->forks);
	print_status(philo, "has taken a second fork");
}

void	eat(t_philosopher *philo)
{
	sem_wait(philo->params->death_sem);
	philo->last_meal_time = get_timestamp();
	print_status(philo, "is eating");
	sem_post(philo->params->death_sem);
	ft_usleep(philo->params->time_to_eat);
	philo->meals_eaten++;
	sem_post(philo->params->forks);
	sem_post(philo->params->forks);
	sem_post(philo->params->pair_of_forks_sem);
}

void	philosopher_routine(t_philosopher *philo)
{
	int	should_stop;

	should_stop = 0;
	while (!should_stop)
	{
		sem_wait(philo->params->stop_sem);
		if (philo->params->stop)
		{
			sem_post(philo->params->stop_sem);
			break ;
		}
		sem_post(philo->params->stop_sem);
		print_status(philo, "is thinking");
		take_forks(philo);
		if (philo->params->number_of_philosophers == 1)
			break ;
		eat(philo);
		print_status(philo, "is sleeping");
		ft_usleep(philo->params->time_to_sleep);
	}
	sem_close(philo->params->forks);
	sem_close(philo->params->print_sem);
	sem_close(philo->params->death_sem);
	sem_close(philo->params->pair_of_forks_sem);
	sem_close(philo->params->stop_sem);
	exit(0);
}

int	initialize_semaphores(t_params *params)
{
	params->forks = SEM_FAILED;
	params->print_sem = SEM_FAILED;
	params->death_sem = SEM_FAILED;
	params->pair_of_forks_sem = SEM_FAILED;
	params->stop_sem = SEM_FAILED;
	sem_unlink("/forks");
	sem_unlink("/print_sem");
	sem_unlink("/death_sem");
	sem_unlink("/pair_of_forks_sem");
	sem_unlink("/stop_sem");
	params->forks = sem_open("/forks", O_CREAT, 0644,
			params->number_of_philosophers);
	if (params->forks == SEM_FAILED)
		return (0);
	params->print_sem = sem_open("/print_sem", O_CREAT, 0644, 1);
	if (params->print_sem == SEM_FAILED)
		return (0);
	params->death_sem = sem_open("/death_sem", O_CREAT, 0644, 1);
	if (params->death_sem == SEM_FAILED)
		return (0);
	params->pair_of_forks_sem = sem_open("/pair_of_forks_sem", O_CREAT, 0644,
			params->number_of_philosophers - 1);
	if (params->pair_of_forks_sem == SEM_FAILED)
		return (0);
	params->stop_sem = sem_open("/stop_sem", O_CREAT, 0644, 1);
	if (params->stop_sem == SEM_FAILED)
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

void	itoa_rgb(int value, char *str)
{
	int	temp;

	if (value >= 100)
	{
		*str++ = '0' + (value / 100);
		value %= 100;
	}
	if (value >= 10)
	{
		*str++ = '0' + (value / 10);
	}
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

int	create_philosophers(t_params *params)
{
	int		i;
	pid_t	pid;

	params->philosophers = mmap(NULL, params->number_of_philosophers
			* sizeof(t_philosopher), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (params->philosophers == MAP_FAILED)
		return (0);
	init_colors(params->philosophers, params->number_of_philosophers);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		params->philosophers[i].id = i + 1;
		params->philosophers[i].last_meal_time = get_timestamp();
		params->philosophers[i].meals_eaten = 0;
		params->philosophers[i].params = params;
		pid = fork();
		if (pid == 0)
		{
			philosopher_routine(&params->philosophers[i]);
			exit(0);
		}
		else if (pid < 0)
		{
			perror("Failed to fork philosopher process");
			return (0);
		}
		else
			params->philosophers[i].pid = pid;
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

void	monitor_philosopher(t_params *params, int i)
{
	long	time_diff;

	sem_wait(params->death_sem);
	time_diff = get_timestamp() - params->philosophers[i].last_meal_time;
	if (time_diff > params->time_to_die)
	{
		print_status(&params->philosophers[i], "died");
		sem_wait(params->stop_sem);
		params->stop = 1;
		sem_post(params->stop_sem);
		i = 0;
		while (i < params->number_of_philosophers)
		{
			kill(params->philosophers[i].pid, SIGTERM);
			i++;
		}
	}
	sem_post(params->death_sem);
}

int	monitor_meals(t_params *params)
{
	int	i;
	int	all_philosophers_done;

	i = 0;
	all_philosophers_done = 1;
	while (i < params->number_of_philosophers)
	{
		monitor_philosopher(params, i);
		if (params->number_of_times_each_philosopher_must_eat > 0)
		{
			if (params->philosophers[i].meals_eaten
				< params->number_of_times_each_philosopher_must_eat)
				all_philosophers_done = 0;
		}
		else
		{
			all_philosophers_done = 0;
		}
		sem_wait(params->stop_sem);
		if (params->stop)
		{
			sem_post(params->stop_sem);
			break ;
		}
		sem_post(params->stop_sem);
		i++;
	}
	return (all_philosophers_done);
}

void	check_meals(t_params *params)
{
	int	i;

	sem_wait(params->print_sem);
	printf("All philosophers have eaten at least %d times.\n",
		params->number_of_times_each_philosopher_must_eat);
	sem_wait(params->stop_sem);
	params->stop = 1;
	sem_post(params->stop_sem);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		kill(params->philosophers[i].pid, SIGTERM);
		i++;
	}
	sem_post(params->print_sem);
}

void	monitor_routine(t_params *params)
{
	int	should_stop;

	should_stop = 0;
	while (!should_stop)
	{
		sem_wait(params->stop_sem);
		if (params->stop)
		{
			sem_post(params->stop_sem);
			break ;
		}
		sem_post(params->stop_sem);
		if (monitor_meals(params))
			check_meals(params);
		usleep(100);
	}
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
