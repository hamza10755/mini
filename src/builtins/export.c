/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 00:38:09 by hamzabillah      ###   ########.fr       */
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

static int	process_export_args(char **args, t_env **env)
{
	int	i;
	int	valid;
	char	*name;
	char	*value;
	const char	*equal_sign;
	char	quote_char;
	char	*quote_start;
	char	*quote_end;

	i = 1;
	while (args[i])
	{
		valid = is_valid_env_var(args[i]);
		if (valid)
		{
			equal_sign = ft_strchr(args[i], '=');
			if (equal_sign)
			{
				name = ft_substr(args[i], 0, equal_sign - args[i]);
				if (!name)
					return (0);
				
				value = ft_strdup(equal_sign + 1);
				if (!value)
				{
					free(name);
					return (0);
				}
				quote_start = ft_strchr(value, '"');
				if (!quote_start)
					quote_start = ft_strchr(value, '\'');
				
				if (quote_start)
				{
					quote_char = *quote_start;
					quote_end = ft_strrchr(value, quote_char);
					if (quote_end && quote_end > quote_start)
					{
						*quote_end = '\0';
						ft_memmove(value, quote_start + 1, ft_strlen(quote_start + 1) + 1);
					}
				}
				else
				{
					char *space = ft_strchr(value, ' ');
					if (space)
					{
						*space = '\0';
						char *next_arg = ft_strdup(space + 1);
						if (next_arg && is_valid_var_name(next_arg))
						{
							set_env_value(next_arg, "", env);
							free(next_arg);
						}
					}
				}
				if (set_env_value(name, value, env) != 0)
				{
					free(name);
					free(value);
					return (0);
				}
				free(name);
				free(value);
			}
			else if (is_valid_var_name(args[i]))
			{
				if (set_env_value(args[i], "", env) != 0)
					return (0);
			}
		}
		else
		{
			ft_putstr_fd("minishell: export: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return (0);
		}
		i++;
	}
	return (1);
}

int	builtin_export(char **args, t_env **env, int fd_out)
{
	if (!args || !env || !*env)
		return (1);
	if (!args[1])
	{
		sort_env(env);
		print_env(*env, fd_out);
		return (0);
	}
	if (!process_export_args(args, env))
		return (1);
	return (0);
}
