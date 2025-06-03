/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:55:11 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_home_dir(char **env)
{
	char	*home;

	home = get_env_value_from_array("HOME", env);
	if (!home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		return (NULL);
	}
	return (home);
}

static char	*get_old_pwd(char **env)
{
	char	*old_pwd;

	old_pwd = get_env_value_from_array("OLDPWD", env);
	if (!old_pwd)
	{
		ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
		return (NULL);
	}
	return (old_pwd);
}

int	builtin_cd(char **args, char **env)
{
	char	*path;
	char	cwd[PATH_MAX];

	if (!args[1] || ft_strncmp(args[1], "~", 2) == 0)
	{
		path = get_home_dir(env);
		if (!path)
			return (1);
	}
	else if (ft_strncmp(args[1], "-", 2) == 0)
	{
		path = get_old_pwd(env);
		if (!path)
			return (1);
		ft_putendl_fd(path, 1);
	}
	else
		path = args[1];
	if (!getcwd(cwd, PATH_MAX))
	{
		ft_putstr_fd("minishell: cd: error getting current directory\n", 2);
		return (1);
	}
	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (1);
	}
	env = update_env_var(env, "OLDPWD", cwd);
	if (!getcwd(cwd, PATH_MAX))
	{
		ft_putstr_fd("minishell: cd: error getting current directory\n", 2);
		return (1);
	}
	env = update_env_var(env, "PWD", cwd);
	return (0);
}