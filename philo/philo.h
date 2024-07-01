/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedurand <jedurand@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 11:33:09 by jedurand          #+#    #+#             */
/*   Updated: 2024/07/01 11:52:48 by jedurand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

# define COLOR_RESET "\x1b[0m"
# define MIN_BRIGHTNESS 120
# define MAX_BRIGHTNESS 255
# define BASE_COLORS_COUNT 6

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
}					t_philosopher;

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
}					t_params;

/* Function declarations */

/* main.c */
void				cleanup(t_params *params);
int					main(int argc, char **argv);

/* utils.c */
int					ft_strlen(const char *s);
void				handle_error(const char *msg);
void				*check_malloc(size_t size);
int					check_pthread_create(int ret);
int					check_pthread_mutex_init(int ret);

/* time.c */
long				get_timestamp(void);
void				ft_usleep(int milliseconds);

/* print.c */
void				print_status(t_philosopher *philo, const char *status);
void				itoa_rgb(int value, char *str);
void				adjust_brightness(int *red, int *green, int *blue, int brightness);
void				construct_ansi_prefix(char **color);
void				construct_color_sequence(int red, int green, int blue, char *color);

/* color_init.c */
void				init_red_color(int *color);
void				init_green_color(int *color);
void				init_blue_color(int *color);
void				init_yellow_color(int *color);
void				init_magenta_color(int *color);

/* color_util.c */
void				init_cyan_color(int *color);
void				init_base_colors(int base_colors[BASE_COLORS_COUNT][3]);
void				set_brightness(int *brightness, int i, int number_of_philosophers);
void				set_philosopher_color(t_philosopher *philosopher, int base_colors[3], int brightness);
void				init_colors(t_philosopher *philosophers, int number_of_philosophers);

/* routine.c */
void				take_forks(t_philosopher *philo);
void				eat(t_philosopher *philo);
void				*philosopher_routine(void *arg);

/* parser.c */
int					ft_atoi_absolute(char *s);
int					parse_arguments(int argc, char **argv, t_params *params);

/* init.c */
int					init_mutexes(t_params *params);
int					init_philosopher_forks(t_params *params, int i);
int					init_forks(t_params *params);
int					init_philosophers(t_params *params);
int					initialize_philosophers(t_params *params);

/* monitor.c */
void				monitor_philosopher(t_params *params, int i);
int					monitor_meals(t_params *params);
void				check_meals(t_params *params);
void				*monitor_routine(void *arg);

#endif
