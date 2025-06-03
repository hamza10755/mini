/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:14:32 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/18 00:02:45 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int main(void)
{
    char        *input;
    t_token     *tokens;
    extern char **environ;
    char        **env;
    int         exit_status = 0;

    // Initialize our own copy of the environment
    env = init_env(environ);
    if (!env)
        return (1);

    tokens = NULL;
    while (1)
    {
        fflush(stdout);
        input = readline("minishill$ ");
        if (!input)
            break;
        if (*input)
        {
            add_history(input);
            if (!ft_strncmp(input, "exit", 5))
            {
                free(input);
                free_env_array(env);
                exit(exit_status);
            }
            tokens = tokenize(input, env);
            if (tokens)
            {
                // Expand tokens
                expand_tokens(tokens, env, &exit_status);

                // Execute command
                execute_command(tokens, env, &exit_status);

                // Clean up
                free_tokens(tokens);
            }
        }
        free(input);
    }
    free_tokens(tokens);
    free_env_array(env);
    return (exit_status);
}
