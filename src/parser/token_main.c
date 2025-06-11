/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:06:52 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:28:36 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_word(const char *input, int *i, char *buffer, size_t *j,
		int *in_word, char **env)
{
	*in_word = 1;
	if (input[*i] == '$')
	{
		append_char(buffer, j, input[*i]);
		(*i)++;
		if (input[*i] == '?')
		{
			append_char(buffer, j, input[*i]);
			(*i)++;
		}
		else
		{
			while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
			{
				append_char(buffer, j, input[*i]);
				(*i)++;
			}
		}
	}
	else if (input[*i] == '~')
	{
		append_char(buffer, j, input[*i]);
		(*i)++;
	}
	else
	{
		append_char(buffer, j, input[*i]);
		(*i)++;
	}
	(void)env;
}

int	process_current_char(const char *input, int *i, char *buffer,
        t_token **tokens, size_t *j, int *in_word, char **env)
{
    if (is_whitespace(input[*i]))
    {
        return (handle_whitespace(i, buffer, j, tokens, in_word));
    }
    else if (is_operator(input[*i]))
    {
        return (handle_operator(input, i, buffer, j, tokens));
    }
    else if (input[*i] == '\'' || input[*i] == '"')
    {
        return (handle_quotes(input, i, buffer, j));
    }
    else
    {
        handle_word(input, i, buffer, j, in_word, env);
        return (1);
    }
}

t_token *process_input(const char *input, char *buffer, char **env)
{
    t_token *tokens;
    size_t  j;
    int     i;
    int     in_word;
    
    tokens = init_process_vars(&j, &i, &in_word);
    while (input[i])
    {
        if (!process_current_char(input, &i, buffer, &tokens, &j, &in_word, env))
        {
            if (tokens)
                free_tokens(tokens);
            return (NULL);
        }
    }
    if (j > 0)
    {
        buffer[j] = '\0';
        if (!add_token(&tokens, buffer, TOKEN_WORD))
        {
            if (tokens)
                free_tokens(tokens);
            return (NULL);
        }
    }
    return (tokens);
}

t_token	*init_process_vars(size_t *j, int *i, int *in_word)
{
	*j = 0;
	*i = 0;
	*in_word = 0;
	return (NULL);
}

t_token	*tokenize(const char *input, char **env)
{
	char	buffer[1024];
	t_token	*tokens;

	if (!input)
		return (NULL);
	tokens = process_input(input, buffer, env);
	return (tokens);
}
