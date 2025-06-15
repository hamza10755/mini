#include "../../includes/minishell.h"
#include <signal.h>
#include <unistd.h>

int		g_signal_flag = 0;

void	handle_sigint(int sig)
{
	g_signal_flag = sig;
}

void	reset_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

void	close_heredoc(int signum)
{
	g_signal_flag = signum;
}

void	reset_signals_heredoc(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(struct sigaction));
	sa.sa_handler = &close_heredoc;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
}

void	signals_setup(int mode)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	if (!mode)
		sa.sa_handler = handle_sigint;
	else
		sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	init_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	reset_signal_status(void)
{
	g_signal_flag = 0;
}