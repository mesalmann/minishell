#include "minishell.h"
#include <unistd.h>

bool		ms_ctx_init(t_ctx *ctx, char **envp)
{
    ft_memset(ctx, 0, sizeof(t_ctx));

    if(isatty(STDIN_FILENO))
        ctx->interactive = true;
    else
        ctx->interactive = false;

    ctx->last_status = 0;
    ctx->env_dirty = true;
    ctx->envp_cache = NULL;
    
      if (envp && *envp)
    {
        // İleride burayı açacaksın:
        // if (!load_env_from_main(ctx, envp))
        //     return (false); // Malloc hatası olursa
    }
    
    return (true);
}