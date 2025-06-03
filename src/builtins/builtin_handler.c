/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/05/17 23:52:59 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int handle_builtin(char **args, char ***env, int *exit_status)
{
	if (ft_strncmp(args[0], "echo", 5) == 0)
		*exit_status = builtin_echo(args);
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		*exit_status = builtin_cd(args, *env);
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		*exit_status = builtin_pwd();
	else if (ft_strncmp(args[0], "export", 7) == 0)
		*exit_status = builtin_export(args, env);
	else if (ft_strncmp(args[0], "unset", 6) == 0)
		*exit_status = builtin_unset(args, env);
	else if (ft_strncmp(args[0], "env", 4) == 0)
		*exit_status = builtin_env(*env);
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		*exit_status = builtin_exit(args);
	else
		return (0);
	return (1);
} 