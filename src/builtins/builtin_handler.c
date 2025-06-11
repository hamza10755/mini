/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:34:01 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_builtin(t_token *tokens)
{
	if (!tokens || !tokens->value)
		return (0);
	if (ft_strncmp(tokens->value, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "export", 7) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "env", 4) == 0)
		return (1);
	if (ft_strncmp(tokens->value, "exit", 5) == 0)
		return (1);
	return (0);
}

static int	handle_export_builtin(char **args, char ***env, int fd_out)
{
	char	**new_env;

	new_env = builtin_export(args, env, fd_out);
	if (new_env)
	{
		free_env_array(*env);
		*env = new_env;
		return (0);
	}
	return (1);
}

static int	handle_unset_builtin(char **args, char ***env)
{
	char	**new_env;

	new_env = builtin_unset(args, env);
	if (new_env)
	{
		free_env_array(*env);
		*env = new_env;
		return (0);
	}
	return (1);
}

static int	dispatch_builtin(t_token *tokens, char **args, char ***env,
		int fd_out)
{
	if (ft_strncmp(tokens->value, "echo", 5) == 0)
		return (builtin_echo(args, fd_out));
	else if (ft_strncmp(tokens->value, "cd", 3) == 0)
		return (builtin_cd(args, *env));
	else if (ft_strncmp(tokens->value, "pwd", 4) == 0)
		return (builtin_pwd(fd_out));
	else if (ft_strncmp(tokens->value, "export", 7) == 0)
		return (handle_export_builtin(args, env, fd_out));
	else if (ft_strncmp(tokens->value, "unset", 6) == 0)
		return (handle_unset_builtin(args, env));
	else if (ft_strncmp(tokens->value, "env", 4) == 0)
		return (builtin_env(*env, fd_out));
	else if (ft_strncmp(tokens->value, "exit", 5) == 0)
		return (builtin_exit(args));
	return (-1);
}

int	execute_builtin(t_token *tokens, char **env, int *exit_status)
{
	char	**args;
	int		fd_out;
	int		ret;

	if (!tokens || !tokens->value)
		return (0);
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (0);
	fd_out = 1;
	ret = dispatch_builtin(tokens, args, &env, fd_out);
	if (ret == -1)
	{
		free_env_array(args);
		return (0);
	}
	*exit_status = ret;
	free_env_array(args);
	return (1);
}

static int	handle_builtin_dispatch(char **args, char ***env, int fd)
{
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (builtin_echo(args, fd));
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		return (builtin_cd(args, *env));
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		return (builtin_pwd(fd));
	else if (ft_strncmp(args[0], "export", 7) == 0)
	{
		*env = builtin_export(args, env, fd);
		return (0);
	}
	else if (ft_strncmp(args[0], "unset", 6) == 0)
	{
		*env = builtin_unset(args, env);
		return (0);
	}
	else if (ft_strncmp(args[0], "env", 4) == 0)
		return (builtin_env(*env, fd));
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		return (builtin_exit(args));
	return (1);
}

int	handle_builtin(char **args, char ***env, int *exit_status, int fd)
{
	int	ret;

	if (!args || !args[0])
		return (1);
	ret = handle_builtin_dispatch(args, env, fd);
	*exit_status = ret;
	return (ret);
}
