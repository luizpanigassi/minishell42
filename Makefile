# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/31 16:25:32 by luinasci          #+#    #+#              #
#    Updated: 2025/05/08 17:05:00 by luinasci         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors
RED	  = \033[0;31m
GREEN	= \033[0;32m
YELLOW  = \033[0;33m
BLUE	 = \033[0;34m
MAGENTA = \033[0;35m
CYAN	 = \033[0;36m
RESET	= \033[0m

NAME		  = minishell
LIBFT_DIR	= libft
LIBFT_LIB	= $(LIBFT_DIR)/libft.a

SRC_PATH	 = src/
SRCS		  = $(addprefix $(SRC_PATH), \
				  create_heredoc.c \
				  execute_pipe.c \
				  exit_status.c \
				  get_cmd_path.c \
				  handle_commands.c \
				  handle_heredoc.c \
				  handle_parse_args.c \
				  handle_parse.c \
				  handle_pipe.c \
				  handle_redirect.c \
				  handle_special.c \
				  init.c \
				  input.c \
				  main.c \
				  parse_args.c \
				  parse_pipeline.c \
				  parser_1.c \
				  parser_2.c \
				  pipe_signals.c \
				  signals.c \
				  )

UTILS_PATH  = utils/
UTILS_SRCS  = $(addprefix $(UTILS_PATH), \
				  utils_1.c \
				  utils_2.c \
				  utils_3.c \
				  utils_4.c \
				  utils_5.c \
				  utils_6.c \
				  utils_7.c \
				  )

BUILTIN_PATH = builtin/
BUILTIN_SRCS = $(addprefix $(BUILTIN_PATH), \
				  builtin.c \
				  exec_cd.c \
				  exec_exit.c \
				  exec_echo.c \
				  exec_pwd.c \
				  exec_env.c \
				  exec_export.c \
				  exec_unset.c \
				  )

OBJ_DIR = obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o) $(UTILS_SRCS:.c=.o) $(BUILTIN_SRCS:.c=.o))
CC			 = gcc
CFLAGS		= -Wall -Wextra -Werror -g3
INCLUDES	 = -Iincludes -I$(LIBFT_DIR)
RM			 = rm -f

# Visual Elements
CHECK_MARK  = ✅
WRENCH		= 🔨
LEAF		  = 🌿
ROCKET		= 🚀
BROOM		 = 🧹
FIRE		  = 🔥

all:		  $(NAME)

$(NAME):	 $(OBJS) $(LIBFT_LIB)
	 @echo "$(GREEN)$(ROCKET) Linking $(NAME)...$(RESET)"
	 @$(CC) $(CFLAGS) $(OBJS) -o $@ -L$(LIBFT_DIR) -lft -lreadline
	 @echo "$(GREEN)$(CHECK_MARK) Successfully built $(NAME)!$(RESET)"
	 @echo "$(MAGENTA)"
	 @echo "███╗	███╗██╗███╗	██╗██╗███████╗██╗  ██╗███████╗██╗	  ██╗	  "
	 @echo "████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║	  ██║	  "
	 @echo "██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║	  ██║	  "
	 @echo "██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║	  ██║	  "
	 @echo "██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗"
	 @echo "╚═╝	  ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝"
	 @echo "$(RESET)"

$(LIBFT_LIB):
	 @echo "$(GREEN)$(LEAF) Building libft...$(RESET)"
	 @make -C $(LIBFT_DIR) --no-print-directory
	 @echo "$(GREEN)$(CHECK_MARK) libft is ready!$(RESET)"

OBJ_DIR = obj

$(OBJ_DIR)/%.o: %.c
	 @mkdir -p $(dir $@)
	 @echo "$(BLUE)$(WRENCH) Compiling $(notdir $<)...$(RESET)"
	 @$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	 @echo "$(RED)$(BROOM) Cleaning object files...$(RESET)"
	 @$(RM) $(OBJS)
	 @rm -rf obj
	 @make -C $(LIBFT_DIR) clean --no-print-directory
	 @echo "$(RED)$(CHECK_MARK) Object files cleaned!$(RESET)"

fclean:	  clean
	 @echo "$(RED)$(FIRE) Full clean...$(RESET)"
	 @$(RM) $(NAME)
	 @make -C $(LIBFT_DIR) fclean --no-print-directory
	 @echo "$(RED)$(CHECK_MARK) Everything is spotless!$(RESET)"

run:	all
	@echo Have fun!
	./$(NAME)

re:			fclean all

.PHONY:	  all clean fclean re
