all:
	clang++ -Wall -Wextra -Werror main_parse_and_start.cpp ConfigLocation.cpp ConfigServer.cpp Configuration.cpp Webserver.cpp Webserver.hpp ConfigLocation.hpp ConfigServer.hpp Configuration.hpp
