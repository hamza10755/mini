/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:05:41 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/05 16:29:55 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_operator(const char *input, int *i, char *buffer, size_t *j,
		t_token **tokens)
{
	char	current;
	char	next;

	current = input[*i];
	next = input[*i + 1];

	flush_buffer(buffer, j, tokens);

	if (current == '|')
	{
		(*i)++;
		add_token(tokens, "|", TOKEN_PIPE);
		return (TOKEN_PIPE);
	}
	else if (current == '<' && next == '<')
	{
		(*i) += 2;
		add_token(tokens, "<<", TOKEN_HEREDOC);
		return (TOKEN_HEREDOC);
	}
	else if (current == '>' && next == '>')
	{
		(*i) += 2;
		add_token(tokens, ">>", TOKEN_APPEND);
		return (TOKEN_APPEND);
	}
	else if (current == ';')
	{
		(*i)++;
		add_token(tokens, ";", TOKEN_SEMICOLON);
		return (TOKEN_SEMICOLON);
	}
	else if (current == '>' || current == '<')
	{
		char op[2];
		op[0] = current;
		op[1] = '\0';
		(*i)++;
		add_token(tokens, op, TOKEN_REDIR);
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

void	handle_whitespace(int *i, char *buffer, size_t *j, t_token **tokens,
		int *in_word)
{
	if (*in_word)
	{
		flush_buffer(buffer, j, tokens);
		*in_word = 0;
	}
	(*i)++;
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
