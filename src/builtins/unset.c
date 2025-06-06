/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 20:59:39 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_var_name(const char *name)
{
	if (!name || !*name)
		return (0);
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*++name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
	}
	return (1);
}

static int	should_remove_var(char *env_var, char **args)
{
	int	i;
	int	len;

	if (!env_var || !args)
		return (0);
	i = 1;
	while (args[i])
	{
		len = ft_strlen(args[i]);
		if (ft_strncmp(env_var, args[i], len) == 0 && env_var[len] == '=')
			return (1);
		i++;
	}
	return (0);
}

char	**builtin_unset(char **args, char ***env)
{
	int		i;
	int		j;
	int		count;
	char	**new_env;
	int		new_count;

	if (!args || !env || !*env)
		return (NULL);
	i = 1;
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return (NULL);
		}
		i++;
	}
	count = 0;
	while ((*env)[count])
		count++;
	new_count = count;
	i = 0;
	while ((*env)[i])
	{
		if (should_remove_var((*env)[i], args))
			new_count--;
		i++;
	}
	new_env = malloc(sizeof(char *) * (new_count + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	j = 0;
	while ((*env)[i])
	{
		if (!should_remove_var((*env)[i], args))
		{
			new_env[j] = ft_strdup((*env)[i]);
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
