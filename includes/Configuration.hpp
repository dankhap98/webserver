#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <vector>
//#include <stdexcept>
#include <exception>
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"

typedef struct  s_misconfiguration
{
    class NoServerException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("No server configuration!");
            }
    };

    class NoFileOpenException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("Unable to open config file!");
            }
    };

    class InvalidFileNameException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("Invalid file name!");
            }
    };

    class InvalidListenException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("Invalid listen property!");
            }
    };

    class InvalidBodyLimitException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("Invalid client body size property!");
            }
    };

    class NoListenException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("No listen property!");
            }
    };

    class NoIndexException: public std::exception
    {
        public:
            virtual const   char* what() const throw()
            {
                return  ("No index property!");
            }
    };

}   t_misconfiguration;

class Configuration
{
    private:
        std::string path;
        std::vector<ConfigServer> servers;

        Configuration(const Configuration& c);
        Configuration & operator=(const Configuration& c);

        ConfigServer    loadServerConfig(std::ifstream& ifs, int *is_exist);
        ConfigLocation    loadLocationConfig(std::ifstream& ifs);

        bool    valid_file_name();
        void    trim_line(std::string& line);
        void    parse_words(std::string line, std::vector<std::string>& words);
        void    parseServerAddress(ConfigServer *cs);
        void    parse_long_prop(ConfigServer * cs, std::vector<std::string>& words);
        void    parse_long_prop(ConfigLocation * cl, std::vector<std::string>& words);
        ConfigServer    *server_exist(std::vector<std::string>& words);
        void    parse_listen(ConfigServer &cs, std::vector<std::string> listen);

    public:
        Configuration();
        Configuration(std::string path);
        ~Configuration();

        void    setPath(const std::string& path);
        void    loadConfig();

        bool    valid_ip(std::string ip);
        bool    valid_port(std::string port);
        bool    valid_size(std::string lim_size);

        std::vector<ConfigServer>   getServers();
};

#endif
