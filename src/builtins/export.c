/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/18 00:04:56 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_var_name(const char *name)
{
	if (!name || !*name)
		return (0);
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*++name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
	}
	return (1);
}

static void	print_export(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putendl_fd(env[i], STDOUT_FILENO);
		i++;
	}
}

int	builtin_export(char **args, char ***env)
{
	int		i;
	int		error;
	char	*equal_sign;
	char	*name;
	char	*value;
	char	**new_env;

	if (!args[1])
	{
		print_export(*env);
		return (0);
	}
	error = 0;
	i = 1;
	while (args[i])
	{
		equal_sign = ft_strchr(args[i], '=');
		if (!equal_sign)
		{
			if (!is_valid_var_name(args[i]))
			{
				ft_putstr_fd("minishell: export: `", STDERR_FILENO);
				ft_putstr_fd(args[i], STDERR_FILENO);
				ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
				error = 1;
			}
			i++;
			continue;
		}
		name = ft_substr(args[i], 0, equal_sign - args[i]);
		if (!name)
			return (1);
		value = ft_strdup(equal_sign + 1);
		if (!value)
		{
			free(name);
			return (1);
		}
		if (!is_valid_var_name(name))
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(name, STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			error = 1;
		}
		else
		{
			new_env = update_env_var(*env, name, value);
			if (new_env)
			{
				*env = new_env;
			}
		}
		free(name);
		free(value);
		i++;
	}
	return (error);
}
