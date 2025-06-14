/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 00:34:16 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_echo(char **args, int fd_out)
{
	int	i;
	int	newline;
	int	j;

	i = 1;
	newline = 1;
	if (args[1] && args[1][0] == '-')
	{
		j = 1;
		while (args[1][j] == 'n')
			j++;
		if (args[1][j] == '\0' && j > 1)
		{
			newline = 0;
			i++;
		}
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], fd_out);
		if (args[i + 1])
			ft_putchar_fd(' ', fd_out);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', fd_out);
	return (0);
}
