/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:08:36 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**init_env(char **environ)
{
	char	**env;
	int		count;
	int		i;

	if (!environ || !environ[0])
		return (NULL);
	count = 0;
	while (environ[count])
		count++;
	env = malloc(sizeof(char *) * (count + 1));
	if (!env)
		return (NULL);
	i = 0;
	while (environ[i])
	{
		env[i] = ft_strdup(environ[i]);
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
