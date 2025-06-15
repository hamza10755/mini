/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:05:41 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:38:54 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	check_pipe_error(t_token *prev)
{
	if (prev->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

static int	check_redir_error(t_token *prev, char current, char next)
{
	if (!prev)
		return (1);
	if (prev->type == TOKEN_REDIR || prev->type == TOKEN_APPEND
		|| prev->type == TOKEN_HEREDOC)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
		if (current == '>' && next == '>')
			ft_putstr_fd(">>'\n", 2);
		else if (current == '<' && next == '<')
			ft_putstr_fd("<<'\n", 2);
		else
			ft_putstr_fd(">'\n", 2);
		return (0);
	}
	return (1);
}

static int	handle_pipe(t_token **tokens, int *i)
{
	t_token	*res;

	(*i)++;
	if (!tokens)
		return (TOKEN_PIPE);
	res = add_token(tokens, "|", TOKEN_PIPE);
	if (!res)
		return (0);
	return (TOKEN_PIPE);
}

static int	handle_heredoc(t_token **tokens, int *i)
{
	t_token	*res;

	(*i) += 2;
	if (!tokens)
		return (TOKEN_HEREDOC);
	res = add_token(tokens, "<<", TOKEN_HEREDOC);
	if (!res)
		return (0);
	return (TOKEN_HEREDOC);
}

static int	handle_append(t_token **tokens, int *i)
{
	t_token	*res;

	(*i) += 2;
	if (!tokens)
		return (TOKEN_APPEND);
	res = add_token(tokens, ">>", TOKEN_APPEND);
	if (!res)
		return (0);
	return (TOKEN_APPEND);
}

static int	handle_semicolon(t_token **tokens, int *i)
{
	t_token	*res;

	(*i)++;
	if (!tokens)
		return (TOKEN_SEMICOLON);
	res = add_token(tokens, ";", TOKEN_SEMICOLON);
	if (!res)
		return (0);
	return (TOKEN_SEMICOLON);
}

static int	handle_single_redir(t_token **tokens, int *i, char current)
{
	t_token	*res;
	char	op[2];

	op[0] = current;
	op[1] = '\0';
	(*i)++;
	if (!tokens)
		return (TOKEN_REDIR);
	res = add_token(tokens, op, TOKEN_REDIR);
	if (!res)
		return (0);
	return (TOKEN_REDIR);
}

static int	check_operator_errors(t_token *prev, char current, char next)
{
	if (current == '|' && !check_pipe_error(prev))
		return (0);
	if ((current == '>' || current == '<') && next == current && prev
		&& !check_redir_error(prev, current, next))
		return (0);
	return (1);
}

static int	handle_operator_errors(const char *input, int *i, t_token **tokens)
{
	char	current;
	char	next;
	t_token	*prev;

	current = input[*i];
	next = input[*i + 1];
	if (tokens && *tokens)
	{
		prev = *tokens;
		while (prev->next)
			prev = prev->next;
		if (!check_operator_errors(prev, current, next))
			return (0);
	}
	return (1);
}

static int	process_operator(const char *input, int *i, t_token **tokens)
{
	char	current;
	char	next;

	current = input[*i];
	next = input[*i + 1];
	if (current == '|')
		return (handle_pipe(tokens, i));
	else if (current == '<' && next == '<')
		return (handle_heredoc(tokens, i));
	else if (current == '>' && next == '>')
		return (handle_append(tokens, i));
	else if (current == ';')
		return (handle_semicolon(tokens, i));
	else if (current == '>' || current == '<')
		return (handle_single_redir(tokens, i, current));
	return (0);
}

int	handle_operator(const char *input, int *i, char *buffer, size_t *j,
		t_token **tokens)
{
	if (!flush_buffer(buffer, j, tokens))
		return (0);
	if (!handle_operator_errors(input, i, tokens))
		return (0);
	return (process_operator(input, i, tokens));
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
