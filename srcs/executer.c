#include "../minishell.h"

void	ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline)
{
	if (!pipeline)
		return ;
	if (!ms_run_heredocs(ctx, pipeline))
	{
		ctx->last_status = 130;
		return ;
	}
	// Şimdilik sadece tek komut (pipeline->next == NULL durumu)
	// Birden fazla komut varsa ileride pipe() ve fork() döngüsü gelecek.
	if (pipeline->next == NULL)
		ms_exec_simple(ctx, pipeline);
}