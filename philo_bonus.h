#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <pthread.h>
# include <semaphore.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/mman.h>

# define COLOR_RESET "\x1b[0m"
# define MIN_BRIGHTNESS 120
# define MAX_BRIGHTNESS 255
# define BASE_COLORS_COUNT 6

typedef struct s_philosopher
{
	int				id;
	long			last_meal_time;
	int				meals_eaten;
	pid_t			pid;
	struct s_params	*params;
	char			color[20];
}					t_philosopher;

typedef struct s_params
{
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	volatile int	stop;
	sem_t			*forks;
	sem_t			*print_sem;
	sem_t			*death_sem;
	sem_t			*pair_of_forks_sem;
	sem_t			*stop_sem;
	t_philosopher	*philosophers;
}					t_params;

long	get_timestamp(void);
void	ft_usleep(int milliseconds);
void	print_status(t_philosopher *philo, const char *status);
void	take_forks(t_philosopher *philo);
void	eat(t_philosopher *philo);
void	philosopher_routine(t_philosopher *philo);
int		initialize_semaphores(t_params *params);
void	cleanup_semaphores(t_params *params);
void	itoa_rgb(int value, char *str);
void	adjust_brightness(int *red, int *green, int *blue, int brightness);
void	construct_ansi_prefix(char **color);
void	construct_color_sequence(int red, int green, int blue, char *color);
void	init_red_color(int *color);
void	init_green_color(int *color);
void	init_blue_color(int *color);
void	init_yellow_color(int *color);
void	init_magenta_color(int *color);
void	init_cyan_color(int *color);
void	init_base_colors(int base_colors[BASE_COLORS_COUNT][3]);
void	set_brightness(int *brightness, int i, int number_of_philosophers);
void	set_philosopher_color(t_philosopher *philosopher, int base_colors[3], int brightness);
void	init_colors(t_philosopher *philosophers, int number_of_philosophers);
int		create_philosophers(t_params *params);
int		parse_arguments(int argc, char **argv, t_params *params);
void	monitor_philosopher(t_params *params, int i);
int		monitor_meals(t_params *params);
void	check_meals(t_params *params);
void	monitor_routine(t_params *params);
void	cleanup(t_params *params);

#endif
