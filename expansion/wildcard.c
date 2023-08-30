#include "../include/expansion.h"

static int	**allocate_dp(int row, int col);
static void	free_dp(int **dp, int row_size);
static void	_wildcard_init(char *pat, char *word, int *len_p, int *len_w);

int	wildcard(char *pat, char *word, int p_i, int w_i)
{
	int	len_p;
	int	len_w;
	int	**dp;
	int	pos;
	int	match_flag;

	// len_p = ft_strlen(pattern);
	// len_w = ft_strlen(word);
	// dp[0][0] = 1;
	// pos = 0;
	// while (pattern[pos] == '*')
	// {
	// 	dp[pos + 1][0] = dp[pos][0];
	// 	pos++;
	// }
	_wildcard_init(pat, word, &len_p, &len_w);
	dp = allocate_dp(len_p, len_w);
	pos = -1;
	while (pat[++pos] == '*')
		dp[pos + 1][0] = dp[pos][0];
	while (++p_i <= len_p)
	{
		w_i = 0;
		while (++w_i <= len_w)
		{
			if (pat[p_i - 1] == '*')
				dp[p_i][w_i] = (dp[p_i - 1][w_i] || dp[p_i][w_i - 1]);
			else if (pat[p_i - 1] == '?' || (pat[p_i - 1] == word[w_i - 1]))
				dp[p_i][w_i] = dp[p_i - 1][w_i - 1];
		}
	}
	match_flag = dp[len_p][len_w];
	free_dp(dp, len_p);
	return (match_flag);
}

static int	**allocate_dp(int row, int col)
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
	dp[0][0] = 1;
	return (dp);
}

static void	free_dp(int **dp, int row_size)
{
	int	i;

	i = 0;
	while (i <= row_size)
		free(dp[i++]);
	free(dp);
}

static void	_wildcard_init(char *pat, char *word, int *len_p, int *len_w)
{
	*len_p = ft_strlen(pat);
	*len_w = ft_strlen(word);
}
