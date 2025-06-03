/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:03:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 22:12:11 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

// Global variable for signal handling (only allowed global)
static int	g_signal_status = 0;

// Signal handler for SIGINT (Ctrl+C)
static void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

// Initialize signal handlers
void	init_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

// Reset signal handlers to default
void	reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

// Get the last signal status
int	get_signal_status(void)
{
	return (g_signal_status);
}

// Reset signal status
void	reset_signal_status(void)
{
	g_signal_status = 0;
} 