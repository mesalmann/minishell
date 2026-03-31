NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -Iincludes
LDFLAGS = -lreadline

SRC = \
	srcs/main/main.c \
	srcs/main/init.c \
	srcs/main/init_utils.c \
	srcs/main/main_utils.c \
	srcs/executor/exec_simple.c \
	srcs/executor/exec_cmd.c \
	srcs/executor/exec_cmd_utils.c \
	srcs/executor/executer.c \
	srcs/executor/executer_child.c \
	srcs/executor/executer_pipeline.c \
	srcs/executor/executer_utils.c \
	srcs/executor/redirs.c \
	srcs/executor/redirs_heredoc.c \
	srcs/executor/redirs_heredoc_utils.c \
	srcs/executor/redirs_utils.c \
	srcs/parser/split.c \
	srcs/parser/parser.c \
	srcs/parser/parser_cmd.c \
	srcs/parser/parser_fill.c \
	srcs/parser/parser_io.c \
	srcs/parser/parser_free.c \
	srcs/parser/syntax_check.c \
	srcs/utils/utils.c \
	srcs/utils/utils2.c \
	srcs/utils/utils3.c \
	srcs/builtins/builtins_utils.c \
	srcs/builtins/ms_pwd.c \
	srcs/builtins/ms_echo.c \
	srcs/builtins/ms_env.c \
	srcs/builtins/ms_env_utils.c \
	srcs/builtins/ms_env_utils2.c \
	srcs/builtins/ms_cd.c \
	srcs/builtins/ms_export.c \
	srcs/builtins/ms_export_utils.c \
	srcs/builtins/ms_unset.c \
	srcs/builtins/ms_exit.c \
	srcs/lexer/lexer.c \
	srcs/lexer/lexer_tokens.c \
	srcs/lexer/lexer_word.c \
	srcs/lexer/lexer_operator.c \
	srcs/lexer/lexer_utils.c \
	srcs/expander/expander_core.c \
	srcs/expander/expander_vars.c \
	srcs/expander/expander_vars_utils.c \
	srcs/expander/expander_sb.c \
	srcs/expander/expander_split.c \
	srcs/signals/signals.c \
	srcs/signals/signals_child.c \
	srcs/env/env.c \
	srcs/env/env_build.c \
	srcs/env/env_utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
