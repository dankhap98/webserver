all:
	clang++ -Wall -Wextra -Werror sources/*.cpp includes/*.hpp
	rm includes/*.gch