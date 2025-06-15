/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:08:29 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

extern int	g_signal_flag;

static int	handle_signal_interrupt(char *input, t_token *tokens,
		int saved_stdin)
{
	if (tokens)
		free_tokens(tokens);
	if (input)
	{
		free(input);
		input = NULL;
	}
	dup2(saved_stdin, 0);
	reset_signal_status();
	return (1);
}

static int	handle_exit_command(char *input, t_env *env, int exit_status)
{
	free(input);
	free_env(env);
	env = NULL;
	exit(exit_status);
	return (0);
}

static void	handle_tokens(t_token *tokens, t_env **env, int *exit_status)
{
	if (tokens)
	{
		expand_tokens(tokens, *env, exit_status);
		execute_command(tokens, env, exit_status);
		set_exit_status_env(*exit_status, env);
		free_tokens(tokens);
	}
	else
	{
		if (*exit_status != 258)
			*exit_status = 2;
		set_exit_status_env(*exit_status, env);
	}
}

static void	cleanup_and_reset(char *input)
{
	if (input)
	{
		free(input);
		input = NULL;
	}
	init_signals();
	reset_signal_status();
}

static int	handle_empty_input(char *input, int saved_stdin)
{
	if (!input)
	{
		if (g_signal_flag == SIGINT)
		{
			dup2(saved_stdin, 0);
			reset_signal_status();
			return (1);
		}
		return (2);
	}
	return (0);
}

static int	handle_command_input(char *input, t_env **env, int *exit_status,
		int saved_stdin)
{
	t_token	*tokens;

	add_history(input);
	if (!ft_strncmp(input, "exit", 5))
		handle_exit_command(input, *env, *exit_status);
	tokens = tokenize(input, exit_status);
	if (g_signal_flag == SIGINT)
	{
		if (handle_signal_interrupt(input, tokens, saved_stdin))
			return (1);
	}
	handle_tokens(tokens, env, exit_status);
	return (0);
}

static void	init_shell(t_env **env, int *saved_stdin)
{
	extern char	**environ;

	*env = init_env(environ);
	if (!*env)
		exit(1);
	init_signals();
	*saved_stdin = dup(0);
}

static void	cleanup_shell(t_env *env)
{
	if (env)
	{
		free_env(env);
		env = NULL;
	}
}

static int	handle_shell_loop(t_env *env, int saved_stdin, int *exit_status)
{
	char	*input;
	int		ret;

	while (1)
	{
		input = readline("minishill$ ");
		ret = handle_empty_input(input, saved_stdin);
		if (ret == 2)
			break ;
		if (ret == 1)
			continue ;
		if (*input)
		{
			if (handle_command_input(input, &env, exit_status, saved_stdin))
				continue ;
		}
		cleanup_and_reset(input);
	}
	return (0);
}

int	main(void)
{
	t_env	*env;
	int		exit_status;
	int		saved_stdin;

	exit_status = 0;
	init_shell(&env, &saved_stdin);
	handle_shell_loop(env, saved_stdin, &exit_status);
	cleanup_shell(env);
	return (exit_status);
}
