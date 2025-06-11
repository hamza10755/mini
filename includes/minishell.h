/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelaih <hbelaih@student.42.amman>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:37:40 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/11 15:46:11 by hbelaih          ###   ########.fr       */
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
# define TOKEN_WORD 0
# define TOKEN_WHITESPACE 1
# define TOKEN_PIPE 2
# define TOKEN_REDIR 3
# define TOKEN_HEREDOC 4
# define TOKEN_APPEND 5
# define TOKEN_SEMICOLON 6
# define TOKEN_QUOTE 7
# define TOKEN_ESCAPE 8

// token_expander.c
char				*get_env_value_from_array(const char *name, char **env);
int					append_expanded(char **buffer, size_t *pos, size_t *cap,
						char *value);
int					expand_var(const char *input, int *i, char **buffer,
						size_t *pos, size_t *cap, char **env, int *exit_status);
char				*expand_string(const char *input, char **env,
						int *exit_status);
void				expand_tokens(t_token *tokens, char **env,
						int *exit_status);

// token_utils.c
int					is_quote(char c);
int					is_whitespace(char c);
void				append_char(char *buffer, size_t *index, char c);
t_token				*create_new_token(const char *value, int type);
char				*allocate_res_buff(char *str);

// token_builder.c
int					is_operator(char c);
t_token				*add_token(t_token **tokens, const char *value, int type);
int				flush_buffer(char *buffer, size_t *index, t_token **tokens);
int					handle_double_operator(const char *input, int *i, char *buf,
						size_t *j, int type);
int					handle_single_operator(const char *input, int *i, char *buf,
						size_t *j, int type);

// token_parser.c
int					handle_operator(const char *input, int *i, char *buffer,
						size_t *j, t_token **tokens);
int					process_quote_content(const char *input, int *i,
						char *buffer, size_t *j, char quote_char);
int					handle_quotes(const char *input, int *i, char *buffer,
						size_t *j);
int				handle_whitespace(int *i, char *buffer, size_t *j,
						t_token **tokens, int *in_word);
t_token				*handle_operator_in_word(char *buffer, size_t *j,
						t_token **tokens, int *in_word);

// token_main.c
void				handle_word(const char *input, int *i, char *buffer,
						size_t *j, int *in_word, char **env);
int					process_current_char(const char *input, int *i,
						char *buffer, t_token **tokens, size_t *j,
						int *in_word, char **env);
t_token				*process_input(const char *input, char *buffer, char **env);
t_token				*init_process_vars(size_t *j, int *i, int *in_word);
t_token				*tokenize(const char *input, char **env);

// token_debug.c
const char			*get_token_type_name(int type);
void				print_tokens(t_token *tokens);
void                free_tokens(t_token *tokens);
int                 ensure_capacity(char **res, size_t *cap, size_t needed);
int                 copy_expanded_value(char **res, size_t *j, char *expanded,
                        size_t *capacity);

// Environment utilities
char	**copy_env(char **env);
char	**add_env_var(char **env, const char *name, const char *value);
char	**update_env_var(char **env, const char *name, const char *value);
char	**remove_env_var(char **env, const char *name);
void	sort_env_vars(char **env);
char	*get_env_value_from_array(const char *name, char **env);
void	free_env_array(char **env);
int		count_env_vars(char **env);
char	**init_env(char **environ);
void	cleanup_env(char **env);
void	free_env_array(char **env);


// Builtin functions
int	builtin_echo(char **args, int fd_out);
int	builtin_cd(char **args, char **env);
int	builtin_pwd(int fd_out);
int	builtin_env(char **env, int fd_out);
int	builtin_exit(char **args);
int	handle_builtin(char **args, char ***env, int *exit_status, int fd_out);
int	execute_builtin(t_token *tokens, char **env, int *exit_status);

// Execution functions
int	execute_command(t_token *tokens, char ***env, int *exit_status);
int	execute_pipeline(t_token *tokens, char ***env, int *exit_status);
void	execute_child_command(t_token *cmd_start, char **env, int is_first_child, t_token *tokens);
int	has_pipe(t_token *tokens);
int	count_pipes(t_token *tokens);

// Redirection handling
int	handle_redirections(t_token *tokens, int *fd_in, int *fd_out);
int	setup_redirection(t_token *token, int *fd_in, int *fd_out);
void	restore_redirections(int fd_in, int fd_out);

// Path resolution
char				*resolve_command_path(char *command, char **env);
char				*search_in_path(char *command, char *path);

// Environment handling
char				**convert_env_to_array(t_env *env);
void				free_env_array(char **env_array);
char				**convert_tokens_to_args(t_token *tokens);
void				sort_env_vars(char **env);

// Pipe handling functions
int		count_pipes(t_token *tokens);
void	setup_pipe_ends(int pipe_fd[2], int prev_pipe_read, int is_last_cmd);
int		execute_pipe_command(t_token *cmd_start, char **env);
void	handle_child_process(t_token *cmd_start, t_token *cmd_end, 
		int pipe_fd[2], int prev_pipe_read, char **env);
int		has_pipe(t_token *tokens);
int		execute_command_with_path(char *cmd_path, char **args, char **env);

// Process handling

// Builtin handling
int					is_builtin(t_token *token);
int					execute_builtin(t_token *tokens, char **env,
						int *exit_status);

// Environment state management
t_env				*get_env(void);
void				set_env(t_env *env);
void				init_global_env(char **env_array);

// Signal handling
void	init_signals(void);
void	reset_signals(void);
void	reset_signal_status(void);
void 	reset_signals_heredoc(void);
void 	signals_setup(int mode);
void 	close_heredoc(int signum);

// Exit status handling
int		get_exit_status(int exit_status);
void	update_exit_status(int wait_status, int *exit_status);
void	set_exit_status_env(int exit_status, char ***env);

char    **builtin_export(char **args, char ***env, int fd_out);
char    **builtin_unset(char **args, char ***env);

void print_sorted_env(char **env, int fd_out);

/* Environment functions */
char	**init_env(char **environ);
void	free_env_array(char **env);
int		is_valid_env_var(const char *str);
int	is_env_var_match(const char *env_entry, const char *name, int name_len);
int		update_env_var_internal(char **env, const char *var);
void	print_sorted_env(char **env, int fd_out);

/* Token functions */
void	free_tokens(t_token *tokens);
void	print_tokens(t_token *tokens);
void	free_array(char **array);

#endif