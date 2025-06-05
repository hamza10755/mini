/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 05:00:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/05 16:31:23 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*generate_temp_filename(void)
{
	char	*filename;
	char	*base;
	size_t	i;
	int		fd;

	base = "/tmp/minishell_heredoc_";
	filename = malloc(ft_strlen(base) + 7);
	if (!filename)
		return (NULL);
	ft_strlcpy(filename, base, ft_strlen(base) + 1);
	i = ft_strlen(base);
	while (i < ft_strlen(base) + 6)
	{
		filename[i] = 'a' + (rand() % 26);
		i++;
	}
	filename[i] = '\0';
	fd = open(filename, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		free(filename);
		return (NULL);
	}
	close(fd);
	return (filename);
}

static char	*read_heredoc_input(const char *delimiter)
{
	char	*line;
	char	*input;
	char	*temp;

	input = ft_strdup("");
	if (!input)
		return (NULL);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			free(input);
			return (NULL);
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break ;
		}
		temp = ft_strjoin(input, line);
		free(input);
		free(line);
		if (!temp)
			return (NULL);
		input = temp;
		temp = ft_strjoin(input, "\n");
		free(input);
		if (!temp)
			return (NULL);
		input = temp;
	}
	return (input);
}

static int	create_heredoc_file(const char *delimiter, int *fd)
{
	char	*input;
	char	*temp_file;
	int		temp_fd;

	input = read_heredoc_input(delimiter);
	if (!input)
		return (1);
	temp_file = generate_temp_filename();
	if (!temp_file)
	{
		free(input);
		return (1);
	}
	temp_fd = open(temp_file, O_RDWR);
	if (temp_fd == -1)
	{
		free(input);
		free(temp_file);
		return (1);
	}
	if (write(temp_fd, input, ft_strlen(input)) == -1)
	{
		close(temp_fd);
		unlink(temp_file);
		free(input);
		free(temp_file);
		return (1);
	}
	close(temp_fd);
	free(input);
	*fd = open(temp_file, O_RDONLY);
	unlink(temp_file);
	free(temp_file);
	if (*fd == -1)
		return (1);
	return (0);
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

int	setup_redirection(t_token *token, int *fd_in, int *fd_out)
{
	int		new_fd;
	int		flags;
	mode_t	mode;

	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if (token->type == TOKEN_REDIR)
	{
		if (token->value[0] == '>')
		{
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			new_fd = open(token->next->value, flags, mode);
			if (new_fd == -1)
			{
				perror("open");
				return (1);
			}
			if (*fd_out != STDOUT_FILENO)
				close(*fd_out);
			*fd_out = new_fd;
		}
		else if (token->value[0] == '<')
		{
			new_fd = open(token->next->value, O_RDONLY);
			if (new_fd == -1)
			{
				perror("open");
				return (1);
			}
			if (*fd_in != STDIN_FILENO)
				close(*fd_in);
			*fd_in = new_fd;
		}
	}
	else if (token->type == TOKEN_APPEND)
	{
		flags = O_WRONLY | O_CREAT | O_APPEND;
		new_fd = open(token->next->value, flags, mode);
		if (new_fd == -1)
		{
			perror("open");
			return (1);
		}
		if (*fd_out != STDOUT_FILENO)
			close(*fd_out);
		*fd_out = new_fd;
	}
	else if (token->type == TOKEN_HEREDOC)
	{
		if (create_heredoc_file(token->next->value, &new_fd) != 0)
		{
			ft_putstr_fd("minishell: heredoc failed\n", STDERR_FILENO);
			return (1);
		}
		if (*fd_in != STDIN_FILENO)
			close(*fd_in);
		*fd_in = new_fd;
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
