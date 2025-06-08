/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/08 22:17:35 by hamzabillah      ###   ########.fr       */
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

int	execute_builtin(t_token *tokens, char **env, int *exit_status)
{
	char	**args;
	int		status;
	int		fd_out;
	char	**new_env;

	if (!tokens || !tokens->value)
		return (0);
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (0);
	fd_out = 1; // Default to stdout
	if (ft_strncmp(tokens->value, "echo", 5) == 0)
		status = builtin_echo(args, fd_out);
	else if (ft_strncmp(tokens->value, "cd", 3) == 0)
		status = builtin_cd(args, env);
	else if (ft_strncmp(tokens->value, "pwd", 4) == 0)
		status = builtin_pwd(fd_out);
	else if (ft_strncmp(tokens->value, "export", 7) == 0)
	{
		new_env = builtin_export(args, &env, fd_out);
		if (new_env)
		{
			free_env_array(env);
			env = NULL;
			env = new_env;
		}
		status = (new_env != NULL) ? 0 : 1;
	}
	else if (ft_strncmp(tokens->value, "unset", 6) == 0)
	{
		new_env = builtin_unset(args, &env);
		if (new_env)
		{
			free_env_array(env);
			env = NULL;
			env = new_env;
		}
		status = (new_env != NULL) ? 0 : 1;
	}
	else if (ft_strncmp(tokens->value, "env", 4) == 0)
		status = builtin_env(env, fd_out);
	else if (ft_strncmp(tokens->value, "exit", 5) == 0)
		status = builtin_exit(args);
	else
	{
		free_env_array(args);
		return (0);
	}
	*exit_status = status;
	free_env_array(args);
	return (1);
}

int	handle_builtin(char **args, char ***env, int *exit_status, int fd)
{
	int	ret;

	if (!args || !args[0])
		return (1);
	if (ft_strncmp(args[0], "echo", 5) == 0)
		ret = builtin_echo(args, fd);
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		ret = builtin_cd(args, *env);
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		ret = builtin_pwd(fd);
	else if (ft_strncmp(args[0], "export", 7) == 0)
	{
		*env = builtin_export(args, env, fd);
		ret = 0;
	}
	else if (ft_strncmp(args[0], "unset", 6) == 0)
	{
		*env = builtin_unset(args, env);
		ret = 0;
	}
	else if (ft_strncmp(args[0], "env", 4) == 0)
		ret = builtin_env(*env, fd);
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		ret = builtin_exit(args);
	else
		ret = 1;
	*exit_status = ret;
	return (ret);
}
