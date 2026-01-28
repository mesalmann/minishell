#include "minishell.h"
volatile sig_atomic_t g_sig = 0;

void handle_sigint(int sig)
//ctrl c kısmında ne yapılacağı  ele alınmış
{
    (void)sig;
    write(1, "\n", 1);
    
    rl_on_new_line();
    //imleç yeni satırda
    rl_replace_line("", 0);
    //halihazırda yazılmış komut satırını sil
    rl_redisplay();
    //boş satırı göster

    g_sig = SIGINT;
    //last status güncellensin diye
    //last_status, shell’in en son çalıştırdığı komutun nasıl bittiğini tutar. $? ile bakılabilir
}

void ms_loop(t_ctx *ctx, char **envp)
{
    char *line;
    char **argv;

    while(1)
    //shelll sonsuz döngüde çalışacak
    {
        if (g_sig == SIGINT)
        {
            ctx->last_status = 130;
            // 0-128 normal exit, 128+ signal kaynaklı exit
            //SIGINT = 2 , SIGQUIT = 3, SIGKILL = 9, SIGSEGV = 11...
            g_sig = 0;
        }

        line = readline("minishell$ ");
        //ekrana prompt basar, kullanıcıdan bir satır yazı bekler

        if(line == NULL)
        //ctrl+d
        //terminal kapanır, stdin e EOF gönderilir
        {
            if(ctx->interactive)
                write(STDERR_FILENO, "exit\n", 5);
            break;
        }   

        if(line[0] == '\0')
        //terminalde bi şey yapmadan sadece enter a basma durumu
        {
            free(line);
            continue;
        }
        add_history(line);
        //istisnai durumlar yoksa line ı history e ekler

        if (ft_strncmp(line, "$?", 3) == 0)
        //$? last statusu kontrol etmemizi sağlayan prompt
        {
            printf("%d\n", ctx->last_status);
            free(line);
            continue;
        }
        argv = ft_split(line, ' ');
        //lexer kısmı 
        // token list oluşturmayı sağlar
        if (argv)
        {
            if (argv[0]) // en az bir argüman varsa
                ms_exec_simple(ctx, argv, envp); //çalıştırılacak
            free_tab(argv);//token list temizle
        }
        free(line);//prompt satırını temizle
    }
}
   
int main (int ac, char **av, char **envp)
{
    t_ctx ctx;

    (void)ac;
    (void)av;
    
    signal(SIGINT, handle_sigint);
    //ctrl c gelince ne tetiklenicek
    signal(SIGQUIT, SIG_IGN);
    // Ctrl+\ 'i yoksay
    
    if(ms_ctx_init(&ctx, envp) == false)   // Context'i başlat
        return 1; // Başarısızsa çık

    ms_loop(&ctx, envp); // Ana döngüyü başlat

    return (ctx.last_status);   // Son komutun exit kodu ile çık
}