/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:55:22 by hbelaih          ###   ########.fr       */
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

int	is_valid_env_var(const char *var)
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

int	update_env_var_internal(char **env, const char *var)
{
	int		i;
	int		var_len;
	char	*equals;

	if (!env || !var)
		return (0);
	equals = ft_strchr(var, '=');
	if (!equals)
		return (0);
	var_len = equals - var;
	i = -1;
	while (env[++i])
	{
		if (ft_strncmp(env[i], var, var_len) == 0 && env[i][var_len] == '=')
		{
			free(env[i]);
			env[i] = ft_strdup(var);
			return (1);
		}
	}
	env[i] = ft_strdup(var);
	if (!env[i])
		return (0);
	env[i + 1] = NULL;
	return (1);
}

static char	**copy_env1(char **env)
{
	int		count = 0;
	char	**new_env;
	int		i = 0;

	while (env[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return (NULL);
	while (i < count)
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			free_env_array(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[count] = NULL;
	return (new_env);
}

static int	process_export_args(char **args, char **new_env)
{
	int	i = 1;
	int	valid;

	while (args[i])
	{
		valid = is_valid_env_var(args[i]);
		if (valid)
		{
			if (!update_env_var_internal(new_env, args[i]))
				return (0);
		}
		else
		{
			fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", args[i]);
			return (0);
		}
		i++;
	}
	return (1);
}

char	**builtin_export(char **args, char ***env, int fd_out)
{
	char	**new_env;

	if (!args || !env || !*env)
		return (NULL);
	if (!args[1])
	{
		print_sorted_env(*env, fd_out);
		return (*env);
	}
	new_env = copy_env1(*env);
	if (!new_env)
		return (NULL);
	if (!process_export_args(args, new_env))
	{
		free_env_array(new_env);
		return (NULL);
	}
	return (new_env);
}

void print_sorted_env(char **env, int fd_out)
{
	int i;
	int j;
	char *temp;

	if (!env)
		return;
	i = 0;
	while (env[i])
	{
		j = i + 1;
		while (env[j])
		{
			if (ft_strncmp(env[i], env[j], ft_strlen(env[i]) + 1) > 0)
			{
				temp = env[i];
				env[i] = env[j];
				env[j] = temp;
			}
			j++;
		}
		ft_putendl_fd(env[i], fd_out);
		i++;
	}
}
