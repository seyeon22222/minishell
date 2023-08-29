#include "../include/minishell.h"

void	search_and_fork_exec(char **argv, t_context *p_ctx)
{
	char	*order;
	char	**path;
	char	*temp_path;

	temp_path = ft_getenv("PATH");
	if (!temp_path)
	{
		msh_error(argv[0], "command not found", 0);
		p_ctx->exit_status = 127;
		fork_error(p_ctx);
		return ;
	}
	path = path_split(temp_path, ':');
	order = make_order(path, argv);
	if (order)
	{
		free(argv[0]);
		argv[0] = order;
		fork_exec(argv, p_ctx);
	}
	else
	{
		if (p_ctx->fd[STDIN] != STDIN)
			close(p_ctx->fd[STDIN]);
		if (p_ctx->fd[STDOUT] != STDOUT)
			close(p_ctx->fd[STDOUT]);
		msh_error(argv[0], "command not found", 0);
		p_ctx->exit_status = 127;
		fork_error(p_ctx);
	}
	free_argv(path);
	free(temp_path);
}

char	*make_order(char **path, char **argv)
{
	struct stat	buff;
	int			idx;
	char		*order;

	idx = 0;
	order = NULL;
	while (path[idx])
	{
		order = ft_strjoin(path[idx], argv[0]);
		stat(order, &buff);
		if (access(order, X_OK) == 0 && (buff.st_mode & S_IFMT) != S_IFDIR)
			return (order);
		free(order);
		idx++;
	}
	return (NULL);
}

void	forked_builtin(t_context *p_ctx, t_builtin	builtin_func, char **argv)
{
	int		pid;
	int		builtin_exit_status;

	if (!*get_is_subshell())
		sigact_fork_mode();
	pid = fork();
	if (pid == 0)
	{
		sigact_modeoff();
		dup2(p_ctx->fd[STDIN], STDIN);
		dup2(p_ctx->fd[STDOUT], STDOUT);
		if (p_ctx->fd_close >= 0)
		{
			close(p_ctx->fd_close);
			p_ctx->fd_close = -1;
		}
		builtin_exit_status = builtin_func(argv);
		exit(builtin_exit_status);
	}
	if (p_ctx->fd[STDIN] != STDIN)
		close(p_ctx->fd[STDIN]);
	if (p_ctx->fd[STDOUT] != STDOUT)
		close(p_ctx->fd[STDOUT]);
	enqueue_after(pid, p_ctx);
}
