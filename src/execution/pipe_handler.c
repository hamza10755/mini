/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 22:00:43 by hamzabillah      ###   ########.fr       */
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

static void	handle_builtin_execution(char **args, t_env **env, int *exit_status,
		t_token *tokens, char **env_array)
{
	*exit_status = handle_builtin(args, env, exit_status, STDOUT_FILENO);
	free_tokens(tokens);
	free_array(args);
	free_array(env_array);
	exit(*exit_status);
}

static void	handle_command_not_found(char **args, t_token *tokens,
		char **env_array, char *cmd_path, int is_first_child)
{
	if (is_first_child)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
	}
	free_tokens(tokens);
	free_array(args);
	free_array(env_array);
	free(cmd_path);
	exit(127);
}

static void	execute_external_command(char **args, char **env_array,
		char *cmd_path)
{
	execve(cmd_path, args, env_array);
	free(cmd_path);
	free_array(args);
	free_array(env_array);
	exit(1);
}

static void	prepare_command_execution(t_token *cmd_start, t_env *env,
		char ***args, char ***env_array)
{
	reset_signals();
	*args = convert_tokens_to_args(cmd_start);
	if (!*args)
		exit(1);
	*env_array = convert_env_to_array(env);
	if (!*env_array)
	{
		free_array(*args);
		exit(1);
	}
}

void	execute_child_command(t_token *cmd_start, t_env *env,
		int is_first_child, t_token *tokens)
{
	char	**args;
	char	*cmd_path;
	int		exit_status;
	char	**env_array;

	prepare_command_execution(cmd_start, env, &args, &env_array);
	if (is_builtin(cmd_start))
		handle_builtin_execution(args, &env, &exit_status, tokens, env_array);
	cmd_path = resolve_command_path(args[0], env_array);
	if (!cmd_path)
		handle_command_not_found(args, tokens, env_array, cmd_path,
			is_first_child);
	execute_external_command(args, env_array, cmd_path);
}

static void	handle_heredoc_redirection(t_token *current, int *prev_pipe_read)
{
	int	fd_in;
	int	fd_out;

	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (setup_redirection(current, &fd_in, &fd_out) != 0)
	{
		if (*prev_pipe_read != STDIN_FILENO)
			close(*prev_pipe_read);
		return ;
	}
	if (fd_in != STDIN_FILENO)
	{
		if (*prev_pipe_read != STDIN_FILENO)
			close(*prev_pipe_read);
		*prev_pipe_read = fd_in;
	}
}

static void	process_heredocs(t_token *tokens, int *prev_pipe_read)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_HEREDOC)
			handle_heredoc_redirection(current, prev_pipe_read);
		current = current->next;
	}
}

static void	init_pipeline_vars(int *prev_pipe_read, t_token **current,
		t_token **cmd_start, int *is_first_child, t_token *tokens)
{
	*prev_pipe_read = STDIN_FILENO;
	*current = tokens;
	*cmd_start = tokens;
	*is_first_child = 1;
}

static void	handle_pipe_error(int *pipefd, int prev_pipe_read)
{
	if (pipefd[0] != -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
	if (prev_pipe_read != STDIN_FILENO)
		close(prev_pipe_read);
}

static void	setup_pipe(t_token *current, int *pipefd, int prev_pipe_read)
{
	if (current->type == TOKEN_PIPE)
	{
		if (pipe(pipefd) == -1)
		{
			handle_pipe_error(pipefd, prev_pipe_read);
			return ;
		}
	}
}

static void	handle_child_io(int *pipefd, int prev_pipe_read, t_token *current)
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
}

static void	handle_parent_pipe(int *pipefd, int *prev_pipe_read,
		t_token **cmd_start, int *is_first_child, t_token *current)
{
	if (current->type == TOKEN_PIPE)
	{
		close(pipefd[1]);
		*prev_pipe_read = pipefd[0];
		*cmd_start = current->next;
		*is_first_child = 0;
	}
}

static void	process_pipe_command(t_token *current, int *pipefd,
		int prev_pipe_read, t_token *cmd_start, t_env **env, int is_first_child,
		t_token *tokens)
{
	pid_t	pid;

	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		handle_pipe_error(pipefd, prev_pipe_read);
		return ;
	}
	if (pid == 0)
	{
		handle_child_io(pipefd, prev_pipe_read, current);
		execute_child_command(cmd_start, *env, is_first_child, tokens);
	}
}

static void	handle_exit_status(int *exit_status, int status)
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

static void	wait_for_children(int *exit_status)
{
	int	status;

	while (waitpid(-1, &status, 0) > 0)
		handle_exit_status(exit_status, status);
}

static void	process_pipeline_commands(t_token *current, int *pipefd,
		int *prev_pipe_read, t_token **cmd_start, int *is_first_child,
		t_env **env, t_token *tokens)
{
	while (current)
	{
		if (current->type == TOKEN_PIPE || !current->next)
		{
			if (current->type == TOKEN_PIPE)
				current->value = NULL;
			setup_pipe(current, pipefd, *prev_pipe_read);
			process_pipe_command(current, pipefd, *prev_pipe_read, *cmd_start,
				env, *is_first_child, tokens);
			if (*prev_pipe_read != STDIN_FILENO)
				close(*prev_pipe_read);
			handle_parent_pipe(pipefd, prev_pipe_read, cmd_start,
				is_first_child, current);
		}
		current = current->next;
	}
}

int	execute_pipeline(t_token *tokens, t_env **env, int *exit_status)
{
	int		pipefd[2];
	int		prev_pipe_read;
	t_token	*current;
	t_token	*cmd_start;
	int		is_first_child;

	init_pipeline_vars(&prev_pipe_read, &current, &cmd_start, &is_first_child,
		tokens);
	process_heredocs(tokens, &prev_pipe_read);
	current = tokens;
	process_pipeline_commands(current, pipefd, &prev_pipe_read, &cmd_start,
		&is_first_child, env, tokens);
	init_signals();
	wait_for_children(exit_status);
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
