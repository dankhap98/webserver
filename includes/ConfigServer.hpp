#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

#include <vector>
#include <iostream>
#include <map>
#include "ConfigLocation.hpp"

typedef struct  s_server_config
{
    std::map<std::string, std::string> props;
    std::map<int, std::string> error_pages;
    std::vector<ConfigLocation> locations;
    std::vector<std::string> allow_methods;
}   t_server_config;

class ConfigServer
{
    private:
        std::vector<t_server_config> config;
        std::string ip_address;
        int port;
    
    public:
        typedef std::map<std::string, std::string> props_type;
        typedef std::map<int, std::string> error_page_type;
        typedef std::vector<ConfigLocation> loc_type;
        ConfigServer();
        ~ConfigServer();

        ConfigServer(const ConfigServer& cs);
        ConfigServer & operator=(const ConfigServer& cs);
        //props_type getProps();
        void     setProperty(std::string name, std::string value);
        /*std::string     getProperty(std::string name);
        loc_type getLocations();
        error_page_type getErrorPages();
        std::vector<std::string>    getAllowMethods();*/
        void     setLocation(ConfigLocation cl);
        void     setAddress(std::string addr);
        void     setPort(int server_port);
        void     setErrorPage(int code, std::string url);
        void     addAllowMethod(std::string meth);
        std::string getAddress() const;
        int     getPort() const;
        std::vector<t_server_config>    getConfig();
        void        addConfig(t_server_config conf);
        t_server_config getConfigByName(std::string host);
        ConfigLocation  getConfigLocationByUrl(t_server_config conf, std::string url);
};

#endif
