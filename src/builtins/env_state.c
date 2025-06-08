/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/08 00:35:38 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**init_env(char **environ)
{
	char	**env;
	int		count;
	int		i;
	char	*shlvl;
	int		shlvl_value;
	int		shlvl_found;

	if (!environ || !environ[0])
		return (NULL);
	count = 0;
	while (environ[count])
		count++;
	env = malloc(sizeof(char *) * (count + 1));
	if (!env)
		return (NULL);
	i = 0;
	shlvl_found = 0;
	while (environ[i])
	{
		if (ft_strncmp(environ[i], "SHLVL=", 6) == 0 && !shlvl_found)
		{
			shlvl_value = ft_atoi(environ[i] + 6) + 1;
			shlvl = ft_itoa(shlvl_value);
			if (!shlvl)
			{
				free_env_array(env);
				return (NULL);
			}
			env[i] = ft_strjoin("SHLVL=", shlvl);
			free(shlvl);
			if (!env[i])
			{
				free_env_array(env);
				return (NULL);
			}
			shlvl_found = 1;
		}
		else if (ft_strncmp(environ[i], "SHLVL=", 6) != 0)
		{
			env[i] = ft_strdup(environ[i]);
			if (!env[i])
			{
				free_env_array(env);
				return (NULL);
			}
		}
		else
		{
			i--;
			count--;
		}
		i++;
	}
	// If SHLVL wasn't found, add it
	if (!shlvl_found)
	{
		env[i] = ft_strdup("SHLVL=1");
		if (!env[i])
		{
			free_env_array(env);
			return (NULL);
		}
		i++;
	}
	env[i] = NULL;
	return (env);
}

void	cleanup_env(char **env)
{
	if (env)
		free_env_array(env);
}
