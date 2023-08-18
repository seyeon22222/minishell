#include "../include/minishell.h"
#include "../libft/libft.h"
#include "../include/execute.h"

t_bool	ft_cd(char **argv)
{
	char	path[PATH_MAX];
	char	*can_env;
	char	*newpwd;

	can_env = vaild_env(argv);
	if (!can_env)
	{
		write(STDERR_FILENO, "cd: Too many arguments\n", 24);
		return (1);
	}
	newpwd = getcwd(path, PATH_MAX);
	if (chdir(can_env) != 0)
	{
		write(STDERR_FILENO, "cd : No such file or directory\n", 32);
		return (1);
	}
	set_envp("OLDPWD", newpwd);
	newpwd = getcwd(path, PATH_MAX);
	set_envp("PWD", newpwd);
	return (0);
}

char	*vaild_env(char **temp)
{
	int		size;
	char	*parameter;

	size = 0;
	parameter = 0;
	while (temp[size])
		size++;
	if (size > 2)
		return (NULL);
	else if (size == 1)
		return (ft_getenv("HOME"));
	parameter = special_case(temp);
	if (!parameter)
		parameter = temp[1];
	return (parameter);
}

char	*special_case(char **temp)
{
	size_t	temp_len;
	char	*parameter;
	char	path[PATH_MAX];

	temp_len = ft_strlen(temp[1]);
	parameter = 0;
	if (temp_len == 1 && ft_strncmp(temp[1], "~", 1) == 0)
		parameter = ft_getenv("HOME");
	else if (temp_len == 1 && ft_strncmp(temp[1], ".", 1) == 0)
	{
		getcwd(path, PATH_MAX);
		return (ft_strdup(path));
	}
	else if (temp_len == 2 && ft_strncmp(temp[1], "..", 2) == 0)
		parameter = "..";
	return (parameter);
}
