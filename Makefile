NAME = minishell
CC = cc

# Normal build flags
CFLAGS = -Wall -Wextra -Werror -g -I. -I/opt/homebrew/opt/readline/include
LDFLAGS = -L/opt/homebrew/opt/readline/lib
LDLIBS = -lreadline -lhistory -lncurses

# AddressSanitizer flags
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -g3 -O0

SRCS = \
	srcs/main.c \
	srcs/init.c \
	srcs/exec_simple.c \
	srcs/split.c \
	srcs/utils.c \
	srcs/executer.c \
	srcs/builtins/builtins_utils.c \
	srcs/builtins/ms_pwd.c \
	srcs/builtins/ms_echo.c \
	srcs/builtins/ms_env.c \
	srcs/builtins/ms_cd.c \
	srcs/builtins/ms_export.c \
	srcs/builtins/ms_unset.c \
	srcs/builtins/ms_exit.c \
	srcs/lexer/lexer.c \
	srcs/syntax/syntax_check.c \
	srcs/expander/expander_core.c \
	srcs/expander/expander_vars.c \
	srcs/parser.c \
	srcs/redirs.c \
	srcs/env.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

# 🔥 ASAN build
asan: CFLAGS += $(ASAN_FLAGS)
asan: LDFLAGS += $(ASAN_FLAGS)
asan: fclean all

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re asan