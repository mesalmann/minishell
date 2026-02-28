#include "minishell.h"
void	ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline)
{
	if (!pipeline)
		return ;
	
	// Şimdilik sadece tek komut (pipeline->next == NULL durumu)
	// Eğer birden fazla komut varsa ileride pipe() ve fork() döngüsü gelecek.
	if (pipeline->next == NULL)
	{
		// ms_exec_simple senin zaten yazdığın fonksiyon!
		// pipeline->argv, senin ms_exec_simple'ın beklediği char **argv'dir.
		ms_exec_simple(ctx, pipeline->argv, ctx->envp_cache);
	}
}