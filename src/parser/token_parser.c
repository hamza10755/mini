/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:05:41 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:26:46 by hbelaih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_operator(const char *input, int *i, char *buffer, size_t *j,
        t_token **tokens)
{
    char	current;
    char	next;
    t_token	*res;

    current = input[*i];
    next = input[*i + 1];

    if (!flush_buffer(buffer, j, tokens))
		return (0);

    if (current == '|')
    {
        (*i)++;
        res = add_token(tokens, "|", TOKEN_PIPE);
        if (!res)
            return (0);
        return (TOKEN_PIPE);
    }
    else if (current == '<' && next == '<')
    {
        (*i) += 2;
        res = add_token(tokens, "<<", TOKEN_HEREDOC);
        if (!res)
            return (0);
        return (TOKEN_HEREDOC);
    }
    else if (current == '>' && next == '>')
    {
        (*i) += 2;
        res = add_token(tokens, ">>", TOKEN_APPEND);
        if (!res)
            return (0);
        return (TOKEN_APPEND);
    }
    else if (current == ';')
    {
        (*i)++;
        res = add_token(tokens, ";", TOKEN_SEMICOLON);
        if (!res)
            return (0);
        return (TOKEN_SEMICOLON);
    }
    else if (current == '>' || current == '<')
    {
        char op[2];
        op[0] = current;
        op[1] = '\0';
        (*i)++;
        res = add_token(tokens, op, TOKEN_REDIR);
        if (!res)
            return (0);
        return (TOKEN_REDIR);
    }
    return (0);
}

int	process_quote_content(const char *input, int *i, char *buffer, size_t *j,
		char quote_char)
{
	(*i)++;
	append_char(buffer, j, quote_char);
	while (input[*i] && input[*i] != quote_char)
	{
		append_char(buffer, j, input[*i]);
		(*i)++;
	}
	if (input[*i] == quote_char)
	{
		append_char(buffer, j, quote_char);
		(*i)++;
		return (1);
	}
	return (0);
}

int	handle_quotes(const char *input, int *i, char *buffer, size_t *j)
{
	char	quote_char;
	int		start_pos;

	quote_char = input[*i];
	start_pos = *i;
	if (!process_quote_content(input, i, buffer, j, quote_char))
	{
		*i = start_pos;
		return (0);
	}
	return (1);
}

int	handle_whitespace(int *i, char *buffer, size_t *j, t_token **tokens,
        int *in_word)
{
    if (*in_word)
    {
        if (!flush_buffer(buffer, j, tokens))
            return (0);
        *in_word = 0;
    }
    (*i)++;
    return (1);
}

t_token	*handle_operator_in_word(char *buffer, size_t *j, t_token **tokens,
		int *in_word)
{
	if (*in_word)
	{
		flush_buffer(buffer, j, tokens);
		*in_word = 0;
	}
	return (*tokens);
}
