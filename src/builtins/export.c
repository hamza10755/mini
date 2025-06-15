/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:31:34 by hamzabillah      ###   ########.fr       */
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

static void	print_export_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

static int	process_quotes_in_value(char *value)
{
	char	*quote_start;
	char	*quote_end;
	char	quote_char;

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
			return (1);
		}
	}
	return (0);
}

static int	handle_spaces(char *value, t_env **env)
{
	char	*space;
	char	*next_arg;

	space = ft_strchr(value, ' ');
	if (space)
	{
		*space = '\0';
		next_arg = ft_strdup(space + 1);
		if (next_arg && is_valid_var_name(next_arg))
		{
			set_env_value(next_arg, "", env);
			free(next_arg);
		}
	}
	return (1);
}

static int	process_export_value(char *value, t_env **env)
{
	if (process_quotes_in_value(value))
		return (1);
	return (handle_spaces(value, env));
}

static int	handle_no_equal_sign(const char *arg, t_env **env)
{
	if (is_valid_var_name(arg))
	{
		if (set_env_value(arg, "", env) != 0)
			return (0);
	}
	return (1);
}

static int	allocate_name_value(const char *arg, const char *equal_sign,
		char **name, char **value)
{
	*name = ft_substr(arg, 0, equal_sign - arg);
	if (!*name)
		return (0);
	*value = ft_strdup(equal_sign + 1);
	if (!*value)
	{
		free(*name);
		return (0);
	}
	return (1);
}

static int	set_env_with_cleanup(char *name, char *value, t_env **env)
{
	if (!process_export_value(value, env))
	{
		free(name);
		free(value);
		return (0);
	}
	if (set_env_value(name, value, env) != 0)
	{
		free(name);
		free(value);
		return (0);
	}
	free(name);
	free(value);
	return (1);
}

static int	handle_with_equal_sign(const char *arg, const char *equal_sign,
		t_env **env)
{
	char	*name;
	char	*value;

	if (!allocate_name_value(arg, equal_sign, &name, &value))
		return (0);
	if (!set_env_with_cleanup(name, value, env))
		return (0);
	return (1);
}

static int	handle_export_arg(const char *arg, t_env **env)
{
	const char	*equal_sign;

	equal_sign = ft_strchr(arg, '=');
	if (equal_sign)
	{
		if (!handle_with_equal_sign(arg, equal_sign, env))
			return (0);
	}
	else
	{
		if (!handle_no_equal_sign(arg, env))
			return (0);
	}
	return (1);
}

static int	process_export_args(char **args, t_env **env)
{
	int	i;
	int	valid;

	i = 1;
	while (args[i])
	{
		valid = is_valid_env_var(args[i]);
		if (valid)
		{
			if (!handle_export_arg(args[i], env))
				return (0);
		}
		else
		{
			print_export_error(args[i]);
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
