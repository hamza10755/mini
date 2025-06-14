/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 01:05:58 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

extern int g_signal_flag;

int main(void)
{
    char        *input;
    t_token     *tokens;
    extern char **environ;
    t_env       *env;
    int         exit_status;
    int         saved_stdin;
    
    exit_status = 0;
    env = init_env(environ);
    if (!env)
        return (1);
    tokens = NULL;
    init_signals();
    saved_stdin = dup(0);
    while (1)
    {
        input = readline("minishill$ ");
        if (!input)
        {
            if (g_signal_flag == SIGINT) {
                dup2(saved_stdin, 0);
                reset_signal_status();
                continue;
            }
            break;
        }
        if (*input)
        {
            add_history(input);
            if (!ft_strncmp(input, "exit", 5))
            {
                free(input);
                free_env(env);
                env = NULL;
                exit(exit_status);
            }
            tokens = tokenize(input, convert_env_to_array(env), &exit_status);
            if (g_signal_flag == SIGINT) {
                if (tokens) {
                    free_tokens(tokens);
                    tokens = NULL;
                }
                if (input)
                {
                    free(input);
                    input = NULL;
                }
                dup2(saved_stdin, 0);
                reset_signal_status();
                continue;
            }
            if (tokens)
            {
                expand_tokens(tokens, env, &exit_status);
                execute_command(tokens, &env, &exit_status);
                set_exit_status_env(exit_status, &env);
                free_tokens(tokens);
                tokens = NULL;
            }
            else
            {
                if (exit_status != 258)
                    exit_status = 2;
                set_exit_status_env(exit_status, &env);
            }
        }
        if (input) {
            free(input);
            input = NULL;
        }
        init_signals();
        reset_signal_status();
    }
    if (env) {
        free_env(env);
        env = NULL;
    }
    return (exit_status);
}
