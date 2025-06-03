/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:06:52 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:24:00 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_word(const char *input, int *i, char *buffer, size_t *j,
		int *in_word, char **env)
{
	char		*expanded;
	char		*temp;
	int			start;

	*in_word = 1;
	if (input[*i] == '$')
	{
		printf("DEBUG: Found $ in handle_word\n");
		start = *i;
		(*i)++;  // Skip the $
		while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
			(*i)++;
		
		// Extract variable name
		char *var_name = ft_substr(input, start + 1, *i - start - 1);
		if (var_name)
		{
			printf("DEBUG: Extracted var_name: %s\n", var_name);
			expanded = get_env_value_from_array(var_name, env);
			if (expanded)
			{
				printf("DEBUG: Expanded value: %s\n", expanded);
				temp = expanded;
				while (*temp)
					append_char(buffer, j, *temp++);
				free(expanded);
			}
			else
			{
				printf("DEBUG: Variable not found\n");
			}
			free(var_name);
		}
	}
	else if (input[*i] == '~')
	{
		start = *i;
		// For tilde expansion, we'll just expand to HOME
		expanded = get_env_value_from_array("HOME", env);
		if (expanded)
		{
			temp = expanded;
			while (*temp)
				append_char(buffer, j, *temp++);
			*i += ft_strlen(input + start) - 1;
			free(expanded);
		}
		else
			append_char(buffer, j, input[*i]);
		(*i)++;
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
	int	result;

	if (is_whitespace(input[*i]))
		handle_whitespace(i, buffer, j, tokens, in_word);
	else if (is_quote(input[*i]))
	{
		if (!handle_quotes(input, i, buffer, j))
			return (0);
		*in_word = 1;
	}
	else if (is_operator(input[*i]))
	{
		handle_operator_in_word(buffer, j, tokens, in_word);
		result = handle_operator(input, i, buffer, j, tokens);
		if (result == -1)
			return (-1);
	}
	else
		handle_word(input, i, buffer, j, in_word, env);
	return (1);
}

t_token	*process_input(const char *input, char *buffer, char **env)
{
	t_token	*tokens;
	size_t	j;
	int		i;
	int		in_word;
	int		result;

	tokens = init_process_vars(&j, &i, &in_word);
	while (input[i])
	{
		result = process_current_char(input, &i, buffer, &tokens, &j, &in_word, env);
		if (result == 0)
		{
			printf("Error: unclosed quote\n");
			free_tokens(tokens);
			return (NULL);
		}
		if (result == -1)
		{
			free_tokens(tokens);
			return (NULL);
		}
	}
	if (in_word || j > 0)
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
	char	buffer[4096];

	return (process_input(input, buffer, env));
}
