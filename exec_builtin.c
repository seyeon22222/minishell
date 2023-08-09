#include "minishell.h"
#include "../libft/libft.h"
# define PATH_MAX 1024
// echo, cd, pwd, export, unset, env, exit
t_bool	ft_echo(t_node *node)
{
	char	**temp;
	int		idx;
	int		flag;

	temp = node->left->word.buf;
	flag = 0;
	if (!temp)
		printf("\n");
	else if (temp[1][0] == '-' && temp[1][1] == 'n')
	{
		idx = 1;
		flag = 1;
	}
	else
		idx = 0;
	while (temp[++idx])
	{
		if (temp[idx + 1] != NULL)
			printf("%s ", temp[idx]);
		else
			printf("%s", temp[idx]);
	}
	if (flag == 0)
		printf("\n");
	return (FALSE);
}

t_bool	ft_pwd(t_node *node)
{
	char	*path[PATH_MAX];

	getcwd(path, PATH_MAX);
	printf("%s\n", path);
	return (FALSE);
}

// 환경변수의 PWD에 현재 폴더의 위치가 저장되어있는데, cd 상대경로 or 절대경로일 때, 이 환경변수의 PWD를 변경해줘야함
// 마찬가지로 OLDPWD는 이전의 내용으로 이를 제거해야함
// cd와 export와 unset은 한 세트로, 비슷한 흐름으로 제어되야함 -> 환경변수를 사용하기 때문
t_bool	ft_cd(t_node *node)
{
	char	*path[PATH_MAX];

	// getcwd로 현재 폴더의 경로를 가져옴
	getcwd(path, PATH_MAX);
	// 성공 시, 리턴코드 0 반환
	return (FALSE);
}

// 등록한 환경변수는 쉘이 종료됨에 따라 같이 환경변수에서 삭제되야함
t_bool	ft_export(t_node *node, char **envp)
{
	char	**temp;
	int		idx;

	// 트리의 왼쪽가지에 2차원배열로 값이 들어가있어야함
	temp = node->left->word.buf;
	// 인자가 없으면 단순 환경변수들을 나열해줌
	if (!temp[1])
	{
		idx = 0;
		while (envp[idx])
		{
			printf("declare -x %s\n", envp[idx]);
			idx++;
		}
	}
	// 인자가 있으면 해당 인자들을 환경변수에 등록해야함
	else
	{
		
	}
	// 성공 시, 리턴코드 0 반환
	return (FALSE);
}

t_bool	ft_unset(t_node *node)
{
	char	**temp;
	int		idx;

	// 트리의 왼쪽가지에 2차원배열로 값이 들어가있어야함
	temp = node->left->word.buf;
	// 인자가 있으면 환경변수에 있는지 확인 후, 제거 / 이상한 인자가 들어오더라도 아무동작을 하지 않음
	if (temp[1])
	{

	}
	// 성공 시, 리턴코드 0 반환
	return (FALSE);
}

// 환경변수를 어디선가 끌고와야함
t_bool	ft_env(t_node *node, char **envp)
{
	int	idx;

	idx = 0;
	while (envp[idx])
	{
		printf("%s\n", envp[idx]);
		idx++;
	}
	// 성공 시, 리턴코드 0 반환
	return (FALSE);
}

t_bool	ft_exit(t_node *node)
{
	char	**temp;
	int		idx;

	temp = node->left->word.buf;
	if (!temp[1])
	{
		idx = 0;
		while (temp[1][idx])
		{
			if (ft_isdigit(temp[1][idx]) == 0)
			{
				printf("exit: %s: numeric argument required", temp[1]);
				exit(255);
			}
			idx++;
		}
		printf("exit\n");
		exit(ft_atoi(temp[1]));
	}
	else
	{
		printf("exit\n");
		exit(0);
	}
	return (TRUE);
}