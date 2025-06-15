/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_expander.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamzabillah <hamzabillah@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:30:00 by hamzabillah       #+#    #+#             */
/*   Updated: 2025/06/15 22:17:41 by hamzabillah      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	append_expanded(char **buffer, size_t *pos, size_t *cap, char *value)
{
	size_t	len;

	if (!value)
		return (1);
	len = ft_strlen(value);
	if (!ensure_capacity(buffer, cap, *pos + len + 1))
		return (0);
	ft_strlcpy(*buffer + *pos, value, *cap - *pos + 1);
	*pos += len;
	return (1);
}

static int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

static int	count_var_chars(const char *str)
{
	int	count;

	count = 0;
	while (str[count] && is_valid_var_char(str[count]))
		count++;
	return (count);
}

static char	*extract_var_name(const char *str, int len)
{
	char	*name;

	name = malloc(len + 1);
	if (!name)
		return (NULL);
	ft_strlcpy(name, str, len + 1);
	return (name);
}

static int	handle_special_var(const char *name, char **buffer, size_t *pos,
		size_t *cap, int *exit_status)
{
	char	*value;

	if (ft_strncmp(name, "?", 2) == 0)
	{
		value = ft_itoa(*exit_status);
		if (!value)
			return (1);
		if (append_expanded(buffer, pos, cap, value) != 0)
		{
			free(value);
			return (1);
		}
		free(value);
		return (0);
	}
	return (-1);
}

static int	handle_exit_status_var(int *i, char **buffer, size_t *pos,
		size_t *cap, int *exit_status)
{
	char	*value;

	(*i)++;
	value = ft_itoa(*exit_status);
	if (!value)
		return (1);
	if (append_expanded(buffer, pos, cap, value) != 0)
	{
		free(value);
		return (1);
	}
	free(value);
	return (0);
}

static int	handle_digit_var(const char *input, int *i, char **buffer,
		size_t *pos)
{
	(*i)++;
	while (input[*i] && is_valid_var_char(input[*i]))
	{
		append_char(*buffer, pos, input[*i]);
		(*i)++;
	}
	return (0);
}

static int	handle_empty_var(char **buffer, size_t *pos, size_t *cap)
{
	if (append_expanded(buffer, pos, cap, "$") != 0)
		return (1);
	return (0);
}

static void	init_space_vars(int *j, int *k, int *space_count)
{
	*j = 0;
	*k = 0;
	*space_count = 0;
}

static void	handle_space_char(char *new_value, int *k, int *space_count)
{
	if (*space_count == 0)
		new_value[(*k)++] = ' ';
	(*space_count)++;
}

static void	handle_non_space_char(char *new_value, int *k, int *space_count,
		char current_char)
{
	new_value[(*k)++] = current_char;
	*space_count = 0;
}

static char	*process_value_spaces(const char *value)
{
	char	*new_value;
	int		j;
	int		k;
	int		space_count;

	new_value = malloc(ft_strlen(value) + 1);
	if (!new_value)
		return (NULL);
	init_space_vars(&j, &k, &space_count);
	while (value[j])
	{
		if (value[j] == ' ')
			handle_space_char(new_value, &k, &space_count);
		else
			handle_non_space_char(new_value, &k, &space_count, value[j]);
		j++;
	}
	new_value[k] = '\0';
	return (new_value);
}

static int	handle_value_expansion(char **buffer, size_t *pos, size_t *cap,
		char *value, int in_quotes)
{
	char	*new_value;
	int		result;

	if (in_quotes)
	{
		result = append_expanded(buffer, pos, cap, value);
		return (result);
	}
	new_value = process_value_spaces(value);
	if (!new_value)
		return (1);
	result = append_expanded(buffer, pos, cap, new_value);
	free(new_value);
	return (result);
}

static int	check_quotes(const char *input, int *i)
{
	if (*i > 0 && input[*i - 1] == '\"')
		return (1);
	return (0);
}

static int	handle_var_name(const char *input, int *i, char **name, int len)
{
	*name = extract_var_name(input + *i, len);
	if (!*name)
		return (1);
	*i += len;
	return (0);
}

static int	process_var_value(char **name, char **value, char **env)
{
	int	special;

	special = handle_special_var(*name, NULL, NULL, NULL, NULL);
	if (special != -1)
	{
		free(*name);
		return (special);
	}
	*value = get_env_value_from_array(*name, env);
	free(*name);
	if (!*value)
		return (0);
	return (-1);
}

int	expand_var(const char *input, int *i, char **buffer, size_t *pos,
		size_t *cap, char **env, int *exit_status)
{
	char	*name;
	char	*value;
	int		len;
	int		in_quotes;
	int		result;

	in_quotes = check_quotes(input, i);
	(*i)++;
	if (input[*i] == '?')
		return (handle_exit_status_var(i, buffer, pos, cap, exit_status));
	if (ft_isdigit(input[*i]))
		return (handle_digit_var(input, i, buffer, pos));
	len = count_var_chars(input + *i);
	if (len == 0)
		return (handle_empty_var(buffer, pos, cap));
	if (handle_var_name(input, i, &name, len))
		return (1);
	result = process_var_value(&name, &value, env);
	if (result != -1)
		return (result);
	return (handle_value_expansion(buffer, pos, cap, value, in_quotes));
}

static int	ensure_buffer_capacity(char **result, size_t *cap, size_t *pos)
{
	char	*tmp;

	if (*pos + 2 >= *cap)
	{
		*cap *= 2;
		tmp = malloc(*cap);
		if (!tmp)
		{
			free(*result);
			return (0);
		}
		ft_memcpy(tmp, *result, *pos);
		tmp[*pos] = '\0';
		free(*result);
		*result = tmp;
	}
	return (1);
}

static int	init_expand_string(char **result, size_t *cap)
{
	*cap = 16;
	*result = malloc(*cap);
	if (!*result)
		return (0);
	(*result)[0] = '\0';
	return (1);
}

static int	handle_single_quotes(const char *input, int *i, char **result,
		size_t *pos, size_t *cap)
{
	(*i)++;
	while (input[*i] && input[*i] != '\'')
	{
		if (!ensure_buffer_capacity(result, cap, pos))
			return (0);
		append_char(*result, pos, input[*i]);
		(*i)++;
	}
	if (input[*i] == '\'')
		(*i)++;
	return (1);
}

static int	handle_double_quotes(const char *input, int *i, char **result,
		size_t *pos, size_t *cap, char **env, int *exit_status)
{
	(*i)++;
	while (input[*i] && input[*i] != '\"')
	{
		if (input[*i] == '$')
		{
			if (expand_var(input, i, result, pos, cap, env, exit_status) == -1)
				return (0);
		}
		else
		{
			if (!ensure_buffer_capacity(result, cap, pos))
				return (0);
			append_char(*result, pos, input[*i]);
			(*i)++;
		}
	}
	if (input[*i] == '\"')
		(*i)++;
	return (1);
}

static int	handle_dollar_sign(const char *input, int *i, char **result,
		size_t *pos, size_t *cap, char **env, int *exit_status)
{
	if (expand_var(input, i, result, pos, cap, env, exit_status) == -1)
		return (0);
	return (1);
}

static int	process_char(const char *input, int *i, char **result, size_t *pos,
		size_t *cap, char **env, int *exit_status)
{
	if (input[*i] == '\'')
	{
		if (!handle_single_quotes(input, i, result, pos, cap))
			return (0);
	}
	else if (input[*i] == '\"')
	{
		if (!handle_double_quotes(input, i, result, pos, cap, env, exit_status))
			return (0);
	}
	else if (input[*i] == '$')
	{
		if (!handle_dollar_sign(input, i, result, pos, cap, env, exit_status))
			return (0);
	}
	else
	{
		if (!ensure_buffer_capacity(result, cap, pos))
			return (0);
		append_char(*result, pos, input[*i]);
		(*i)++;
	}
	return (1);
}

char	*expand_string(const char *input, char **env, int *exit_status)
{
	char	*result;
	size_t	pos;
	size_t	cap;
	int		i;

	if (!input)
		return (NULL);
	if (!init_expand_string(&result, &cap))
		return (NULL);
	pos = 0;
	i = 0;
	while (input[i])
	{
		if (!process_char(input, &i, &result, &pos, &cap, env, exit_status))
		{
			free(result);
			return (NULL);
		}
	}
	return (result);
}

static int	check_export_value(const char *value)
{
	char	*equal_sign;
	char	*quote_start;
	char	*quote_end;

	equal_sign = ft_strchr(value, '=');
	if (!equal_sign)
		return (0);
	quote_start = ft_strchr(equal_sign, '"');
	if (!quote_start)
		quote_start = ft_strchr(equal_sign, '\'');
	if (!quote_start)
		return (0);
	quote_end = ft_strrchr(quote_start, *quote_start);
	if (!quote_end || quote_end <= quote_start)
		return (0);
	return (1);
}

static void	handle_export_value(t_token *current)
{
	char	*expanded;

	expanded = ft_strdup(current->value);
	if (expanded)
	{
		free(current->value);
		current->value = expanded;
	}
}

static void	create_split_token(t_token *current, char *value, int exit_status)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return ;
	new_token->value = ft_strdup(value);
	new_token->type = TOKEN_WORD;
	new_token->exit_status = exit_status;
	new_token->next = current->next;
	if (current->next)
		current->next->prev = new_token;
	new_token->prev = current;
	current->next = new_token;
}

static void	handle_split_tokens(t_token *current, char **split_tokens,
		int exit_status)
{
	int	i;

	i = 1;
	while (split_tokens[i])
	{
		create_split_token(current, split_tokens[i], exit_status);
		current = current->next;
		i++;
	}
	i = 0;
	while (split_tokens[i])
		free(split_tokens[i++]);
	free(split_tokens);
}

static void	handle_word_expansion(t_token *current, char **env_array,
		int *exit_status, int is_command)
{
	char	*expanded;
	char	**split_tokens;

	expanded = expand_string(current->value, env_array, exit_status);
	if (!expanded)
		return ;
	if (ft_strchr(expanded, ' ') != NULL && !is_command)
	{
		split_tokens = ft_split(expanded, ' ');
		if (split_tokens)
		{
			free(current->value);
			current->value = ft_strdup(split_tokens[0]);
			handle_split_tokens(current, split_tokens, *exit_status);
		}
		free(expanded);
	}
	else
	{
		free(current->value);
		current->value = expanded;
	}
}

static void	process_token(t_token *current, char **env_array, int *exit_status,
		int is_command, int is_export)
{
	if (is_export && check_export_value(current->value))
		handle_export_value(current);
	else
		handle_word_expansion(current, env_array, exit_status, is_command);
}

void	expand_tokens(t_token *tokens, t_env *env, int *exit_status)
{
	t_token	*current;
	t_token	*next;
	int		is_command;
	int		is_export;
	char	**env_array;

	current = tokens;
	is_command = 1;
	is_export = 0;
	env_array = convert_env_to_array(env);
	if (!env_array)
		return ;
	if (current && current->type == TOKEN_WORD && ft_strncmp(current->value,
			"export", 6) == 0)
		is_export = 1;
	while (current)
	{
		next = current->next;
		if (current->type == TOKEN_WORD)
			process_token(current, env_array, exit_status, is_command,
				is_export);
		current = next;
		is_command = 0;
	}
	free_array(env_array);
}
