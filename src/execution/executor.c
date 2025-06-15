/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:13:20 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 23:34:28 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_signal_flag;

static char	*increment_shlvl(const char *env_entry)
{
	int		shlvl;
	char	*shlvl_str;
	char	*new_value;
	char	*itoa_res;

	shlvl_str = (char *)(env_entry + 6);
	shlvl = ft_atoi(shlvl_str);
	if (shlvl < 0)
		shlvl = 0;
	itoa_res = ft_itoa(shlvl + 1);
	if (!itoa_res)
		return (NULL);
	new_value = ft_strjoin("SHLVL=", itoa_res);
	free(itoa_res);
	return (new_value);
}

static void	free_env_array(char **env, int last)
{
	while (--last >= 0)
		free(env[last]);
	free(env);
}

static int	count_env_vars1(char **env)
{
	int	count;

	count = 0;
	while (env[count])
		count++;
	return (count);
}

static int	copy_env_entry(char **new_env, char **env, int i)
{
	new_env[i] = ft_strdup(env[i]);
	if (!new_env[i])
		return (0);
	return (1);
}

static int	handle_shlvl_entry(char **new_env, char **env, int i,
		int *shlvl_updated)
{
	char	*new_value;

	new_value = increment_shlvl(env[i]);
	if (!new_value)
		return (0);
	new_env[i] = new_value;
	*shlvl_updated = 1;
	return (1);
}

static int	copy_env_entries(char **new_env, char **env, int *shlvl_updated)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "SHLVL=", 6) == 0 && !*shlvl_updated)
		{
			if (!handle_shlvl_entry(new_env, env, i, shlvl_updated))
			{
				free_env_array(new_env, i);
				return (0);
			}
		}
		else
		{
			if (!copy_env_entry(new_env, env, i))
			{
				free_env_array(new_env, i);
				return (0);
			}
		}
		i++;
	}
	return (1);
}

static char	**allocate_new_env(char **env)
{
	char	**new_env;
	int		count;

	count = count_env_vars1(env);
	new_env = malloc(sizeof(char *) * (count + 2));
	return (new_env);
}

static char	**copy_env_with_shlvl(char **env, int *shlvl_updated)
{
	char	**new_env;

	*shlvl_updated = 0;
	new_env = allocate_new_env(env);
	if (!new_env)
		return (NULL);
	if (!copy_env_entries(new_env, env, shlvl_updated))
		return (NULL);
	return (new_env);
}

static char	**copy_and_update_env(char **env, int *shlvl_updated)
{
	char	**new_env;
	int		i;

	new_env = copy_env_with_shlvl(env, shlvl_updated);
	if (!new_env)
		return (NULL);
	i = 0;
	while (new_env[i])
		i++;
	if (!*shlvl_updated)
	{
		new_env[i] = ft_strdup("SHLVL=1");
		if (!new_env[i])
		{
			free_env_array(new_env, i);
			return (NULL);
		}
		i++;
		*shlvl_updated = 1;
	}
	new_env[i] = NULL;
	return (new_env);
}

static int	handle_shell_env(char **cmd_path, char ***env)
{
	char	**new_env;
	int		shlvl_updated;

	if (ft_strnstr(*cmd_path, "minishell", ft_strlen(*cmd_path))
		|| ft_strnstr(*cmd_path, "bash", ft_strlen(*cmd_path))
		|| ft_strnstr(*cmd_path, "sh", ft_strlen(*cmd_path))
		|| ft_strnstr(*cmd_path, "zsh", ft_strlen(*cmd_path)))
	{
		new_env = copy_and_update_env(*env, &shlvl_updated);
		if (!new_env)
			return (1);
		*env = new_env;
		return (2);
	}
	return (0);
}

static void	cleanup_env(char **env, int env_allocated)
{
	int	i;

	if (env_allocated == 2)
	{
		i = 0;
		while (env[i])
			free(env[i++]);
		free(env);
	}
}

static int	execute_child_process(char *cmd_path, char **args, char **env)
{
	int	i;

	reset_signals();
	if (execve(cmd_path, args, env) == -1)
	{
		i = 0;
		while (env[i])
			free(env[i++]);
		free(env);
		perror("execve");
		exit(1);
	}
	exit(1);
}

static int	handle_parent_process(pid_t pid, int *exit_status)
{
	int	status;

	waitpid(pid, &status, 0);
	init_signals();
	update_exit_status(status, exit_status);
	return (*exit_status);
}

int	execute_command_with_path(char *cmd_path, char **args, char **env)
{
	pid_t	pid;
	int		exit_status;
	int		env_allocated;

	exit_status = 0;
	env_allocated = handle_shell_env(&cmd_path, &env);
	if (env_allocated == 1)
		return (1);
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		cleanup_env(env, env_allocated);
		perror("fork");
		return (1);
	}
	if (pid == 0)
		exit_status = execute_child_process(cmd_path, args, env);
	else
		exit_status = handle_parent_process(pid, &exit_status);
	cleanup_env(env, env_allocated);
	return (exit_status);
}

static int	check_for_command(t_token *tokens)
{
	t_token	*current;
	int		has_command;
			t_token *prev;

	current = tokens;
	has_command = 0;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			prev = current->prev;
			if (!prev || (prev->type != TOKEN_REDIR
					&& prev->type != TOKEN_APPEND
					&& prev->type != TOKEN_HEREDOC))
			{
				has_command = 1;
				break ;
			}
		}
		if (current->type == TOKEN_PIPE || current->type == TOKEN_SEMICOLON)
			break ;
		current = current->next;
	}
	return (has_command);
}

static int	handle_heredoc_only(t_token *tokens, int *exit_status)
{
	int	fd_in;
	int	fd_out;

	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (handle_redirections(tokens, &fd_in, &fd_out) != 0)
	{
		if (g_signal_flag == SIGINT)
			*exit_status = 1;
		return (1);
	}
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	if (g_signal_flag == SIGINT)
		*exit_status = 1;
	else
		*exit_status = 0;
	return (0);
}

static int	handle_redirection_error(t_token *tokens, int *exit_status)
{
	if (tokens->type == TOKEN_REDIR)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '>'\n", 2);
		*exit_status = 2;
		return (1);
	}
	else if (tokens->type == TOKEN_APPEND)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '>>'\n", 2);
		*exit_status = 2;
		return (1);
	}
	return (0);
}

static int	handle_builtin_execution(char **args, t_env **env, int *exit_status,
		int fd_out)
{
	int	result;

	result = handle_builtin(args, env, exit_status, fd_out);
	free_array(args);
	return (result);
}

static int	handle_env_array_error(char **args, int *exit_status)
{
	free_array(args);
	*exit_status = 1;
	return (1);
}

static int	handle_command_not_found(char **args, char **env_array,
		int *exit_status)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(args[0], 2);
	ft_putstr_fd(": command not found\n", 2);
	free_array(args);
	free_array(env_array);
	*exit_status = 127;
	return (127);
}

static int	execute_external_command(char **args, char **env_array,
		char *cmd_path)
{
	int	result;

	result = execute_command_with_path(cmd_path, args, env_array);
	free(cmd_path);
	free_array(args);
	free_array(env_array);
	return (result);
}

static int	handle_command_execution(t_token *tokens, t_env **env,
		int *exit_status, int fd_out)
{
	char	**args;
	char	**env_array;
	char	*cmd_path;

	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	if (is_builtin(tokens))
		return (handle_builtin_execution(args, env, exit_status, fd_out));
	env_array = convert_env_to_array(*env);
	if (!env_array)
		return (handle_env_array_error(args, exit_status));
	cmd_path = resolve_command_path(args[0], env_array);
	if (!cmd_path)
		return (handle_command_not_found(args, env_array, exit_status));
	return (execute_external_command(args, env_array, cmd_path));
}

static void	restore_fds(int saved_stdin, int saved_stdout, int fd_in,
		int fd_out)
{
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
}

static int	setup_file_descriptors(t_token *tokens, int *fd_in, int *fd_out)
{
	*fd_in = STDIN_FILENO;
	*fd_out = STDOUT_FILENO;
	if (handle_redirections(tokens, fd_in, fd_out) != 0)
		return (1);
	return (0);
}

static int	save_and_redirect_fds(int fd_in, int fd_out, int *saved_stdin,
		int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
	if (fd_in != STDIN_FILENO)
		dup2(fd_in, STDIN_FILENO);
	if (fd_out != STDOUT_FILENO)
		dup2(fd_out, STDOUT_FILENO);
	return (0);
}

static int	handle_no_command(t_token *tokens, int *exit_status)
{
	if (tokens->type == TOKEN_HEREDOC)
		return (handle_heredoc_only(tokens, exit_status));
	return (handle_redirection_error(tokens, exit_status));
}

static int	prepare_and_execute(t_token *tokens, t_env **env, int *exit_status,
		int fd_out)
{
	char	**args;
	int		result;

	args = convert_tokens_to_args(tokens);
	if (!args)
		return (1);
	result = handle_command_execution(tokens, env, exit_status, fd_out);
	return (result);
}

int	execute_simple_command(t_token *tokens, t_env **env, int *exit_status)
{
	int	fd_in;
	int	fd_out;
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	if (!tokens)
		return (1);
	if (!check_for_command(tokens))
		return (handle_no_command(tokens, exit_status));
	if (setup_file_descriptors(tokens, &fd_in, &fd_out) != 0)
		return (1);
	if (save_and_redirect_fds(fd_in, fd_out, &saved_stdin, &saved_stdout) != 0)
		return (1);
	result = prepare_and_execute(tokens, env, exit_status, fd_out);
	restore_fds(saved_stdin, saved_stdout, fd_in, fd_out);
	*exit_status = result;
	return (result);
}

static int	execute_pipeline_command(t_token *tokens, t_env **env,
		int *exit_status)
{
	int	result;

	result = execute_pipeline(tokens, env, exit_status);
	return (result);
}

static int	execute_single_command(t_token *tokens, t_env **env,
		int *exit_status)
{
	int	result;

	result = execute_simple_command(tokens, env, exit_status);
	return (result);
}

int	execute_command(t_token *tokens, t_env **env, int *exit_status)
{
	int	result;

	if (!tokens)
		return (1);
	if (has_pipe(tokens))
		result = execute_pipeline_command(tokens, env, exit_status);
	else
		result = execute_single_command(tokens, env, exit_status);
	return (result);
}
