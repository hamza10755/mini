/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:50:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:57:05 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int get_exit_status(int exit_status)
{
	return exit_status;
}

void update_exit_status(int wait_status, int *exit_status)
{
	if (WIFEXITED(wait_status))
		*exit_status = WEXITSTATUS(wait_status);
	else if (WIFSIGNALED(wait_status))
		*exit_status = 128 + WTERMSIG(wait_status);
}

void set_exit_status_env(int exit_status, char ***env)
{
	char *status_str;
	char **new_env;

	status_str = ft_itoa(exit_status);
	if (!status_str)
		return;
	
	new_env = update_env_var(*env, "?", status_str);
	free(status_str);
	if (new_env)
	{
		free_env_array(*env);
		*env = new_env;
	}
} 
