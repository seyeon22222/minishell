#include "../include/minishell.h"
#include "../include/exec_word_util.h"
#include "../include/execute_util.h"
#include "../include/filename_expansion.h"

extern int	amb_flag;

t_list	*filename_expansion(t_list *list, t_bool glob_flag)
{
	t_list	*expanded_list;
	char	*pattern;

	pattern = concatenate(list);
	if (!pattern)
		exit(ENOMEM);
	if (glob_flag)
	{
		expanded_list = globbing(pattern);
		if (expanded_list)
		{
			free(pattern);
			return (expanded_list);
		}
	}
	return (ft_lstnew(pattern));
}

char	*concatenate(t_list *list)
{
	int		str_size;
	char	*content;
	char	*pattern;
	t_list	*head;

	head = list;
	str_size = 0;
	while (list)
	{
		str_size += ft_strlen(list->content);
		list = list->next;
	}
	pattern = ft_calloc(str_size + 1, sizeof(char));
	if (!pattern)
		return (NULL);
	list = head;
	while (list)
	{
		content = list->content;
		ft_strlcat(pattern, content, str_size + 1);
		list = list->next;
	}
	return (pattern);
}

t_list	*globbing(char *pattern)
{
	t_list			*matches;
	DIR				*dp;
	char			dirbuf[PATH_MAX];
	struct dirent	*dir;
	int				file_cnt;

	file_cnt = 0;
	matches = NULL;
	getcwd(dirbuf, PATH_MAX);
	dp = opendir(dirbuf);
	dir = readdir(dp);
	while (dir != NULL)
	{
		dir = readdir(dp);
		if (dir && dir->d_name[0] == '.')
			dir->d_type = DT_UNKNOWN;
		if (dir && (dir->d_type == DT_REG || dir->d_type == DT_DIR) \
			&& is_match(pattern, dir->d_name, 0, 0))
		{
			ft_lstadd_back(&matches, ft_lstnew(ft_strdup(dir->d_name)));
			file_cnt++;
		}
	}
	closedir(dp);
	if (*get_redirect_ambiguity() == TRUE && file_cnt > 1)
	{
		printf("minishell: %s: ambiguous redirect\n", pattern);
		set_redirect_ambiguity(FALSE);
		return (NULL);
	}
	else
		return (matches);
}

int	**allocate_dp(int row, int col)
{
	int	**dp;
	int	idx;

	dp = ft_calloc(row + 1, sizeof(int *));
	if (!dp)
		exit(ENOMEM);
	idx = 0;
	while (idx <= row)
	{
		dp[idx] = ft_calloc(col + 1, sizeof(int));
		if (!(dp[idx]))
			exit(ENOMEM);
		idx++;
	}
	return (dp);
}

void free_dp(int **dp, int row_size)
{
	int i;

	i = 0;
	while (i <= row_size)
	{
		free(dp[i++]);
	}
	free(dp);
}

int	is_match(char *pattern, char *word, int p_idx, int w_idx)
{
	int	len_p;
	int	len_w;
	int	**dp;
	int	pos;
	int match_flag;

	len_p = ft_strlen(pattern);
	len_w = ft_strlen(word);
	dp = allocate_dp(len_p, len_w);
	dp[0][0] = 1;
	pos = 0;
	while (pattern[pos] == '*')
	{
		dp[pos + 1][0] = dp[pos][0];
		pos++;
	}
	while (++p_idx <= len_p)
	{
		w_idx = 0;
		while (++w_idx <= len_w)
		{
			if (pattern[p_idx - 1] == '*')
				dp[p_idx][w_idx] = \
				(dp[p_idx - 1][w_idx] || dp[p_idx][w_idx - 1]);
			else if (pattern[p_idx - 1] == '?' \
			|| (pattern[p_idx - 1] == word[w_idx - 1]))
				dp[p_idx][w_idx] = dp[p_idx - 1][w_idx - 1];
		}
	}
	match_flag = dp[len_p][len_w];
	free_dp(dp, len_p);
	return (match_flag);
}

void	msh_error(char *name, char *msg, int error)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (msg == NULL && error == 0)
	{
		perror(name);
		return ;
	}
	if (name)
	{
		ft_putstr_fd(name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (msg)
		ft_putendl_fd(msg, STDERR_FILENO);
	else if (error)
		ft_putendl_fd(strerror(error), STDERR_FILENO);
}
