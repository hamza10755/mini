/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:03:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:52:49 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	g_signal_status = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

void	init_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

void	reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

int	get_signal_status(void)
{
	return (g_signal_status);
}

void	reset_signal_status(void)
{
	g_signal_status = 0;
} 