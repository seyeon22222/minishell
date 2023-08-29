#include "../include/minishell.h"
#include "../include/execute.h"
#include "../include/exec_node_util.h"
#include "../include/exec_word_util.h"
#include "../include/make_argv_util.h"
#include "../include/execute_util.h"
#include "../include/filename_expansion.h"
#include "../include/arg_expansion.h"
#include "../include/wait_queue.h"
#include "../include/ms_signal.h"

static t_bool	check_str(char *argv, int idx, int size, char *sep);

void	fork_error(t_context *p_ctx)
{
	int	pid;

	pid = fork();
	if (pid == 0)
		exit(p_ctx->exit_status);
	enqueue_after(pid, p_ctx);
}

void	exec_subshell(t_node *node, t_context *p_ctx)
{
	int		pid;
	t_node	*lhs;

	lhs = node->left;
	if (!*get_is_subshell())
		sigact_fork_mode();
	set_is_subshell(TRUE);
	pid = fork();
	if (pid == 0)
	{
		sigact_modeoff();
		if (p_ctx->fd_close >= 0)
			close(p_ctx->fd_close);
		exec_node(lhs, p_ctx);
		wait_queue_after(p_ctx);
		exit(p_ctx->exit_status);
	}
	set_is_subshell(FALSE);
	if (p_ctx->fd[STDIN] != STDIN)
		close(p_ctx->fd[STDIN]);
	if (p_ctx->fd[STDOUT] != STDOUT)
		close(p_ctx->fd[STDOUT]);
	enqueue_after(pid, p_ctx);
}

void	exec_or(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;
	t_node	*rhs;

	lhs = node->left;
	rhs = node->right;
	exec_node(lhs, p_ctx);
	find_last_pid(p_ctx);
	wait_queue_after(p_ctx);
	if (*get_last_exit_status() != 0)
		exec_node(rhs, p_ctx);
}

void	exec_and(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;
	t_node	*rhs;

	lhs = node->left;
	rhs = node->right;
	exec_node(lhs, p_ctx);
	find_last_pid(p_ctx);
	wait_queue_after(p_ctx);
	if (*get_last_exit_status() == 0)
		exec_node(rhs, p_ctx);
}

void	exec_pipe(t_node *node, t_context *p_ctx)
{
	t_node		*lhs;
	t_node		*rhs;
	int			pipe_fd[2];
	t_context	aux;

	p_ctx->is_piped_cmd = TRUE;
	lhs = node->left;
	rhs = node->right;
	pipe(pipe_fd);
	aux = *p_ctx;
	aux.fd[STDOUT] = pipe_fd[STDOUT];
	aux.fd_close = pipe_fd[STDIN];
	exec_node(lhs, &aux);
	p_ctx->pid_list = aux.pid_list;
	p_ctx->pid_size = aux.pid_size;
	aux = *p_ctx;
	aux.fd[STDIN] = pipe_fd[STDIN];
	aux.fd_close = pipe_fd[STDOUT];
	exec_node(rhs, &aux);
	p_ctx->pid_list = aux.pid_list;
	p_ctx->pid_size = aux.pid_size;

	p_ctx->is_piped_cmd = FALSE;
}

static t_bool	is_regular_file(char *filename, t_context *p_ctx)
{
	struct stat	buff;

	if (access(filename, F_OK) != 0)
	{
		msh_error(filename, NULL, ENOENT);
		p_ctx->exit_status = 127;
		return (FALSE);
	}
	stat(filename, &buff);
	if ((buff.st_mode & S_IFMT) == S_IFDIR)
	{
		msh_error(filename, NULL, EISDIR);
		p_ctx->exit_status = 126;
		return (FALSE);
	}
	return (TRUE);
}

t_bool	*get_redirect_ambiguity(void)
{
	static t_bool	redirect_ambiguity;

	return (&redirect_ambiguity);
}

void	set_redirect_ambiguity(t_bool value)
{
	*get_redirect_ambiguity() = value;
}

void	exec_input(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;
	t_node	*rhs;
	char	**filename;

	lhs = node->left;
	rhs = node->right;
	if (p_ctx->fd[STDIN] != STDIN)
		close(p_ctx->fd[STDIN]);
	set_redirect_ambiguity(TRUE);
	filename = (char **)make_argv(rhs->word);
	if (*get_redirect_ambiguity() == FALSE)
	{
		p_ctx->exit_status = 1;
		fork_error(p_ctx);
		return ;
	}
	set_redirect_ambiguity(FALSE);
	if (!is_regular_file(filename[0], p_ctx))
		fork_error(p_ctx);
	else
	{
		p_ctx->fd[STDIN] = open(filename[0], O_RDONLY, 0644);
		exec_node(lhs, p_ctx);
	}
	free_argv(filename);
	return ;
}

void	exec_heredoc(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;

	lhs = node->left;
	if (p_ctx->fd[STDIN] != STDIN)
		close(p_ctx->fd[STDIN]);
	p_ctx->fd[STDIN] = \
		open(p_ctx->heredoc_file_name[p_ctx->heredoc_file_idx++], \
		O_RDONLY, 0644);
	exec_node(lhs, p_ctx);
}

static t_bool	is_not_directory(char *filename, t_context *p_ctx)
{
	struct stat	buff;

	stat(filename, &buff);
	if ((buff.st_mode & S_IFMT) == S_IFDIR)
	{
		msh_error(filename, NULL, EISDIR);
		p_ctx->exit_status = 126;
		return (FALSE);
	}
	return (TRUE);
}

void	exec_output(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;
	t_node	*rhs;
	char	**filename;

	lhs = node->left;
	rhs = node->right;
	if (p_ctx->fd[STDOUT] != STDOUT)
		close(p_ctx->fd[STDOUT]);
	set_redirect_ambiguity(TRUE);
	filename = make_argv(rhs->word);
	if (*get_redirect_ambiguity() == FALSE)
	{
		p_ctx->exit_status = 1;
		fork_error(p_ctx);
		return ;
	}
	set_redirect_ambiguity(FALSE);
	if (!is_not_directory(filename[0], p_ctx))
		fork_error(p_ctx);
	else
	{
		p_ctx->fd[STDOUT] = open(filename[0], O_CREAT | O_TRUNC | O_WRONLY, 0644);
		exec_node(lhs, p_ctx);
	}
	free_argv(filename);
	return ;
}

void	exec_append(t_node *node, t_context *p_ctx)
{
	t_node	*lhs;
	t_node	*rhs;
	char	**filename;

	lhs = node->left;
	rhs = node->right;
	if (p_ctx->fd[STDOUT] != STDOUT)
		close(p_ctx->fd[STDOUT]);
	set_redirect_ambiguity(TRUE);
	filename = make_argv(rhs->word);
	if (*get_redirect_ambiguity() == FALSE)
	{
		p_ctx->exit_status = 1;
		fork_error(p_ctx);
		return ;
	}
	set_redirect_ambiguity(FALSE);
	if (!is_not_directory(filename[0], p_ctx))
		fork_error(p_ctx);
	else
	{
		p_ctx->fd[STDOUT] = open(filename[0], O_CREAT | O_APPEND | O_WRONLY, 0644);
		exec_node(lhs, p_ctx);
	}
	free_argv(filename);
	return ;
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
	path = ft_split2(temp_path, ':');
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

void redirect_fd(int dst[2])
{
	dup2(dst[STDIN], STDIN);
	dup2(dst[STDOUT], STDOUT);
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

t_bool	exec_builtin(char **argv, t_context *p_ctx)
{
	t_bool		can_builtin;
	t_builtin	builtin_func;
	int			builtin_exit_status;
	int			tmp_fd[2];

	can_builtin = FALSE;
	builtin_func = check_builtin(argv[0]);
	if (builtin_func)
	{
		if (p_ctx->is_piped_cmd)
			forked_builtin(p_ctx, builtin_func, argv);
		else
		{
			tmp_fd[STDIN] = dup(STDIN);
			tmp_fd[STDOUT] = dup(STDOUT);
			redirect_fd(p_ctx->fd);
			builtin_exit_status = builtin_func(argv);
			redirect_fd(tmp_fd);
			p_ctx->exit_status = builtin_exit_status;
			set_last_exit_status(p_ctx->exit_status);
		}
		can_builtin = TRUE;
	}
	return (can_builtin);
}

t_builtin	check_builtin(char *argv)
{
	if (argv[0] == 'c' && check_str(argv, 1, 1, "d"))
		return (ft_cd);
	else if (argv[0] == 'e')
	{
		if (argv[1] == 'c' && check_str(argv, 2, 2, "ho"))
			return (ft_echo);
		else if (argv[1] == 'x')
		{
			if (argv[2] == 'p' && check_str(argv, 3, 3, "ort"))
				return (ft_export);
			else if (argv[2] == 'i' && check_str(argv, 3, 1, "t"))
				return (ft_exit);
		}
		else if (argv[1] == 'n' && check_str(argv, 2, 1, "v"))
			return (ft_env);
	}
	else if (argv[0] == 'p' && check_str(argv, 1, 2, "wd"))
		return (ft_pwd);
	else if (argv[0] == 'u' && check_str(argv, 1, 4, "nset"))
		return (ft_unset);
	return (NULL);
}

static t_bool	check_str(char *argv, int idx, int size, char *sep)
{
	return (ft_memcmp(argv + idx, sep, size + 1) == 0);
}

void	exec_word(t_node *node, t_context *p_ctx)
{
	char	**argv;

	argv = make_argv(node->word);
	if (ft_strchr(argv[0], '/') == NULL)
	{
		if (exec_builtin(argv, p_ctx) == FALSE)
			search_and_fork_exec(argv, p_ctx);
	}
	else if (can_access(argv[0], p_ctx))
		fork_exec(argv, p_ctx);
	else
		fork_error(p_ctx);
	free_argv(argv);
}
