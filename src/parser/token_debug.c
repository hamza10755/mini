/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:08:18 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/06 21:05:52 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

const char	*get_token_type_name(int type)
{
	const char	*type_names[] = {"WORD", "WHITESPACE", "PIPE", "REDIR",
			"HEREDOC", "APPEND", "SEMICOLON", "QUOTE", "ESCAPE"};

	if (type >= 0 && type <= 8)
		return (type_names[type]);
	return ("UNKNOWN");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		printf("Token: %s, Type: %d\n", current->value, current->type);
		current = current->next;
	}
}

int	ensure_capacity(char **res, size_t *cap, size_t needed)
{
	char	*new_res;
	size_t	new_cap;

	if (*cap >= needed)
		return (1);
	new_cap = *cap * 2;
	if (new_cap < needed)
		new_cap = needed;
	new_res = malloc(sizeof(char) * new_cap);
	if (!new_res)
		return (0);
	ft_strlcpy(new_res, *res, *cap + 1);
	free(*res);
	*res = new_res;
	*cap = new_cap;
	return (1);
}

int	copy_expanded_value(char **res, size_t *j, char *expanded, size_t *capacity)
{
	size_t	vl;

	if (!expanded)
		return (1);
	vl = ft_strlen(expanded);
	if (!ensure_capacity(res, capacity, *j + vl + 1))
		return (0);
	ft_strlcpy(*res + *j, expanded, *capacity + 1);
	*j += vl;
	return (1);
}
