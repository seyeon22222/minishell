#include <iostream>
#include <cstring>
using namespace std;

#define FALSE 0

// int **allocate_dp(int row, int col) 
// {
// 	int **dp;
// 	dp = calloc(row + 1, sizeof(int *));
// 	for (int i = 0; i <= row; i++)
// 	{
// 		dp[i] = calloc(col + 1, sizeof(int));
// 	}
// 	return (dp);
// }

int wildcard2(char *pattern, char *word) 
{
	int	len_p;
	int	len_w;
	int	p_idx;
	int	w_idx;

	len_p = strlen(pattern);
	len_w = strlen(word);
	// dp = allocate_dp(len_p, len_w);
	bool dp[len_p + 1][len_w + 1];
	memset(dp, false, sizeof(dp));
	
	dp[0][0] = true;
	if (pattern[0] == '*')
		dp[1][0] = true;
	p_idx = 0;
	while (++p_idx <= len_p)
	{
		w_idx = 0;
		while (++w_idx <= len_w)
		{
			if (pattern[p_idx - 1] == word[w_idx - 1])
				dp[p_idx][w_idx] = dp[p_idx - 1][w_idx - 1];
			else if (pattern[p_idx - 1] == '*')
				dp[p_idx][w_idx] = dp[p_idx - 1][w_idx] || dp[p_idx][w_idx - 1];
		}
		// if (dp[1][1] == 0)
		// 	return (FALSE);
	}
	return (dp[len_p][len_w]);
}

int main()
{
	char pattern[200];
	char word[200];
	int num;

	cin >> pattern;
	cin >> num;
	for (int i = 0; i < num; i++)
	{
		cin >> word;
		if (wildcard2(pattern, word))
			cout << word << '\n';
	}
}

// true
// ***abc
// 1
// _abc

// true
// **abc
// 1
// abc

// false
// ***abcd
// 1
// abcd
