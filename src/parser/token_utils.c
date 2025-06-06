/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:04:42 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:03:04 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_quote(char c)
{
	return (c == '\'' || c == '\"');
}

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

void	append_char(char *buffer, size_t *index, char c)
{
	buffer[(*index)++] = c;
	buffer[*index] = '\0';
}

t_token	*create_new_token(const char *value, int type)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = ft_strdup(value);
	if (!new_token->value)
	{
		free(new_token);
		return (NULL);
	}
	new_token->type = type;
	new_token->next = NULL;
	new_token->prev = NULL;
	return (new_token);
}

char	*allocate_res_buff(char *str)
{
	char	*res;
	size_t	init_size;

	if (!str)
		return (NULL);
	init_size = ft_strlen(str) * 4 + 1;
	if (init_size < 1024)
		init_size = 1024;
	res = malloc(sizeof(char) * init_size);
	if (!res)
		return (NULL);
	return (res);
}

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
} 