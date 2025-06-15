/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:06:52 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:17:16 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_dollar_sign(const char *input, int *i, char *buffer,
		size_t *j)
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

static void	handle_tilde(const char *input, int *i, char *buffer, size_t *j)
{
	append_char(buffer, j, input[*i]);
	(*i)++;
}

static void	handle_regular_char(const char *input, int *i, char *buffer,
		size_t *j)
{
	append_char(buffer, j, input[*i]);
	(*i)++;
}

void	handle_word(const char *input, int *i, char *buffer, size_t *j,
		int *in_word)
{
	*in_word = 1;
	if (input[*i] == '$')
		handle_dollar_sign(input, i, buffer, j);
	else if (input[*i] == '~')
		handle_tilde(input, i, buffer, j);
	else
		handle_regular_char(input, i, buffer, j);
}

static int	handle_whitespace_case(int *i, char *buffer, size_t *j,
		t_token **tokens, int *in_word)
{
	return (handle_whitespace(i, buffer, j, tokens, in_word));
}

static int	handle_operator_case(const char *input, int *i, char *buffer,
		size_t *j, t_token **tokens)
{
	return (handle_operator(input, i, buffer, j, tokens));
}

static int	handle_quote_case(const char *input, int *i, char *buffer,
		size_t *j)
{
	return (handle_quotes(input, i, buffer, j));
}

static int	handle_word_case(const char *input, int *i, char *buffer, size_t *j,
		int *in_word)
{
	handle_word(input, i, buffer, j, in_word);
	return (1);
}

int	process_current_char(const char *input, int *i, char *buffer,
		t_token **tokens, size_t *j, int *in_word, char **env)
{
	if (is_whitespace(input[*i]))
		return (handle_whitespace_case(i, buffer, j, tokens, in_word));
	else if (is_operator(input[*i]))
		return (handle_operator_case(input, i, buffer, j, tokens));
	else if (input[*i] == '\'' || input[*i] == '"')
		return (handle_quote_case(input, i, buffer, j));
	else
		return (handle_word_case(input, i, buffer, j, in_word));
	(void)env;
}

static int	handle_quotes_in_input(const char *input, int *i, char *buffer,
		size_t *j, t_token **tokens)
{
	int	quote_result;

	quote_result = handle_quotes(input, i, buffer, j);
	if (!quote_result)
	{
		if (*tokens)
			free_tokens(*tokens);
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		return (0);
	}
	return (1);
}

static int	handle_remaining_buffer(char *buffer, size_t j, t_token **tokens)
{
	buffer[j] = '\0';
	if (!add_token(tokens, buffer, TOKEN_WORD))
	{
		if (*tokens)
			free_tokens(*tokens);
		return (0);
	}
	return (1);
}

static int	process_input_loop(const char *input, int *i, char *buffer,
		size_t *j, t_token **tokens, int *in_word, char **env)
{
	while (input[*i])
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			if (!handle_quotes_in_input(input, i, buffer, j, tokens))
				return (0);
		}
		else if (!process_current_char(input, i, buffer, tokens, j, in_word,
				env))
		{
			if (*tokens)
				free_tokens(*tokens);
			return (0);
		}
	}
	return (1);
}

t_token	*process_input(const char *input, char *buffer, char **env)
{
	t_token	*tokens;
	size_t	j;
	int		i;
	int		in_word;

	tokens = init_process_vars(&j, &i, &in_word);
	if (!process_input_loop(input, &i, buffer, &j, &tokens, &in_word, env))
		return (NULL);
	if (j > 0)
	{
		if (!handle_remaining_buffer(buffer, j, &tokens))
			return (NULL);
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

static int	handle_quotes_in_word(const char *input, size_t *i, int *in_quotes,
		char *quote_char)
{
	if ((input[*i] == '"' || input[*i] == '\'') && !*in_quotes)
	{
		*in_quotes = 1;
		*quote_char = input[*i];
	}
	else if (input[*i] == *quote_char && *in_quotes)
		*in_quotes = 0;
	return (1);
}

static int	process_word_content(const char *input, size_t *i, int in_quotes,
		char quote_char, int found_equals)
{
	while (input[*i] && (!isspace(input[*i]) || in_quotes || found_equals))
	{
		if (!handle_quotes_in_word(input, i, &in_quotes, &quote_char))
			return (0);
		if (input[*i] == '=' && !in_quotes)
			found_equals = 1;
		(*i)++;
	}
	return (1);
}

static int	add_word_token(const char *input, size_t start, size_t *i,
		t_token **tokens)
{
	char	*word;

	word = ft_substr(input, start, *i - start);
	if (!word)
		return (0);
	if (!add_token(tokens, word, TOKEN_WORD))
	{
		free(word);
		return (0);
	}
	return (1);
}

static int	process_word(const char *input, size_t *i, t_token **tokens)
{
	size_t	start;
	int		in_quotes;
	char	quote_char;
	int		found_equals;

	start = *i;
	in_quotes = 0;
	quote_char = 0;
	found_equals = 0;
	if (!process_word_content(input, i, in_quotes, quote_char, found_equals))
		return (0);
	return (add_word_token(input, start, i, tokens));
}

static int	handle_input_redirection(const char *input, size_t *i,
		t_token **tokens, int *exit_status)
{
	if (input[*i + 1] == '<')
	{
		if (input[*i + 2] == '<')
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '<<<'\n",
				2);
			*exit_status = 258;
			return (0);
		}
		else
		{
			if (!add_token(tokens, "<<", TOKEN_HEREDOC))
				return (0);
			*i += 2;
		}
	}
	else
	{
		if (!add_token(tokens, "<", TOKEN_REDIR))
			return (0);
		(*i)++;
	}
	return (1);
}

static int	handle_output_redirection(const char *input, size_t *i,
		t_token **tokens, int *exit_status)
{
	if (input[*i + 1] == '>')
	{
		if (input[*i + 2] == '>')
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '>'\n",
				2);
			*exit_status = 258;
			return (0);
		}
		else
		{
			if (!add_token(tokens, ">>", TOKEN_APPEND))
				return (0);
			*i += 2;
		}
	}
	else
	{
		if (!add_token(tokens, ">", TOKEN_REDIR))
			return (0);
		(*i)++;
	}
	return (1);
}

static int	process_redirection(const char *input, size_t *i, t_token **tokens,
		int *exit_status)
{
	if (input[*i] == '<')
		return (handle_input_redirection(input, i, tokens, exit_status));
	else if (input[*i] == '>')
		return (handle_output_redirection(input, i, tokens, exit_status));
	return (1);
}

static int	check_pipe_syntax(const char *input, size_t i, t_token **tokens)
{
	t_token	*last_token;

	if (input[i] == '|')
	{
		last_token = *tokens;
		while (last_token && last_token->next)
			last_token = last_token->next;
		if (last_token && last_token->type == TOKEN_PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
				2);
			return (0);
		}
	}
	return (1);
}

static int	check_redirection_syntax(const char *input, size_t i,
		t_token **tokens)
{
	t_token	*last_token;

	if (input[i] == '>' || input[i] == '<')
	{
		last_token = *tokens;
		while (last_token && last_token->next)
			last_token = last_token->next;
		if (last_token && (last_token->type == TOKEN_REDIR
				|| last_token->type == TOKEN_APPEND
				|| last_token->type == TOKEN_HEREDOC))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
			if (input[i] == '>' && input[i + 1] == '>')
				ft_putstr_fd(">>'\n", 2);
			else
				ft_putstr_fd(">'\n", 2);
			return (0);
		}
	}
	return (1);
}

static int	check_triple_redirection(const char *input, size_t i)
{
	if (input[i] == '>' && input[i + 1] == '>' && input[i + 2] == '>')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '>>'\n", 2);
		return (0);
	}
	return (1);
}

static int	check_double_pipe(const char *input, size_t i, int *exit_status)
{
	if (input[i] == '|' && input[i + 1] == '|')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '||'\n", 2);
		*exit_status = 258;
		return (0);
	}
	return (1);
}

static int	check_syntax_error(const char *input, size_t i, t_token **tokens,
		int *exit_status)
{
	if (!check_pipe_syntax(input, i, tokens))
		return (0);
	if (!check_redirection_syntax(input, i, tokens))
		return (0);
	if (!check_triple_redirection(input, i))
		return (0);
	if (!check_double_pipe(input, i, exit_status))
		return (0);
	return (1);
}

static int	handle_pipe_token(const char *input, size_t *i, t_token **tokens,
		int *exit_status)
{
	if (!check_syntax_error(input, *i, tokens, exit_status))
		return (0);
	if (!add_token(tokens, "|", TOKEN_PIPE))
		return (0);
	(*i)++;
	return (1);
}

static int	handle_redirection_token(const char *input, size_t *i,
		t_token **tokens, int *exit_status)
{
	if (!check_syntax_error(input, *i, tokens, exit_status))
		return (0);
	if (!process_redirection(input, i, tokens, exit_status))
		return (0);
	return (1);
}

static int	process_token(const char *input, size_t *i, t_token **tokens,
		int *exit_status)
{
	if (isspace(input[*i]))
		(*i)++;
	else if (input[*i] == '|')
	{
		if (!handle_pipe_token(input, i, tokens, exit_status))
			return (0);
	}
	else if (input[*i] == '<' || input[*i] == '>')
	{
		if (!handle_redirection_token(input, i, tokens, exit_status))
			return (0);
	}
	else
	{
		if (!process_word(input, i, tokens))
			return (0);
	}
	return (1);
}

t_token	*tokenize(const char *input, int *exit_status)
{
	t_token	*tokens;
	size_t	i;

	if (!input)
		return (NULL);
	tokens = NULL;
	i = 0;
	while (input[i])
	{
		if (!process_token(input, &i, &tokens, exit_status))
			return (free_tokens(tokens), NULL);
	}
	return (tokens);
}
