/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/18 00:02:50 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**init_env(char **envp)
{
	char	**env;
	int		i;
	int		count;

	if (!envp)
		return (NULL);
	count = 0;
	while (envp[count])
		count++;
	env = (char **)malloc(sizeof(char *) * (count + 1));
	if (!env)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		env[i] = ft_strdup(envp[i]);
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