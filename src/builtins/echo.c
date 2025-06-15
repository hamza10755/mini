/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:22:25 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	check_n_flag(char *arg, int *newline)
{
	int	j;

	if (!arg || arg[0] != '-')
		return (0);
	j = 1;
	while (arg[j] == 'n')
		j++;
	if (arg[j] == '\0' && j > 1)
	{
		*newline = 0;
		return (1);
	}
	return (0);
}

static void	print_args(char **args, int start, int fd_out)
{
	int	i;

	i = start;
	while (args[i])
	{
		ft_putstr_fd(args[i], fd_out);
		if (args[i + 1])
			ft_putchar_fd(' ', fd_out);
		i++;
	}
}

int	builtin_echo(char **args, int fd_out)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (args[1] && check_n_flag(args[1], &newline))
		i++;
	print_args(args, i, fd_out);
	if (newline)
		ft_putchar_fd('\n', fd_out);
	return (0);
}
