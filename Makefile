all:
	clang++ -Wall -Wextra -Werror sources/main_parse_and_start.cpp sources/ConfigLocation.cpp sources/ConfigServer.cpp sources/Configuration.cpp sources/Webserver.cpp includes/Webserver.hpp includes/ConfigLocation.hpp includes/ConfigServer.hpp includes/Configuration.hpp
