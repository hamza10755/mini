/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:37:40 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 22:47:18 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

// Token structure
typedef struct s_token
{
	char			*value;
	int				type;
	int				exit_status;
	struct s_token	*next;
	struct s_token	*prev;
}					t_token;

// Environment structure
typedef struct s_env
{
	char			*value;
	struct s_env	*next;
}					t_env;

// Token types
enum e_token_type
{
	TOKEN_WORD,
	TOKEN_WHITESPACE,
	TOKEN_PIPE,
	TOKEN_REDIR,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_SEMICOLON,
	TOKEN_QUOTE,
	TOKEN_ESCAPE
};

// Environment utilities
t_env	*init_env(char **envp);
void	free_env(t_env *env);
char	**convert_env_to_array(t_env *env);
char	*get_env_value(const char *name, t_env *env);
int		set_env_value(const char *name, const char *value, t_env **env);
void	unset_env_value(const char *name, t_env **env);
int		count_env_vars(t_env *env);
int		count_env_vars_array(char **env);
void	print_env(t_env *env, int fd_out);
void	sort_env(t_env **env);
void	init_shlvl_env(t_env **env);
void	update_shlvl_array(char **env);

// Builtin functions
int	builtin_echo(char **args, int fd_out);
int	builtin_cd(char **args, t_env *env);
int	builtin_pwd(int fd_out);
int	builtin_env(t_env *env, int fd_out);
int	builtin_exit(char **args);
int	builtin_export(char **args, t_env **env, int fd_out);
int	builtin_unset(char **args, t_env **env);

// Token functions
void	free_tokens(t_token *tokens);
void	print_tokens(t_token *tokens);
int		ensure_capacity(char **res, size_t *cap, size_t needed);
int		copy_expanded_value(char **res, size_t *j, char *expanded, size_t *capacity);

// Token expander functions
char	*get_env_value_from_array(const char *name, char **env);
int		append_expanded(char **buffer, size_t *pos, size_t *cap, char *value);
int		expand_var(const char *input, int *i, char **buffer, size_t *pos, size_t *cap, char **env, int *exit_status);
char	*expand_string(const char *input, char **env, int *exit_status);
void	expand_tokens(t_token *tokens, t_env *env, int *exit_status);

// Token utils functions
int		is_quote(char c);
int		is_whitespace(char c);
void	append_char(char *buffer, size_t *index, char c);
t_token	*create_new_token(char *value, int type);
char	*allocate_res_buff(char *str);

// Token builder functions
int		is_operator(char c);
t_token	*add_token(t_token **tokens,  char *value, int type);
int		flush_buffer(char *buffer, size_t *index, t_token **tokens);
int		handle_double_operator(const char *input, int *i, char *buf, size_t *j, int type);
int		handle_single_operator(const char *input, int *i, char *buf, size_t *j, int type);

// Token parser functions
int		handle_operator(const char *input, int *i, char *buffer, size_t *j, t_token **tokens);
int		process_quote_content(const char *input, int *i, char *buffer, size_t *j, char quote_char);
int		handle_quotes(const char *input, int *i, char *buffer, size_t *j);
int		handle_whitespace(int *i, char *buffer, size_t *j, t_token **tokens, int *in_word);
t_token	*handle_operator_in_word(char *buffer, size_t *j, t_token **tokens, int *in_word);

// Token main functions
void	handle_word(const char *input, int *i, char *buffer, size_t *j, int *in_word);
int		process_current_char(const char *input, int *i, char *buffer, t_token **tokens, size_t *j, int *in_word, char **env);
t_token	*process_input(const char *input, char *buffer, char **env);
t_token	*init_process_vars(size_t *j, int *i, int *in_word);
t_token	*tokenize(const char *input, int *exit_status);

// Builtin handler functions
int	is_builtin(t_token *tokens);
int	handle_builtin(char **args, t_env **env, int *exit_status, int fd_out);
int	execute_builtin(t_token *tokens, t_env **env, int *exit_status);

// Pipe handler functions
int	count_pipes(t_token *tokens);
void	execute_child_command(t_token *cmd_start, t_env *env, int is_first_child, t_token *tokens);
int	execute_pipeline(t_token *tokens, t_env **env, int *exit_status);
int	has_pipe(t_token *tokens);

// Exit status functions
int	get_exit_status(int exit_status);
void	update_exit_status(int wait_status, int *exit_status);
void	set_exit_status_env(int exit_status, t_env **env);

// Path and environment functions
void	free_array(char **array);
char	*resolve_command_path(char *command, char **env);
char	*search_in_path(char *command, char *path);
char	**convert_tokens_to_args(t_token *tokens);

// Command execution functions
int	execute_command(t_token *tokens, t_env **env, int *exit_status);
int	execute_simple_command(t_token *tokens, t_env **env, int *exit_status);

// Signal handling functions
void	init_signals(void);
void	reset_signals(void);
void	reset_signal_status(void);

// Redirection functions
int	handle_redirections(t_token *tokens, int *fd_in, int *fd_out);
int setup_redirection(t_token *token, int *fd_in, int *fd_out);

#endif