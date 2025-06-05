/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/05 17:13:29 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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

static void	execute_child_command(t_token *cmd_start, char **env)
{
	char	**args;
	char	*cmd_path;

	args = convert_tokens_to_args(cmd_start);
	if (!args)
		exit(1);

	cmd_path = resolve_command_path(args[0], env);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		free_env_array(args);
		exit(127);
	}

	execve(cmd_path, args, env);
	free(cmd_path);
	free_env_array(args);
	exit(1);
}

int	execute_pipeline(t_token *tokens, char **env, int *exit_status)
{
	int		pipefd[2];
	int		prev_pipe_read;
	pid_t	pid;
	t_token	*current;
	t_token	*cmd_start;
	int		status;

	prev_pipe_read = STDIN_FILENO;
	current = tokens;
	cmd_start = tokens;

	while (current)
	{
		if (current->type == TOKEN_PIPE || !current->next)
		{
			if (current->type == TOKEN_PIPE)
				current->value = NULL;

			if (current->type == TOKEN_PIPE)
			{
				if (pipe(pipefd) == -1)
				{
					if (prev_pipe_read != STDIN_FILENO)
						close(prev_pipe_read);
					return (1);
				}
			}

			pid = fork();
			if (pid == -1)
			{
				if (current->type == TOKEN_PIPE)
				{
					close(pipefd[0]);
					close(pipefd[1]);
				}
				if (prev_pipe_read != STDIN_FILENO)
					close(prev_pipe_read);
				return (1);
			}

			if (pid == 0)
			{
				if (prev_pipe_read != STDIN_FILENO)
				{
					if (dup2(prev_pipe_read, STDIN_FILENO) == -1)
					{
						perror("dup2 input");
						exit(1);
					}
					close(prev_pipe_read);
				}
				if (current->type == TOKEN_PIPE)
				{
					if (dup2(pipefd[1], STDOUT_FILENO) == -1)
					{
						perror("dup2 output");
						exit(1);
					}
					close(pipefd[0]);
					close(pipefd[1]);
				}

				execute_child_command(cmd_start, env);
			}
			if (prev_pipe_read != STDIN_FILENO)
				close(prev_pipe_read);

			if (current->type == TOKEN_PIPE)
			{
				close(pipefd[1]);
				prev_pipe_read = pipefd[0];
				cmd_start = current->next;
			}
		}
		current = current->next;
	}
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFEXITED(status))
			*exit_status = WEXITSTATUS(status);
	}

	if (prev_pipe_read != STDIN_FILENO)
		close(prev_pipe_read);

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
