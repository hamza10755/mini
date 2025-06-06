/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:06:52 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:13:52 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*extract_var_name(const char *input)
{
	int		i;
	char	*var_name;

	i = 0;
	while (input[i] && (ft_isalnum(input[i]) || input[i] == '_'))
		i++;
	var_name = ft_substr(input, 0, i);
	return (var_name);
}

void	handle_word(const char *input, int *i, char *buffer, size_t *j,
		int *in_word, char **env)
{
	char	*var_name;
	char	*expanded;
	char	*temp;

	*in_word = 1;
	if (input[*i] == '$')
	{
		(*i)++;
		var_name = extract_var_name(input + *i);
		if (var_name)
		{
			expanded = get_env_value_from_array(var_name, env);
			if (expanded)
			{
				temp = expanded;
				while (*temp)
					append_char(buffer, j, *temp++);
				free(expanded);
			}
			free(var_name);
		}
		*i += ft_strlen(var_name);
	}
	else if (input[*i] == '~')
	{
		expanded = get_env_value_from_array("HOME", env);
		if (expanded)
		{
			temp = expanded;
			while (*temp)
				append_char(buffer, j, *temp++);
			*i += ft_strlen(input + *i) - 1;
			free(expanded);
		}
	}
	else
	{
		append_char(buffer, j, input[*i]);
		(*i)++;
	}
}

int	process_current_char(const char *input, int *i, char *buffer,
		t_token **tokens, size_t *j, int *in_word, char **env)
{
	if (is_whitespace(input[*i]))
	{
		handle_whitespace(i, buffer, j, tokens, in_word);
		return (1);
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

t_token	*process_input(const char *input, char *buffer, char **env)
{
	t_token	*tokens;
	size_t	j;
	int		i;
	int		in_word;

	tokens = init_process_vars(&j, &i, &in_word);
	while (input[i])
	{
		if (!process_current_char(input, &i, buffer, &tokens, &j, &in_word, env))
		{
			free_tokens(tokens);
			return (NULL);
		}
	}
	if (in_word)
		flush_buffer(buffer, &j, &tokens);
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
