/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:14:32 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:58:34 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int main(void)
{
    char        *input;
    t_token     *tokens;
    extern char **environ;
    char        **env;
    int         exit_status;
    
    exit_status = 0;
    env = init_env(environ);
    if (!env)
        return (1);

    tokens = NULL;
    init_signals();
    while (1)
    {
        fflush(stdout);
        input = readline("minishill$ ");
        if (!input)
        {
            write(1, "exit\n", 5);
            break;
        }
        if (*input)
        {
            add_history(input);
            if (!ft_strncmp(input, "exit", 5))
            {
                free(input);
                free_env_array(env);
                env = NULL;
                exit(exit_status);
            }
            tokens = tokenize(input, env);
            if (tokens)
            {
                expand_tokens(tokens, env, &exit_status);
                execute_command(tokens, &env, &exit_status);
                set_exit_status_env(exit_status, &env);
                free_tokens(tokens);
            }
            else
            {
                exit_status = 1;
                set_exit_status_env(exit_status, &env);
            }
        }
        free(input);
        init_signals();
        reset_signal_status();
    }
    free_tokens(tokens);
    free_env_array(env);
    env = NULL;
    return (exit_status);
}
