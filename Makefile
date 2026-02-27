NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g -I. -I/opt/homebrew/opt/readline/include
LDFLAGS = -L/opt/homebrew/opt/readline/lib
LDLIBS = -lreadline -lhistory -lncurses

SRCS = srcs/main.c srcs/init.c srcs/exec_simple.c srcs/split.c srcs/utils.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
.PHONY: all clean fclean re