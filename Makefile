NAME		=	webserv

CC			=	clang++

OBJS_DIR	=	.obj

HEADERS		=	ConfigLocation.hpp ConfigServer.hpp Configuration.hpp Request.hpp Response.hpp server.hpp Webserver.hpp CGIClass.hpp Autoindex.hpp

HEADER	=		$(addprefix includes/, $(HEADERS))

CFLAGS	=   	-Wall -Wextra -Werror -std=c++98

SRC		=		cgi/CGIClass ConfigLocation ConfigServer Configuration main_parse_and_start Request Response server Webserver Autoindex

SRSUF	=		$(addsuffix .cpp, $(SRC))

SRCS	=		$(addprefix sources/, $(SRSUF))

OBJS	=		$(addprefix $(OBJS_DIR)/, $(patsubst %.cpp, %.o, $(SRCS)))


$(OBJS_DIR)/%.o:	%.cpp $(HEADER)
					@test -d $(OBJS_DIR) || mkdir $(OBJS_DIR)
					@test -d $(OBJS_DIR)/sources || mkdir $(OBJS_DIR)/sources
					@test -d $(OBJS_DIR)/sources/cgi || mkdir $(OBJS_DIR)/sources/cgi
					@printf "$(GREEN)$(BOLD)Compilation $(UNDER_LINE)$(YELLOW)$<$(NO_COLOR)  $(BOLD)–– $(RED)[KO]        $(NO_COLOR)\r"
					@$(CC) $(CFLAGS) -c $< -o $@
					@printf "$(GREEN)$(BOLD)Compilation $(UNDER_LINE)$(YELLOW)$<$(NO_COLOR)  $(BOLD)–– $(GREEN)[OK]$(NO_COLOR)\n"

all:	$(NAME)

$(NAME): ${OBJS}
		$(CC) $(CFLAGS) $(OBJS) -o $@
		@printf "$(GREEN)$(BOLD)Compilation completed$(NO_COLOR)\n"

clean:
		@rm -f ${OBJS}
		@rm -f includes/*.gch
		@-rm -rf $(OBJS_DIR)
		@-rm -rf $(OBJS_DIR)/sources
		@-rm -rf $(OBJS_DIR)/sources/cgi

fclean:	clean
		@rm -f ${NAME}
		@printf "$(UNDER_LINE)$(BOLD)$(NAME):$(NO_COLOR) $(CYAN)All files deleted$(NO_COLOR)\n"

re:		fclean all

run:	all
		./$(NAME)

.PHONY:	all clean fclean re run

################
##   COLORS   ##
################

BOLD	 	= \033[1m
NO_COLOR 	= \033[0m
UNDER_LINE	= \033[4m

BLACK	 	= \033[0;30m
RED 	 	= \033[0;31m
GREEN 	 	= \033[0;32m
BROWN 	 	= \033[0;33m
BLUE 		= \033[0;34m
PURPLE 	 	= \033[0;35m
CYAN 	 	= \033[0;36m
LIGHT_GRAY	= \033[0;37m

DARK_GRAY	= \033[1;30m
LIGHT_RED	= \033[1;31m
LIGHT_GREEN	= \033[1;32m
YELLOW 		= \033[1;33m
LIGHT_BLUE	= \033[1;34m
LIGHT_PURPLE= \033[1;35m
LIGHT_CYAN	= \033[1;36m
WHITE 		= \033[1;37m
