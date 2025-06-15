/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 05:00:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/16 00:01:29 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_signal_flag;

static void	heredoc_child_handler(int signum)
{
	(void)signum;
	g_signal_flag = SIGINT;
	close(0);
	exit(1);
}

static char	*read_heredoc_input_fork(const char *delimiter, int write_fd)
{
	char				*line;
	char				*temp;
	struct sigaction	sa;

	line = NULL;
	temp = NULL;
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = heredoc_child_handler;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break ;
		}
		temp = ft_strjoin(line, "\n");
		write(write_fd, temp, ft_strlen(temp));
		free(line);
		free(temp);
	}
	return (NULL);
}

static int	handle_pipe_creation(int *pipefd)
{
	if (pipe(pipefd) == -1)
		return (1);
	return (0);
}

static int	handle_fork_creation(pid_t *pid, int *pipefd)
{
	*pid = fork();
	if (*pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	return (0);
}

static void	handle_child_process(const char *delimiter, int *pipefd)
{
	close(pipefd[0]);
	read_heredoc_input_fork(delimiter, pipefd[1]);
	close(pipefd[1]);
	exit(0);
}

static int	handle_parent_process(pid_t pid, int *pipefd, int *fd)
{
	int	status;

	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) || WEXITSTATUS(status) == 1)
	{
		close(pipefd[0]);
		g_signal_flag = SIGINT;
		return (1);
	}
	*fd = pipefd[0];
	return (0);
}

static int	create_heredoc_file(const char *delimiter, int *fd)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	status = handle_pipe_creation(pipefd);
	if (status != 0)
		return (status);
	status = handle_fork_creation(&pid, pipefd);
	if (status != 0)
		return (status);
	if (pid == 0)
		handle_child_process(delimiter, pipefd);
	return (handle_parent_process(pid, pipefd, fd));
}

static int	handle_output_redirection(t_token *token, int *fd_out)
{
	int	fd;

	fd = open(token->next->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (1);
	*fd_out = fd;
	return (0);
}

static int	handle_input_redirection(t_token *token, int *fd_in)
{
	int	fd;

	fd = open(token->next->value, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(token->next->value, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (1);
	}
	*fd_in = fd;
	return (0);
}

static int	handle_append_redirection(t_token *token, int *fd_out)
{
	int	fd;

	fd = open(token->next->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		return (1);
	*fd_out = fd;
	return (0);
}

static int	handle_redirection_type(t_token *token, int *fd_in, int *fd_out)
{
	int	status;

	status = 0;
	if (token->type == TOKEN_REDIR)
	{
		if (token->value[0] == '>')
			status = handle_output_redirection(token, fd_out);
		else if (token->value[0] == '<')
			status = handle_input_redirection(token, fd_in);
	}
	else if (token->type == TOKEN_APPEND)
		status = handle_append_redirection(token, fd_out);
	else if (token->type == TOKEN_HEREDOC)
		status = create_heredoc_file(token->next->value, fd_in);
	return (status);
}

int	setup_redirection(t_token *token, int *fd_in, int *fd_out)
{
	int	status;

	status = handle_redirection_type(token, fd_in, fd_out);
	return (status);
}

int	handle_redirections(t_token *tokens, int *fd_in, int *fd_out)
{
	t_token	*current;
	int		status;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_REDIR || current->type == TOKEN_APPEND
			|| current->type == TOKEN_HEREDOC)
		{
			status = setup_redirection(current, fd_in, fd_out);
			if (status != 0)
				return (status);
		}
		current = current->next;
	}
	return (0);
}

void	restore_redirections(int fd_in, int fd_out)
{
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
}
