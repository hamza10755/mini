/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/18 00:04:48 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_builtin(t_token *token)
{
	if (!token || !token->value)
		return (0);
	if (ft_strncmp(token->value, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(token->value, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(token->value, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(token->value, "export", 7) == 0)
		return (1);
	if (ft_strncmp(token->value, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(token->value, "env", 4) == 0)
		return (1);
	if (ft_strncmp(token->value, "exit", 5) == 0)
		return (1);
	return (0);
}

int	execute_builtin(t_token *tokens, char **env, int *exit_status)
{
	char	**args;
	int		status;

	if (!tokens || !tokens->value)
		return (0);
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (0);
	if (ft_strncmp(tokens->value, "echo", 5) == 0)
		status = builtin_echo(args);
	else if (ft_strncmp(tokens->value, "cd", 3) == 0)
		status = builtin_cd(args, env);
	else if (ft_strncmp(tokens->value, "pwd", 4) == 0)
		status = builtin_pwd();
	else if (ft_strncmp(tokens->value, "export", 7) == 0)
		status = builtin_export(args, &env);
	else if (ft_strncmp(tokens->value, "unset", 6) == 0)
		status = builtin_unset(args, &env);
	else if (ft_strncmp(tokens->value, "env", 4) == 0)
		status = builtin_env(env);
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
