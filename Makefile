NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g -I. 

SRCS        = srcs/main.c \
              srcs/init.c \
              srcs/exec_simple.c \
              srcs/split.c \
              srcs/utils.c


OBJS        = $(SRCS:.c=.o)


LIBS        = -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)
	@echo "✅ Minishell hazır!"

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re