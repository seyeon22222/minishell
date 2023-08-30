#ifndef MINISHELL2_H
# define MINISHELL2_H


# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <errno.h>
# include <stdio.h>
# include "../libft/libft.h"
# include "grammar.h"
# include "ft_signal.h"
# include "parser.h"
# include "execute.h"

# define PROC_MAX 1024
# define STDOUT 1
# define STDIN 0

void	init_envp(char **envp);
t_list	**get_envp(void);
void	print_eof_exit(void);
void	minishell_loop(void);
int		*get_last_pid(void);
void	set_last_pid(int pid);
int		*get_last_exit_status(void);
void	set_last_exit_status(int exit_status);
t_list	*_delete_process(t_list **head, t_list *d_node);
void	wait_list(t_context *p_ctx);
void	*wait_process(t_list *node, t_context *p_ctx, int flag);
void	_wait_process(t_list *ret, t_context *p_ctx, t_list *node, pid_t pid);
void	cir_lstadd(pid_t pid, t_context *p_ctx);
void	cir_lstadd_back(t_list **head, t_list *n_node);
#endif