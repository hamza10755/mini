/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/05 17:12:17 by hamzabillah      ###   ########.fr       */
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
		exit(1);
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
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	fd_in = 0;
	fd_out = 1;
	if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
	{
		free(args);
		return (1);
	}

	int saved_stdin = -1;
	int saved_stdout = -1;
	if (fd_in != STDIN_FILENO)
	{
		saved_stdin = dup(STDIN_FILENO);
		dup2(fd_in, STDIN_FILENO);
	}
	if (fd_out != STDOUT_FILENO)
	{
		saved_stdout = dup(STDOUT_FILENO);
		dup2(fd_out, STDOUT_FILENO);
	}

	if (is_builtin(current))
	{
		result = handle_builtin(args, &env, exit_status, fd_out);
		free(args);
	}
	else
	{
		cmd_path = resolve_command_path(args[0], env);
		if (!cmd_path)
		{
			fprintf(stderr, "minishill: %s: command not found\n", args[0]);
			free(args);
			if (saved_stdin != -1)
			{
				dup2(saved_stdin, STDIN_FILENO);
				close(saved_stdin);
			}
			if (saved_stdout != -1)
			{
				dup2(saved_stdout, STDOUT_FILENO);
				close(saved_stdout);
			}
			if (fd_in != STDIN_FILENO)
				close(fd_in);
			if (fd_out != STDOUT_FILENO)
				close(fd_out);
			return (127);
		}
		result = execute_command_with_path(cmd_path, args, env);
		free(cmd_path);
		free(args);
	}

	if (saved_stdin != -1) 
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin); 
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
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
