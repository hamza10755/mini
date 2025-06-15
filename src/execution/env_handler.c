/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:50:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 16:18:46 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	update_shlvl_env(t_env **env)
{
	char	*shlvl_str;
	char	*new_shlvl;
	int		shlvl;

	shlvl_str = get_env_value("SHLVL", *env);
	if (!shlvl_str)
	{
		set_env_value("SHLVL", "1", env);
		return;
	}
	shlvl = ft_atoi(shlvl_str);
	free(shlvl_str);
	if (shlvl < 0)
		shlvl = 0;
	new_shlvl = ft_itoa(shlvl + 1);
	if (!new_shlvl)
		return;
	set_env_value("SHLVL", new_shlvl, env);
	free(new_shlvl);
}

static int	find_shlvl_index(char **env)
{
	int	i;

	if (!env)
		return (-1);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "SHLVL=", 6) == 0)
			return (i);
		i++;
	}
	return (-1);
}

static void	update_shlvl_value(char **env, int idx)
{
	char	*shlvl_str;
	char	*new_shlvl;
	char	*new_value;
	int		shlvl;

	shlvl_str = env[idx] + 6;
	shlvl = ft_atoi(shlvl_str);
	if (shlvl < 0)
		shlvl = 0;
	new_shlvl = ft_itoa(shlvl + 1);
	if (!new_shlvl)
		return;
	new_value = ft_strjoin("SHLVL=", new_shlvl);
	free(new_shlvl);
	if (!new_value)
		return;
	env[idx] = new_value;
}

void	update_shlvl_array(char **env)
{
	int	idx;
	int	i;

	if (!env)
		return;
	idx = find_shlvl_index(env);
	if (idx >= 0)
	{
		update_shlvl_value(env, idx);
		return;
	}
	i = 0;
	while (env[i])
		i++;
	env[i] = ft_strdup("SHLVL=1");
	if (!env[i])
		return;
	env[i + 1] = NULL;
}

void	init_shlvl_env(t_env **env)
{
	char	*shlvl_str;

	shlvl_str = get_env_value("SHLVL", *env);
	if (!shlvl_str)
	{
		set_env_value("SHLVL", "1", env);
		return;
	}
	free(shlvl_str);
}

static t_env	*create_env_node(const char *env_str)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->value = ft_strdup(env_str);
	if (!node->value)
	{
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
}

static int	append_env_node(t_env **env, const char *env_str)
{
	t_env	*node;

	node = create_env_node(env_str);
	if (!node)
		return (1);
	node->next = *env;
	*env = node;
	return (0);
}

t_env	*init_env(char **envp)
{
	t_env	*env;
	int		i;

	env = NULL;
	i = 0;
	while (envp && envp[i])
	{
		if (append_env_node(&env, envp[i]))
		{
			free_env(env);
			return (NULL);
		}
		i++;
	}
	init_shlvl_env(&env);
	return (env);
}

void	free_env(t_env *env)
{
	t_env	*current;
	t_env	*next;

	current = env;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}

static int	count_env_nodes(t_env *env)
{
	int		count = 0;
	t_env	*current = env;

	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static int	fill_env_array(char **env_array, t_env *env, int count)
{
	int		i = 0;
	t_env	*current = env;

	while (i < count)
	{
		env_array[i] = ft_strdup(current->value);
		if (!env_array[i])
			return (i);
		current = current->next;
		i++;
	}
	return (i);
}

char	**convert_env_to_array(t_env *env)
{
	char	**env_array;
	int		count;
	int		filled;

	count = count_env_nodes(env);
	env_array = malloc((count + 1) * sizeof(char *));
	if (!env_array)
		return (NULL);

	filled = fill_env_array(env_array, env, count);
	if (filled != count)
	{
		free_array(env_array);
		return (NULL);
	}
	env_array[count] = NULL;
	return (env_array);
}

char	*get_env_value(const char *name, t_env *env)
{
	t_env	*current;
	size_t	name_len;

	name_len = ft_strlen(name);
	current = env;
	while (current)
	{
		if (ft_strncmp(current->value, name, name_len) == 0
			&& current->value[name_len] == '=')
			return (ft_strdup(current->value + name_len + 1));
		current = current->next;
	}
	return (NULL);
}

static char	*create_env_value(const char *name, const char *value)
{
	size_t	name_len = ft_strlen(name);
	size_t	value_len = ft_strlen(value);
	char	*new_value = malloc(name_len + value_len + 2);

	if (!new_value)
		return (NULL);
	ft_strlcpy(new_value, name, name_len + 1);
	new_value[name_len] = '=';
	ft_strlcpy(new_value + name_len + 1, value, value_len + 1);
	return (new_value);
}

static int	update_env_node(t_env *current, char *new_value)
{
	free(current->value);
	current->value = new_value;
	return (0);
}

static int	add_env_node(const char *name, char *new_value, t_env **env)
{
	(void)name;
	t_env	*new_node = malloc(sizeof(t_env));

	if (!new_node)
	{
		free(new_value);
		return (1);
	}
	new_node->value = new_value;
	new_node->next = *env;
	*env = new_node;
	return (0);
}

int	set_env_value(const char *name, const char *value, t_env **env)
{
	t_env	*current;
	char	*new_value;
	size_t	name_len = ft_strlen(name);

	new_value = create_env_value(name, value);
	if (!new_value)
		return (1);
	current = *env;
	while (current)
	{
		if (ft_strncmp(current->value, name, name_len) == 0
			&& current->value[name_len] == '=')
			return update_env_node(current, new_value);
		current = current->next;
	}
	return add_env_node(name, new_value, env);
}

void	unset_env_value(const char *name, t_env **env)
{
	t_env	*current;
	t_env	*prev;
	size_t	name_len;

	name_len = ft_strlen(name);
	current = *env;
	prev = NULL;

	while (current)
	{
		if (ft_strncmp(current->value, name, name_len) == 0
			&& current->value[name_len] == '=')
		{
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
			free(current->value);
			free(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}

int	count_env_vars(t_env *env)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

void	print_env(t_env *env, int fd_out)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strchr(current->value, '='))
		{
			ft_putstr_fd(current->value, fd_out);
			ft_putchar_fd('\n', fd_out);
		}
		current = current->next;
	}
}

void	sort_env(t_env **env)
{
	t_env	*current;
	t_env	*next;
	char	*temp;
	int		swapped;

	if (!*env || !(*env)->next)
		return;
	swapped = 1;
	while (swapped)
	{
		swapped = 0;
		current = *env;
		while (current->next)
		{
			next = current->next;
			if (ft_strncmp(current->value, next->value, 1024) > 0)
			{
				temp = current->value;
				current->value = next->value;
				next->value = temp;
				swapped = 1;
			}
			current = current->next;
		}
	}
} 
