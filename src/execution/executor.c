/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:25:03 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	execute_command_with_path(char *cmd_path, char **args, char **env)
{
	pid_t	pid;
	int		status;
	int		exit_status = 0;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		reset_signals();
		if (execve(cmd_path, args, env) == -1)
		{
			perror("execve");
			exit(1);
		}
		exit(1); // Should never reach here
	}
	else
	{
		waitpid(pid, &status, 0);
		update_exit_status(status, &exit_status);
	}

	return (get_exit_status(exit_status));
}

int	execute_simple_command(t_token *tokens, char **env, int *exit_status)
{
	t_token	*current;
	char	*cmd_path;
	char	**args;
	int		fd_in;
	int		fd_out;
	int		result;

	current = tokens;
	if (!current)
		return (1);
	printf("DEBUG: Original command token: %s\n", current->value);
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	printf("DEBUG: Converted args[0]: %s\n", args[0]);
	printf("DEBUG: Checking if builtin...\n");
	if (is_builtin(current))
	{
		printf("DEBUG: Command is a builtin\n");
		result = execute_builtin(tokens, env, exit_status);
		free(args);
		return (result);
	}
	printf("DEBUG: Not a builtin, resolving path...\n");
	cmd_path = resolve_command_path(args[0], env);
	if (!cmd_path)
	{
		printf("DEBUG: Command path not found, checking if expanded builtin...\n");
		if (is_builtin(current))
		{
			printf("DEBUG: Command is a builtin after expansion\n");
			result = execute_builtin(tokens, env, exit_status);
			free(args);
			return (result);
		}
		printf("DEBUG: Command not found: %s\n", args[0]);
		free(args);
		return (127);
	}
	fd_in = 0;
	fd_out = 1;
	if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
	{
		free(cmd_path);
		free(args);
		return (1);
	}
	result = execute_command_with_path(cmd_path, args, env);
	free(cmd_path);
	free(args);
	restore_redirections(fd_in, fd_out);
	return (result);
}

int	execute_command(t_token *tokens, char **env, int *exit_status)
{
	int		has_pipe_token;
	int		result;

	if (!tokens)
		return (1);
	has_pipe_token = has_pipe(tokens);
	if (has_pipe_token)
	{
		result = execute_pipeline(tokens, env, exit_status);
		return (result);
	}
	else
	{
		result = execute_simple_command(tokens, env, exit_status);
		return (result);
	}
}
