# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/19 16:21:48 by luinasci          #+#    #+#              #
#    Updated: 2025/03/26 18:55:39 by luinasci         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= minishell
LIBFT_DIR	= libft
LIBFT_LIB	= $(LIBFT_DIR)/libft.a

SRC_PATH	= src/
SRCS		= $(addprefix $(SRC_PATH), \
			  builtin.c \
			  input.c \
			  main.c \
			  parser.c \
			  signals.c \
			  )

UTILS_PATH	= utils/
UTILS_SRCS	= $(addprefix $(UTILS_PATH), \
			  utils.c \
			  )

OBJS		= $(SRCS:.c=.o) $(UTILS_SRCS:.c=.o)
CC			= gcc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -Iincludes -I$(LIBFT_DIR)
RM			= rm -f

all:		$(NAME)

$(NAME):	$(OBJS) $(LIBFT_LIB)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -L$(LIBFT_DIR) -lft -lreadline

$(LIBFT_LIB):
	@make -C $(LIBFT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@make -C $(LIBFT_DIR) clean

fclean:		clean
	$(RM) $(NAME)
	@make -C $(LIBFT_DIR) fclean

re:			fclean all

.PHONY:		all clean fclean re
