/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/08 23:17:44 by hamzabillah      ###   ########.fr       */
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

void	execute_child_command(t_token *cmd_start, char **env, int is_first_child)
{
	char	**args;
	char	*cmd_path;
	int		exit_status;

	reset_signals();
	args = convert_tokens_to_args(cmd_start);
	if (!args)
		exit(1);

	if (is_builtin(cmd_start))
	{
		exit_status = handle_builtin(args, &env, &exit_status, STDOUT_FILENO);
		free_env_array(args);
		exit(exit_status);
	}

	cmd_path = resolve_command_path(args[0], env);
	if (!cmd_path)
	{
		if (is_first_child) {
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(args[0], 2);
			ft_putstr_fd(": command not found\n", 2);
		}
		free_env_array(args);
		exit(127);
	}

	execve(cmd_path, args, env);
	free(cmd_path);
	free_env_array(args);
	exit(1);
}

int	execute_pipeline(t_token *tokens, char ***env, int *exit_status)
{
	int		pipefd[2];
	int		prev_pipe_read;
	pid_t	pid;
	t_token	*current;
	t_token	*cmd_start;
	int		status;
	int		is_first_child;

	prev_pipe_read = STDIN_FILENO;
	current = tokens;
	cmd_start = tokens;
	is_first_child = 1;

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

			signal(SIGINT, SIG_IGN);
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

				execute_child_command(cmd_start, *env, is_first_child);
			}
			if (prev_pipe_read != STDIN_FILENO)
				close(prev_pipe_read);

			if (current->type == TOKEN_PIPE)
			{
				close(pipefd[1]);
				prev_pipe_read = pipefd[0];
				cmd_start = current->next;
				is_first_child = 0;
			}
		}
		current = current->next;
	}
	init_signals();
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFEXITED(status))
			*exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGINT)
				*exit_status = 130;
			else if (WTERMSIG(status) == SIGQUIT)
				*exit_status = 131;
		}
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
