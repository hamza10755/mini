/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:28:02 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <stdio.h>

int	count_pipes(t_token *tokens)
{
	int		count;
	t_token	*current;

	count = 0;
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
			count++;
		current = current->next;
	}
	return (count);
}

void	setup_pipe_ends(int pipe_fd[2], int prev_pipe_read, int is_last_cmd)
{
	if (prev_pipe_read != STDIN_FILENO)
	{
		if (dup2(prev_pipe_read, STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(prev_pipe_read);
	}

	if (!is_last_cmd)
	{
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}
}

int	execute_pipe(t_token *tokens, char **env, int *exit_status)
{
	int		pipefd[2];
	pid_t	pid;
	char	**args;
	char	*cmd_path;
	int		status;

	if (pipe(pipefd) == -1)
		return (1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			close(pipefd[1]);
			exit(1);
		}
		close(pipefd[1]);
		args = convert_tokens_to_args(tokens);
		if (!args)
			exit(1);
		cmd_path = resolve_command_path(args[0], env);
		if (!cmd_path)
		{
			free_env_array(args);
			exit(1);
		}
		execve(cmd_path, args, env);
		free(cmd_path);
		free_env_array(args);
		exit(1);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		*exit_status = WEXITSTATUS(status);
	return (0);
}

int	execute_pipeline(t_token *tokens, char **env, int *exit_status)
{
	t_token	*current;
	char	**args;
	char	*cmd_path;
	int		status;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
		{
			if (execute_pipe(current->next, env, exit_status) != 0)
				return (1);
			current = current->next;
			while (current && current->type != TOKEN_PIPE)
				current = current->next;
		}
		else
		{
			args = convert_tokens_to_args(current);
			if (!args)
				return (1);
			cmd_path = resolve_command_path(args[0], env);
			if (!cmd_path)
			{
				free_env_array(args);
				return (1);
			}
			pid_t pid = fork();
			if (pid == -1)
			{
				free(cmd_path);
				free_env_array(args);
				return (1);
			}
			if (pid == 0)
			{
				execve(cmd_path, args, env);
				free(cmd_path);
				free_env_array(args);
				exit(1);
			}
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				*exit_status = WEXITSTATUS(status);
			free(cmd_path);
			free_env_array(args);
			current = current->next;
		}
	}
	return (0);
}

int	has_pipe(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
			return (1);
		current = current->next;
	}
	return (0);
} 