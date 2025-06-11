/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 23:40:42 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int g_signal_flag;

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

int	execute_simple_command(t_token *tokens, char ***env, int *exit_status, int in_pipeline)
{
	t_token	*current;
	char	*cmd_path;
	char	**args;
	int		fd_in;
	int		fd_out;
	int		result;
	int		saved_stdin;
	int		saved_stdout;
	int		has_command;

	current = tokens;
	if (!current)
		return (1);

	has_command = 0;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			t_token *prev = current->prev;
			if (!prev || (prev->type != TOKEN_REDIR && 
				prev->type != TOKEN_APPEND && 
				prev->type != TOKEN_HEREDOC))
			{
				has_command = 1;
				break;
			}
		}
		if (current->type == TOKEN_PIPE || current->type == TOKEN_SEMICOLON)
			break;
		current = current->next;
	}

	if (!has_command)
	{
		if (tokens->type == TOKEN_HEREDOC)
		{
			fd_in = STDIN_FILENO;
			fd_out = STDOUT_FILENO;
			if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
			{
				if (g_signal_flag == SIGINT)
					*exit_status = 1;
				return (1);
			}
			if (fd_in != STDIN_FILENO)
				close(fd_in);
			if (fd_out != STDOUT_FILENO)
				close(fd_out);
			if (g_signal_flag == SIGINT)
				*exit_status = 1;
			else
				*exit_status = 0;
			return (0);
		}
		else if (tokens->type == TOKEN_REDIR)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '>'\n", 2);
			*exit_status = 2;
			return (1);
		}
		else if (tokens->type == TOKEN_APPEND)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '>>'\n", 2);
			*exit_status = 2;
			return (1);
		}
	}

	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);

	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
	{
		free_array(args);
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
		free_array(args);
	}
	else
	{
		cmd_path = resolve_command_path(args[0], *env);
		if (!cmd_path)
		{
			if (!in_pipeline) {
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(args[0], 2);
				ft_putstr_fd(": command not found\n", 2);
			}
			free_array(args);
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
		free_array(args);
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
		result = execute_simple_command(tokens, env, exit_status, 0);
		return (result);
	}
}
