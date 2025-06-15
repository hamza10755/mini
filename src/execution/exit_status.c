/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:50:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/12 14:45:44 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	get_exit_status(int exit_status)
{
	return (exit_status);
}

void	update_exit_status(int wait_status, int *exit_status)
{
	if (WIFEXITED(wait_status))
		*exit_status = WEXITSTATUS(wait_status);
	else if (WIFSIGNALED(wait_status))
		*exit_status = 128 + WTERMSIG(wait_status);
}

void	set_exit_status_env(int exit_status, t_env **env)
{
	char	*status_str;

	status_str = ft_itoa(exit_status);
	if (!status_str)
		return ;
	set_env_value("?", status_str, env);
	free(status_str);
}
