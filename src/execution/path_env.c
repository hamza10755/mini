/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 05:00:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 21:31:59 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

char	*resolve_command_path(char *command, char **env)
{
	char	*path;
	char	*cmd_path;

	if (!command)
		return (NULL);
	if (ft_strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (ft_strdup(command));
		return (NULL);
	}
	path = get_env_value_from_array("PATH", env);
	if (!path)
		return (NULL);
	cmd_path = search_in_path(command, path);
	free(path);
	if (!cmd_path)
		return (NULL);
	return (cmd_path);
}

static char	*create_command_path(char *dir, char *command)
{
	char	*temp;
	char	*cmd_path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	cmd_path = ft_strjoin(temp, command);
	free(temp);
	return (cmd_path);
}

static char	*check_directory(char *dir, char *command, char *path_copy)
{
	char	*cmd_path;

	cmd_path = create_command_path(dir, command);
	if (!cmd_path)
	{
		free(path_copy);
		return (NULL);
	}
	if (access(cmd_path, X_OK) == 0)
	{
		free(path_copy);
		return (cmd_path);
	}
	free(cmd_path);
	return (NULL);
}

static char	*search_path_directories(char *command, char *path_copy)
{
	char	*dir;
	char	*path_ptr;
	char	*result;

	path_ptr = path_copy;
	while ((dir = ft_strtok(path_ptr, ":")))
	{
		path_ptr = NULL;
		result = check_directory(dir, command, path_copy);
		if (result)
			return (result);
	}
	return (NULL);
}

char	*search_in_path(char *command, char *path)
{
	char	*path_copy;
	char	*result;

	if (!command || !path)
		return (NULL);
	path_copy = ft_strdup(path);
	if (!path_copy)
		return (NULL);
	result = search_path_directories(command, path_copy);
	if (!result)
		free(path_copy);
	return (result);
}

static int	count_valid_args(t_token *tokens)
{
	int		count;
	t_token	*current;
	t_token	*prev;

	count = 0;
	current = tokens;
	prev = NULL;
	while (current && current->type != TOKEN_PIPE
		&& current->type != TOKEN_SEMICOLON)
	{
		if (current->type == TOKEN_WORD && !(prev && (prev->type == TOKEN_REDIR
					|| prev->type == TOKEN_APPEND
					|| prev->type == TOKEN_HEREDOC)))
			count++;
		prev = current;
		current = current->next;
	}
	return (count);
}

static int	is_valid_arg(t_token *current, t_token *prev)
{
	if (current->type != TOKEN_WORD)
		return (0);
	if (prev && (prev->type == TOKEN_REDIR || prev->type == TOKEN_APPEND
			|| prev->type == TOKEN_HEREDOC))
		return (0);
	return (1);
}

static int	fill_args_array(char **args, t_token *tokens, int count)
{
	int		i;
	t_token	*current;
	t_token	*prev;

	i = 0;
	current = tokens;
	prev = NULL;
	while (current && i < count)
	{
		if (is_valid_arg(current, prev))
		{
			args[i] = ft_strdup(current->value);
			if (!args[i])
			{
				free_array(args);
				return (0);
			}
			i++;
		}
		prev = current;
		current = current->next;
	}
	args[count] = NULL;
	return (1);
}

char	**convert_tokens_to_args(t_token *tokens)
{
	char	**args;
	int		count;

	count = count_valid_args(tokens);
	args = malloc((count + 1) * sizeof(char *));
	if (!args)
		return (NULL);
	if (!fill_args_array(args, tokens, count))
		return (NULL);
	return (args);
}
