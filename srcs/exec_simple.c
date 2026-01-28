#include "minishell.h"

static char *join_path(char *dir, char *cmd)
{
    char *temp;
    char *full_path;

    temp = ft_strjoin(dir, "/");
    if (!temp) return (NULL);
    full_path = ft_strjoin(temp, cmd);
    free(temp);
    return (full_path);
}

char *ms_find_path(char *cmd, char **envp)
{
    int     i;
    char    **paths;
    char    *final_path;


    if (ft_strchr(cmd, '/'))
        return (ft_strdup(cmd));
    ///bin/ls ./a.out yazsaydı:
    //PATH’e bakılmaz
    //“Bu yolu dene” denirdi
    Ama "ls" → slash yok, devam.

    i = 0;
    while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
        i++;
    //“PATH= ile başlayan env satırını bul”
    if (!envp[i]) return (NULL);

    paths = ft_split(envp[i] + 5, ':');
    //"/usr/bin:/bin"
    //→ ["/usr/bin", "/bin", NULL]
    if (!paths) return (NULL);

    i = 0;
    while (paths[i])
    {
        final_path = join_path(paths[i], cmd);
        //command ile pathi birleştirir
        // /usr/bin/ls
        if (access(final_path, X_OK) == 0) //dosya var mı ve çalıştırılabilir mi?
        {
            free_tab(paths);
            return (final_path);
        }
        free(final_path);
        i++;
    }
    free_tab(paths);
    return (NULL);
}

void ms_exec_simple(t_ctx *ctx, char **argv, char **envp)
{
    char    *path;
    pid_t   pid;
    int     status;

    path = ms_find_path(argv[0], envp);
    //komutun yolu bulunur 
    if (!path) //komut bulunamazsa
    {
        write(2, "minishell: ", 11);
        write(2, argv[0], ft_strlen(argv[0]));
        write(2, ": command not found\n", 20);
        ctx->last_status = 127; //komut yok exit statusu
        return ;
    }

    pid = fork();
    //process koopyalanır
    if (pid == 0) // child process içindeyiz
    {

        if (execve(path, argv, envp) == -1) //ls komutu execve olmazsa yani çalışmazsa perror 
        {
            perror("minishell"); 
            free(path); 
            _exit(126); //komut var ama çalıştırılamıyor exiti
        }
    }
    else if (pid > 0) //shell köşede bekler
    {
        waitpid(pid, &status, 0); //şunla bekler aslında
        while (WIFEXITED(status) == 0 && WIFSIGNALED(status) == 0) 
        {
             if (waitpid(pid, &status, 0) == -1 && errno != EINTR)
                break; 
        }

        if (WIFEXITED(status)) //normal mi öldü
            ctx->last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status)) // signal ile mi öldü
             ctx->last_status = 128 + WTERMSIG(status); 
        
        free(path);
    }
    else
        perror("fork"); //fork gerçekleşmeme durumu
    //perror kernelden gelen hata mesajını yazar
    // fork : kernel mesajı gibi 
    
}