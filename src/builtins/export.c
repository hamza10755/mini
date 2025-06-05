/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/05 16:36:56 by hamzabillah      ###   ########.fr       */
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

static int	is_valid_env_var(const char *var)
{
	const char	*equal_sign;
	char		*name;

	if (!var || !*var)
		return (0);
	equal_sign = ft_strchr(var, '=');
	if (!equal_sign)
		return (is_valid_var_name(var));
	name = ft_substr(var, 0, equal_sign - var);
	if (!name)
		return (0);
	if (!is_valid_var_name(name))
	{
		free(name);
		return (0);
	}
	free(name);
	return (1);
}

int	builtin_export(char **args, char ***env, int fd_out)
{
	int		i;
	int		status;
	char	**sorted_env;
	char	*equal_sign;
	char	*name;
	char	*value;

	status = 0;
	if (!args[1])
	{
		sorted_env = copy_env(*env);
		if (!sorted_env)
			return (1);
		sort_env_vars(sorted_env);
		i = 0;
		while (sorted_env[i])
		{
			ft_putstr_fd("declare -x ", fd_out);
			ft_putendl_fd(sorted_env[i], fd_out);
			i++;
		}
		free_env_array(sorted_env);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		if (!is_valid_env_var(args[i]))
		{
			ft_putstr_fd("minishell: export: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			status = 1;
		}
		else
		{
			equal_sign = ft_strchr(args[i], '=');
			if (!equal_sign)
			{
				if (add_env_var(*env, args[i], "") == NULL)
					return (1);
			}
			else
			{
				name = ft_substr(args[i], 0, equal_sign - args[i]);
				if (!name)
					return (1);
				value = ft_strdup(equal_sign + 1);
				if (!value)
				{
					free(name);
					return (1);
				}
				if (add_env_var(*env, name, value) == NULL)
				{
					free(name);
					free(value);
					return (1);
				}
				free(name);
				free(value);
			}
		}
		i++;
	}
	return (status);
}
