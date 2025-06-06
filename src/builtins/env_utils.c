/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:51:05 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_env_value_from_array(const char *name, char **env)
{
	int		i;
	int		len;

	if (!name || !env)
		return (NULL);
	len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
			return (ft_strdup(env[i] + len + 1));
		i++;
	}
	return (NULL);
}

int	count_env_vars(char **env)
{
	int	count;

	if (!env || !env[0])
		return (0);

	count = 0;
	while (env[count])
		count++;
	return (count);
}

void	free_env_array(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

char	**copy_env(char **env)
{
	char	**new_env;
	int		count;
	int		i;

	if (!env || !env[0])
		return (NULL);

	count = count_env_vars(env);
	if (count == 0)
		return (NULL);

	new_env = (char **)malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);

	i = 0;
	while (env[i])
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			free_env_array(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

char	**add_env_var(char **env, const char *name, const char *value)
{
	char	**new_env;
	char	*new_var;
	char	*temp;
	int		count;
	int		i;

	count = count_env_vars(env);
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return (NULL);
	i = 0;
	while (env[i])
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			free_env_array(new_env);
			return (NULL);
		}
		i++;
	}
	temp = ft_strjoin(name, "=");
	if (!temp)
	{
		free_env_array(new_env);
		return (NULL);
	}
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
	{
		free_env_array(new_env);
		return (NULL);
	}
	new_env[i] = new_var;
	new_env[i + 1] = NULL;
	return (new_env);
}

char	**update_env_var(char **env, const char *name, const char *value)
{
	char	**new_env;
	char	*new_var;
	int		i;
	int		len;
	int		count;

	if (!env || !name || !value)
		return (NULL);
	len = ft_strlen(name);
	count = count_env_vars(env);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
		{
			new_var = ft_strjoin(name, "=");
			if (!new_var)
				return (NULL);
			new_env = malloc(sizeof(char *) * (count + 1));
			if (!new_env)
			{
				free(new_var);
				return (NULL);
			}
			i = 0;
			while (env[i])
			{
				if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
					new_env[i] = ft_strjoin(new_var, value);
				else
					new_env[i] = ft_strdup(env[i]);
				if (!new_env[i])
				{
					while (--i >= 0)
						free(new_env[i]);
					free(new_env);
					free(new_var);
					return (NULL);
				}
				i++;
			}
			new_env[i] = NULL;
			free(new_var);
			return (new_env);
		}
		i++;
	}
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return (NULL);
	i = 0;
	while (env[i])
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			while (--i >= 0)
				free(new_env[i]);
			free(new_env);
			return (NULL);
		}
		i++;
	}
	new_var = ft_strjoin(name, "=");
	if (!new_var)
	{
		while (--i >= 0)
			free(new_env[i]);
		free(new_env);
		return (NULL);
	}
	new_env[i] = ft_strjoin(new_var, value);
	free(new_var);
	if (!new_env[i])
	{
		while (--i >= 0)
			free(new_env[i]);
		free(new_env);
		return (NULL);
	}
	new_env[i + 1] = NULL;
	return (new_env);
}

char	**remove_env_var(char **env, const char *name)
{
	int		i;
	int		j;
	int		name_len;
	char	**new_env;
	int		count;

	if (!env || !name)
		return (NULL);

	name_len = ft_strlen(name);
	count = count_env_vars(env);

	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
		return (env);

	j = 0;
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) != 0 || env[i][name_len] != '=')
		{
			new_env[j] = ft_strdup(env[i]);
			if (!new_env[j])
			{
				free_env_array(new_env);
				return (env);
			}
			j++;
		}
		i++;
	}
	new_env[j] = NULL;

	free_env_array(env);
	return (new_env);
}

void	sort_env_vars(char **env)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (env[i])
	{
		j = i + 1;
		while (env[j])
		{
			if (ft_strncmp(env[i], env[j], ft_strlen(env[i]) + 1) > 0)
			{
				temp = env[i];
				env[i] = env[j];
				env[j] = temp;
			}
			j++;
		}
		i++;
	}
}
