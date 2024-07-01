NAME = philo_bonus
CC = cc
CFLAGS = -Wall -Wextra -Werror -lpthread -lrt

SRCS = main_bonus.c utils_bonus.c print_bonus.c color_init_bonus.c color_util_bonus.c routine_bonus.c init_bonus.c monitor_bonus.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
