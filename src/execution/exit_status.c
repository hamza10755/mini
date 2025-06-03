/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:50:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:00:05 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// Get exit status
int get_exit_status(int exit_status)
{
	return exit_status;
}

// Update exit status based on wait status
void update_exit_status(int wait_status, int *exit_status)
{
	if (WIFEXITED(wait_status))
		*exit_status = WEXITSTATUS(wait_status);
	else if (WIFSIGNALED(wait_status))
		*exit_status = 128 + WTERMSIG(wait_status);
} 