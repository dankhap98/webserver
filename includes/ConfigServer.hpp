#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

#include <vector>
#include <iostream>
#include <map>
#include "ConfigLocation.hpp"

class ConfigServer
{
    private:
        std::map<std::string, std::string> props;
        std::vector<ConfigLocation> locations;
        std::string ip_address;
        int port;
    
    public:
        typedef std::map<std::string, std::string> props_type;
        typedef std::vector<ConfigLocation> loc_type;
        ConfigServer();
        ~ConfigServer();

        ConfigServer(const ConfigServer& cs);
        ConfigServer & operator=(const ConfigServer& cs);
        props_type getProps();
        void     setProperty(std::string name, std::string value);
        std::string     getProperty(std::string name);
        loc_type getLocations();
        void     setLocation(ConfigLocation cl);
        void     setAddress(std::string addr);
        void     setPort(int server_port);
        std::string getAddress() const;
        int     getPort() const;
};

#endif