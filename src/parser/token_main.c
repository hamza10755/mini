/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:06:52 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 01:05:33 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
    int     quote_result;
    
    tokens = init_process_vars(&j, &i, &in_word);
    while (input[i])
    {
        if (input[i] == '\'' || input[i] == '"')
        {
            quote_result = handle_quotes(input, &i, buffer, &j);
            if (!quote_result)
            {
                if (tokens)
                    free_tokens(tokens);
                ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
                return (NULL);
            }
        }
        else if (!process_current_char(input, &i, buffer, &tokens, &j, &in_word, env))
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

static int	process_word(const char *input, size_t *i, t_token **tokens)
{
	size_t	start;
	char	*word;
	int		in_quotes;
	char	quote_char;
	int		found_equals;

	start = *i;
	in_quotes = 0;
	quote_char = 0;
	found_equals = 0;
	while (input[*i] && (!isspace(input[*i]) || in_quotes || found_equals))
	{
		if ((input[*i] == '"' || input[*i] == '\'') && !in_quotes)
		{
			in_quotes = 1;
			quote_char = input[*i];
		}
		else if (input[*i] == quote_char && in_quotes)
			in_quotes = 0;
		else if (input[*i] == '=' && !in_quotes)
			found_equals = 1;
		(*i)++;
	}
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

static int	process_redirection(const char *input, size_t *i, t_token **tokens, int *exit_status)
{
	if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			if (input[*i + 2] == '<')
			{
				ft_putstr_fd("minishell: syntax error near unexpected token '<<<'\n", 2);
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
	}
	else if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			if (input[*i + 2] == '>')
			{
				ft_putstr_fd("minishell: syntax error near unexpected token '>'\n", 2);
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
	}
	return (1);
}

static int	check_syntax_error(const char *input, size_t i, t_token **tokens, int *exit_status)
{
	t_token	*last_token;

	if (input[i] == '|')
	{
		last_token = *tokens;
		while (last_token && last_token->next)
			last_token = last_token->next;
		if (last_token && last_token->type == TOKEN_PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
			return (0);
		}
	}

	if (input[i] == '>' || input[i] == '<')
	{
		last_token = *tokens;
		while (last_token && last_token->next)
			last_token = last_token->next;
		if (last_token && (last_token->type == TOKEN_REDIR || 
			last_token->type == TOKEN_APPEND || last_token->type == TOKEN_HEREDOC))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
			if (input[i] == '>' && input[i + 1] == '>')
				ft_putstr_fd(">>'\n", 2);
			else
				ft_putstr_fd(">'\n", 2);
			return (0);
		}
	}

	if (input[i] == '>' && input[i + 1] == '>' && input[i + 2] == '>')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '>>'\n", 2);
		return (0);
	}

	if (input[i] == '|' && input[i + 1] == '|')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '||'\n", 2);
		*exit_status = 258;
		return (0);
	}

	return (1);
}

t_token	*tokenize(const char *input, char **env, int *exit_status)
{
	t_token *tokens;
	size_t	i;

	(void)env;
	if (!input)
		return (NULL);
	tokens = NULL;
	i = 0;
	while (input[i])
	{
		if (isspace(input[i]))
			i++;
		else if (input[i] == '|')
		{
			if (!check_syntax_error(input, i, &tokens, exit_status))
				return (free_tokens(tokens), NULL);
			if (!add_token(&tokens, "|", TOKEN_PIPE))
				return (free_tokens(tokens), NULL);
			i++;
		}
		else if (input[i] == '<' || input[i] == '>')
		{
			if (!check_syntax_error(input, i, &tokens, exit_status))
				return (free_tokens(tokens), NULL);
			if (!process_redirection(input, &i, &tokens, exit_status))
				return (free_tokens(tokens), NULL);
		}
		else
		{
			if (!process_word(input, &i, &tokens))
				return (free_tokens(tokens), NULL);
		}
	}
	return (tokens);
}
