/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:39:31 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	count_env(char **environ)
{
	int	count;

	count = 0;
	while (environ && environ[count])
		count++;
	return (count);
}

static int	handle_shlvl(char **env, char *environ_entry, int i)
{
	int		shlvl_value;
	char	*shlvl;

	shlvl_value = ft_atoi(environ_entry + 6) + 1;
	shlvl = ft_itoa(shlvl_value);
	if (!shlvl)
		return (0);
	env[i] = ft_strjoin("SHLVL=", shlvl);
	free(shlvl);
	if (!env[i])
		return (0);
	return (1);
}

static int	copy_env_entry(char **env, char *environ_entry, int i)
{
	env[i] = ft_strdup(environ_entry);
	if (!env[i])
		return (0);
	return (1);
}

static int	add_shlvl_if_missing(char **env, int *i)
{
	if (!copy_env_entry(env, "SHLVL=1", (*i)++))
		return (0);
	return (1);
}

static int	fill_env(char **env, char **environ)
{
	int	i;
	int	shlvl_found;

	i = 0;
	shlvl_found = 0;
	while (environ[i])
	{
		if (ft_strncmp(environ[i], "SHLVL=", 6) == 0 && !shlvl_found)
		{
			if (!handle_shlvl(env, environ[i], i))
				return (0);
			shlvl_found = 1;
		}
		else if (ft_strncmp(environ[i], "SHLVL=", 6) != 0)
		{
			if (!copy_env_entry(env, environ[i], i))
				return (0);
		}
		i++;
	}
	if (!shlvl_found)
		if (!add_shlvl_if_missing(env, &i))
			return (0);
	env[i] = NULL;
	return (1);
}

char	**init_env(char **environ)
{
	char	**env;
	int		count;

	if (!environ || !environ[0])
		return (NULL);
	count = count_env(environ);
	env = malloc(sizeof(char *) * (count + 2));
	if (!env)
		return (NULL);
	if (!fill_env(env, environ))
	{
		free_env_array(env);
		return (NULL);
	}
	return (env);
}

void	cleanup_env(char **env)
{
	if (env)
		free_env_array(env);
}
