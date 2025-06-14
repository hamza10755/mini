/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:50:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 00:39:01 by hamzabillah      ###   ########.fr       */
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

void	update_shlvl_array(char **env)
{
	char	*shlvl_str;
	char	*new_shlvl;
	int		shlvl;
	int		i;
	char	*new_value;

	if (!env)
		return;
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "SHLVL=", 6) == 0)
		{
			shlvl_str = env[i] + 6;
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
			env[i] = new_value;
			return;
		}
		i++;
	}
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

t_env	*init_env(char **envp)
{
	t_env	*env;
	t_env	*current;
	int		i;

	env = NULL;
	i = 0;
	while (envp && envp[i])
	{
		current = malloc(sizeof(t_env));
		if (!current)
		{
			free_env(env);
			return (NULL);
		}
		current->value = ft_strdup(envp[i]);
		if (!current->value)
		{
			free(current);
			free_env(env);
			return (NULL);
		}
		current->next = env;
		env = current;
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

char	**convert_env_to_array(t_env *env)
{
	char	**env_array;
	int		count;
	t_env	*current;
	int		i;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}

	env_array = malloc((count + 1) * sizeof(char *));
	if (!env_array)
		return (NULL);

	i = 0;
	current = env;
	while (i < count)
	{
		env_array[i] = ft_strdup(current->value);
		if (!env_array[i])
		{
			free_array(env_array);
			return (NULL);
		}
		current = current->next;
		i++;
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

int	set_env_value(const char *name, const char *value, t_env **env)
{
	t_env	*current;
	char	*new_value;
	size_t	name_len;

	name_len = ft_strlen(name);
	new_value = malloc(name_len + ft_strlen(value) + 2);
	if (!new_value)
		return (1);

	ft_strlcpy(new_value, name, name_len + 1);
	new_value[name_len] = '=';
	ft_strlcpy(new_value + name_len + 1, value, ft_strlen(value) + 1);

	current = *env;
	while (current)
	{
		if (ft_strncmp(current->value, name, name_len) == 0
			&& current->value[name_len] == '=')
		{
			free(current->value);
			current->value = new_value;
			return (0);
		}
		current = current->next;
	}

	current = malloc(sizeof(t_env));
	if (!current)
	{
		free(new_value);
		return (1);
	}
	current->value = new_value;
	current->next = *env;
	*env = current;
	return (0);
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
