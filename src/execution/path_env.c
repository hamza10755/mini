/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 05:00:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:47:04 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	free_array(char **array)
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
	return (cmd_path);
}

char	*search_in_path(char *command, char *path)
{
	char	*dir;
	char	*cmd_path;
	char	*path_copy;
	char	*path_ptr;
	char	*temp;

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

	// Count number of arguments
	count = 0;
	current = tokens;
	while (current && current->type != TOKEN_PIPE && current->type != TOKEN_SEMICOLON)
	{
		if (current->type == TOKEN_WORD)
			count++;
		current = current->next;
	}

	// Allocate array for arguments
	args = malloc((count + 1) * sizeof(char *));
	if (!args)
		return (NULL);

	// Copy arguments
	i = 0;
	current = tokens;
	while (i < count)
	{
		if (current->type == TOKEN_WORD)
		{
			args[i] = ft_strdup(current->value);
			if (!args[i])
			{
				free_array(args);
				return (NULL);
			}
			i++;
		}
		current = current->next;
	}
	args[count] = NULL;
	return (args);
}
