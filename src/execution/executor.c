/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/08 21:33:25 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	execute_command_with_path(char *cmd_path, char **args, char **env)
{
	pid_t	pid;
	int		status;
	int		exit_status = 0;

	signal(SIGINT, SIG_IGN);
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
		init_signals();
		update_exit_status(status, &exit_status);
	}

	return (exit_status);
}

int	execute_simple_command(t_token *tokens, char ***env, int *exit_status)
{
	t_token	*current;
	char	*cmd_path;
	char	**args;
	int		fd_in;
	int		fd_out;
	int		result;
	int		saved_stdin;
	int		saved_stdout;

	current = tokens;
	if (!current)
		return (1);
	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
	{
		free(args);
		*exit_status = 1;
		return (1);
	}

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (fd_in != STDIN_FILENO)
		dup2(fd_in, STDIN_FILENO);
	if (fd_out != STDOUT_FILENO)
		dup2(fd_out, STDOUT_FILENO);

	if (is_builtin(current))
	{
		result = handle_builtin(args, env, exit_status, fd_out);
		free(args);
	}
	else
	{
		cmd_path = resolve_command_path(args[0], *env);
		if (!cmd_path)
		{
			ft_putstr_fd("minishill: ", STDERR_FILENO);
			ft_putstr_fd(args[0], STDERR_FILENO);
			ft_putstr_fd(": command not found\n", STDERR_FILENO);
			free(args);
			dup2(saved_stdin, STDIN_FILENO);
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdin);
			close(saved_stdout);
			if (fd_in != STDIN_FILENO)
				close(fd_in);
			if (fd_out != STDOUT_FILENO)
				close(fd_out);
			*exit_status = 127;
			return (127);
		}
		result = execute_command_with_path(cmd_path, args, *env);
		free(cmd_path);
		free(args);
	}
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	*exit_status = result;
	return (result);
}

int	execute_command(t_token *tokens, char ***env, int *exit_status)
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
