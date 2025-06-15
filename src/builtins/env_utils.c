/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/16 00:02:54 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_env_value_from_array(const char *name, char **env)
{
	int		i;
	int		len;
	char	*value;

	if (!name || !env)
		return (NULL);
	len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
		{
			value = ft_strdup(env[i] + len + 1);
			return (value);
		}
		i++;
	}
	return (NULL);
}

int	count_env_vars_array(char **env)
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
	count = count_env_vars_array(env);
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

static char	*create_env_var(const char *name, const char *value)
{
	char	*temp;
	char	*new_var;

	temp = ft_strjoin(name, "=");
	if (!temp)
		return (NULL);
	new_var = ft_strjoin(temp, value);
	free(temp);
	return (new_var);
}

static char	**copy_env_to_new(char **env, int count)
{
	char	**new_env;
	int		i;

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
	return (new_env);
}

char	**add_env_var(char **env, const char *name, const char *value)
{
	char	**new_env;
	char	*new_var;
	int		count;
	int		i;

	count = count_env_vars_array(env);
	new_env = copy_env_to_new(env, count);
	if (!new_env)
		return (NULL);
	i = count;
	new_var = create_env_var(name, value);
	if (!new_var)
	{
		free_env_array(new_env);
		return (NULL);
	}
	new_env[i] = new_var;
	new_env[i + 1] = NULL;
	return (new_env);
}

static char	*make_env_assignment(const char *name, const char *value)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, value);
	free(tmp);
	return (result);
}

static char	**update_existing_env_var(char **env, const char *name,
		const char *value, int len, int count)
{
	char	**new_env;
	int		i;

	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
			new_env[i] = make_env_assignment(name, value);
		else
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
	new_env[i] = NULL;
	return (new_env);
}

static char	**alloc_and_copy_env(char **env, int count)
{
	char	**new_env;
	int		i;

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
	return (new_env);
}

static char	**add_new_env_var(char **env, const char *name, const char *value,
		int count)
{
	char	**new_env;
	int		i;

	new_env = alloc_and_copy_env(env, count);
	if (!new_env)
		return (NULL);
	i = count;
	new_env[i] = make_env_assignment(name, value);
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

char	**update_env_var(char **env, const char *name, const char *value)
{
	int	i;
	int	len;
	int	count;

	if (!env || !name || !value)
		return (NULL);
	len = ft_strlen(name);
	count = count_env_vars_array(env);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
			return (update_existing_env_var(env, name, value, len, count));
		i++;
	}
	return (add_new_env_var(env, name, value, count));
}

int	is_env_var_match(const char *env_entry, const char *name, int name_len)
{
	return (ft_strncmp(env_entry, name, name_len) == 0
		&& env_entry[name_len] == '=');
}

static char	**copy_env_excluding_var(char **env, const char *name, int name_len,
		int count)
{
	char	**new_env;
	int		i;
	int		j;

	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
		return (NULL);
	i = 0;
	j = 0;
	while (env[i])
	{
		if (!is_env_var_match(env[i], name, name_len))
		{
			new_env[j] = ft_strdup(env[i]);
			if (!new_env[j])
			{
				free_env_array(new_env);
				return (NULL);
			}
			j++;
		}
		i++;
	}
	new_env[j] = NULL;
	return (new_env);
}

char	**remove_env_var(char **env, const char *name)
{
	int		name_len;
	int		count;
	char	**new_env;

	if (!env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	count = count_env_vars_array(env);
	new_env = copy_env_excluding_var(env, name, name_len, count);
	if (!new_env)
		return (env);
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
void	cleanup_env(char **env)
{
	if (env)
		free_array(env);
}