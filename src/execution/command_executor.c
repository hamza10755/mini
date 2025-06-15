/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 14:50:17 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/minishell.h"

int	execute_command(t_token *tokens, t_env **env, int *exit_status)
{
	if (!tokens || !tokens->value)
		return (0);
	if (is_builtin(tokens))
		return (execute_builtin(tokens, env, exit_status));
	if (has_pipe(tokens))
		return (execute_pipeline(tokens, env, exit_status));
	return (execute_simple_command(tokens, env, exit_status));
}

int	execute_simple_command(t_token *tokens, t_env **env, int *exit_status)
{
	char	**args;
	char	*cmd_path;
	pid_t	pid;
	int		status;
	char	**env_array;

	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	env_array = convert_env_to_array(*env);
	if (!env_array)
	{
		free_array(args);
		return (1);
	}
	cmd_path = resolve_command_path(args[0], env_array);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		free_array(args);
		free_env_array(env_array);
		*exit_status = 127;
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		free(cmd_path);
		free_array(args);
		free_env_array(env_array);
		return (1);
	}
	if (pid == 0)
	{
		reset_signals();
		execve(cmd_path, args, env_array);
		free(cmd_path);
		free_array(args);
		free_env_array(env_array);
		exit(1);
	}
	free(cmd_path);
	free_array(args);
	free_env_array(env_array);
	waitpid(pid, &status, 0);
	update_exit_status(status, exit_status);
	return (0);
} 
