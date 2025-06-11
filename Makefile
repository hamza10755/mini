NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -I./includes -g
LDFLAGS = -lreadline -lncurses

SRC = src
LIBFTDIR = libft 

SRCS = main.c \
       parser/token_builder.c \
       parser/token_parser.c \
       parser/token_expander.c \
       parser/token_utils.c \
       parser/token_main.c \
       parser/token_debug.c \
       execution/executor.c \
       execution/redirections.c \
       execution/path_env.c \
       execution/signals.c \
       execution/exit_status.c \
       execution/pipe_handler.c \
       builtins/builtin_handler.c \
       builtins/cd.c \
       builtins/echo.c \
       builtins/env.c \
       builtins/export.c \
       builtins/pwd.c \
       builtins/unset.c \
       builtins/env_utils.c \
       builtins/env_state.c \
       builtins/exit.c

SRC_FILES = $(addprefix $(SRC)/, $(SRCS))
OBJ_FILES = $(SRC_FILES:.c=.o)

LIBFT = $(LIBFTDIR)/libft.a

all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(LIBFT) $(OBJ_FILES)
	@$(CC) $(OBJ_FILES) -L$(LIBFTDIR) -lft -o $(NAME) $(LDFLAGS)

$(LIBFT):
	@make -C $(LIBFTDIR)

clean:
	@rm -f $(OBJ_FILES)
	@make -C $(LIBFTDIR) clean

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
