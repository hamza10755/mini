/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtok.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:03:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 22:08:15 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_delimiter(char c, const char *delim)
{
	while (*delim)
	{
		if (c == *delim)
			return (1);
		delim++;
	}
	return (0);
}

char	*ft_strtok(char *str, const char *delim)
{
	static char	*last;
	char		*token_start;

	if (str)
		last = str;
	if (!last || !*last)
		return (NULL);
	// Skip leading delimiters
	while (*last && is_delimiter(*last, delim))
		last++;
	if (!*last)
		return (NULL);
	token_start = last;
	// Find the end of the token
	while (*last && !is_delimiter(*last, delim))
		last++;
	if (*last)
	{
		*last = '\0';
		last++;
	}
	return (token_start);
} 