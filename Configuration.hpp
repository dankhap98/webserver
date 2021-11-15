#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"

class Configuration
{
    private:
        std::string path;
        std::vector<ConfigServer> servers;

        Configuration(const Configuration& c);
        Configuration & operator=(const Configuration& c);

        ConfigServer    loadServerConfig(std::ifstream& ifs);
        ConfigLocation    loadLocationConfig(std::ifstream& ifs);

        void    trim_line(std::string& line);
        void    parse_words(std::string line, std::vector<std::string>& words);

    public:
        Configuration();
        Configuration(std::string path);
        ~Configuration();

        void    setPath(const std::string& path);
        void    loadConfig();

        class NoFileOpenException: public std::exception
        {
            public:
                virtual const   char* what() const throw()
                {
                    return  ("Unable to open config file!");
                }
        };

        std::vector<ConfigServer>   getServers();
};

#endif