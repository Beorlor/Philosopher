NAME = philo
CC = cc
CFLAGS = -Wall -Wextra -Werror -lpthread -lrt

SRCS = main.c utils.c time.c print.c color_init.c color_util.c routine.c parser.c init.c monitor.c
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
