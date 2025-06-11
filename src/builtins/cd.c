/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:36:08 by hbelaih          ###   ########.fr       */
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

static int	change_directory(char *path, char **env)
{
	char	cwd[PATH_MAX];

	if (!getcwd(cwd, PATH_MAX))
	{
		ft_putstr_fd("minishell: cd: error getting current directory\n", 2);
		return (1);
	}
	if (chdir(path) == -1)
	{
		ft_putstr_fd((char *)path, 2);
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

static char	*resolve_cd_path(char **args, char **env)
{
	char	*path;

	if (!args[1] || ft_strncmp(args[1], "~", 2) == 0)
	{
		path = get_home_dir(env);
		if (!path)
			return (NULL);
	}
	else if (ft_strncmp(args[1], "-", 2) == 0)
	{
		path = get_old_pwd(env);
		if (!path)
			return (NULL);
		ft_putendl_fd(path, 1);
	}
	else
		path = args[1];
	return (path);
}

int	builtin_cd(char **args, char **env)
{
	char	*path;

	path = resolve_cd_path(args, env);
	if (!path)
		return (1);
	return (change_directory(path, env));
}
