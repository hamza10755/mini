/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/12 14:45:31 by hamzabillah      ###   ########.fr       */
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

static int	handle_export_builtin(char **args, t_env **env, int fd_out)
{
	return (builtin_export(args, env, fd_out));
}

static int	handle_unset_builtin(char **args, t_env **env)
{
	return (builtin_unset(args, env));
}

int	handle_builtin(char **args, t_env **env, int *exit_status, int fd_out)
{
	if (!args || !args[0])
		return (1);
	if (ft_strncmp(args[0], "echo", 5) == 0)
		*exit_status = builtin_echo(args, fd_out);
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		*exit_status = builtin_cd(args, *env);
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		*exit_status = builtin_pwd(fd_out);
	else if (ft_strncmp(args[0], "export", 7) == 0)
		*exit_status = handle_export_builtin(args, env, fd_out);
	else if (ft_strncmp(args[0], "unset", 6) == 0)
		*exit_status = handle_unset_builtin(args, env);
	else if (ft_strncmp(args[0], "env", 4) == 0)
		*exit_status = builtin_env(*env, fd_out);
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		*exit_status = builtin_exit(args);
	return (0);
}

int	execute_builtin(t_token *tokens, t_env **env, int *exit_status)
{
	char	**args;
	int		fd_out;

	fd_out = STDOUT_FILENO;
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	handle_builtin(args, env, exit_status, fd_out);
	free_array(args);
	return (0);
}
