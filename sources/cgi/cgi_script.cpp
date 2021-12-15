//
// Created by Stoops Ysilla on 12/10/21.
//

#include "../../includes/server.hpp"

int main1()
{
	int ac;
	char **av;
    if (ac > 1) {
        std::ofstream		file;
        std::stringstream	buffer;
        std::string         message;
        size_t  position = 0;
        size_t	av_size = sizeof(av[1]);

        file.open("../../test.html", std::ifstream::in | std::fstream::out | std::fstream::binary);
        if (file.is_open() == false)
            return (-1);
        buffer << file.rdbuf();
        message = "\n<li>" + std::string(av[1]) + "</li>";
        position = buffer.str().size() - 88;
        file.seekp(position);
        file.write(message.c_str(), message.size());
        file.close();
        return 0;
    }
}