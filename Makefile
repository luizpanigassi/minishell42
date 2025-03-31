# **************************************************************************** #
#																										#
#																			:::		::::::::	 #
#	 Makefile														 :+:		:+:	 :+:	 #
#																	  +:+ +:+			+:+		#
#	 By: luinasci <luinasci@student.42.fr>			 +#+  +:+		 +#+			#
#																 +#+#+#+#+#+	+#+				#
#	 Created: 2025/03/19 16:21:48 by luinasci			 #+#	 #+#				  #
#	 Updated: 2025/03/31 16:14:49 by luinasci			###	########.fr		  #
#																										#
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
				  builtin.c \
				  input.c \
				  main.c \
				  parser.c \
				  signals.c \
				  )

UTILS_PATH  = utils/
UTILS_SRCS  = $(addprefix $(UTILS_PATH), \
				  utils.c \
				  )

OBJS		  = $(SRCS:.c=.o) $(UTILS_SRCS:.c=.o)
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

%.o: %.c
	 @echo "$(BLUE)$(WRENCH) Compiling $(notdir $<)...$(RESET)"
	 @$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	 @echo "$(RED)$(BROOM) Cleaning object files...$(RESET)"
	 @$(RM) $(OBJS)
	 @make -C $(LIBFT_DIR) clean --no-print-directory
	 @echo "$(RED)$(CHECK_MARK) Object files cleaned!$(RESET)"

fclean:	  clean
	 @echo "$(RED)$(FIRE) Full clean...$(RESET)"
	 @$(RM) $(NAME)
	 @make -C $(LIBFT_DIR) fclean --no-print-directory
	 @echo "$(RED)$(CHECK_MARK) Everything is spotless!$(RESET)"

re:			fclean all

.PHONY:	  all clean fclean re
