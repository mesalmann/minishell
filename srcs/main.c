#include "minishell.h"
volatile sig_atomic_t g_sig = 0;

void handle_sigint(int sig)
{
    (void)sig;
    write(1, "\n", 1);

    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();

    g_sig = SIGINT;
}

void ms_loop(t_ctx *ctx, char **envp)
{
    char *line;
    char **argv;

    while (1)
    {
        if (g_sig == SIGINT)
        {
            ctx->last_status = 130;
            g_sig = 0;
        }

        line = readline("minishell$ ");

        if (line == NULL)
        {
            if (ctx->interactive)
                write(STDERR_FILENO, "exit\n", 5);
            break;
        }

        if (line[0] == '\0')
        {
            free(line);
            continue;
        }
        add_history(line);

        t_cmdnode *test_cmd = malloc(sizeof(t_cmdnode));
        test_cmd->argv = ft_split(line, ' '); // Şimdilik basitçe bölüyoruz
        test_cmd->redirs = NULL;
        test_cmd->heredocs = NULL;
        test_cmd->next = NULL;

        // Executor'ı çağırıyoruz
        ms_execute_pipeline(ctx, test_cmd);

        // Temizlik (Her döngüde free etmelisin)
        free_tab(test_cmd->argv);
        free(test_cmd);
        if (ft_strncmp(line, "$?", 3) == 0)
        {
            printf("%d\n", ctx->last_status);
            free(line);
            continue;
        }

        argv = ft_split(line, ' ');
        if (argv)
        {
            if (argv[0])
                ms_exec_simple(ctx, argv, envp);
            free_tab(argv);
        }
        free(line);
    }
}

int main(int ac, char **av, char **envp)
{
    t_ctx ctx;

    (void)ac;
    (void)av;

    signal(SIGINT, handle_sigint); // Ctrl-C ile çıkış yapmayı sağlayan signal.
    signal(SIGQUIT, SIG_IGN);

    if (ms_ctx_init(&ctx, envp) == false)
        return 1;

    ms_loop(&ctx, envp);

    return (ctx.last_status);
}