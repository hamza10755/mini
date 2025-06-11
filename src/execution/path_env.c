/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 05:00:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 11:23:10 by hbelaih          ###   ########.fr       */
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

char	*search_in_path(char *command, char *path)
{
	char	*dir;
	char	*cmd_path;
	char	*path_copy;
	char	*path_ptr;
	char	*temp;

	if (!command || !path)
		return (NULL);

	path_copy = ft_strdup(path);
	if (!path_copy)
		return (NULL);

	path_ptr = path_copy;
	while ((dir = ft_strtok(path_ptr, ":")))
	{
		path_ptr = NULL;
		temp = ft_strjoin(dir, "/");
		if (!temp)
		{
			free(path_copy);
			return (NULL);
		}
		cmd_path = ft_strjoin(temp, command);
		free(temp);
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
	}
	free(path_copy);
	return (NULL);
}

char	**convert_tokens_to_args(t_token *tokens)
{
	char	**args;
	int		count;
	int		i;
	t_token	*current;
	t_token	*prev = NULL;

	count = 0;
	current = tokens;
	while (current && current->type != TOKEN_PIPE && current->type != TOKEN_SEMICOLON)
	{
		if (current->type == TOKEN_WORD &&
			!(prev && (prev->type == TOKEN_REDIR || prev->type == TOKEN_APPEND || prev->type == TOKEN_HEREDOC)))
			count++;
		prev = current;
		current = current->next;
	}

	args = malloc((count + 1) * sizeof(char *));
	if (!args)
		return (NULL);

	i = 0;
	current = tokens;
	prev = NULL;
	while (current && i < count)
	{
		if (current->type == TOKEN_WORD &&
			!(prev && (prev->type == TOKEN_REDIR || prev->type == TOKEN_APPEND || prev->type == TOKEN_HEREDOC)))
		{
			args[i] = ft_strdup(current->value);
			if (!args[i])
			{
				free_array(args);
				return (NULL);
			}
			i++;
		}
		prev = current;
		current = current->next;
	}
	args[count] = NULL;
	return (args);
}
