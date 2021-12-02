#include "ConfigServer.hpp"

ConfigServer::ConfigServer(): props(), locations(), ip_address(), port() {}

ConfigServer::~ConfigServer() {}

ConfigServer::ConfigServer(const ConfigServer& cs)
{
    *this = cs;
}

ConfigServer & ConfigServer::operator=(const ConfigServer& cs)
{
    this->props.clear();
    this->locations.clear();

    this->props = (std::map<std::string, std::string>(cs.props)); //std::move(cs.props);
    this->locations = cs.locations;
    this->ip_address = cs.ip_address;
    this->port = cs.port;

    return *this;
}

ConfigServer::props_type  ConfigServer::getProps()
{
    return this->props;
}

ConfigServer::loc_type  ConfigServer::getLocations()
{
    return this->locations;
}

void    ConfigServer::setProperty(std::string name, std::string value)
{
    this->props[name] = value;
}

std::string ConfigServer::getProperty(std::string name)
{
    return this->props[name]; //check if not exist!
}

void    ConfigServer::setLocation(ConfigLocation cl)
{
    this->locations.push_back(cl);
}

void    ConfigServer::setAddress(std::string addr)
{
    this->ip_address = addr;
}

void    ConfigServer::setPort(int server_port)
{
    this->port = server_port;
}

std::string ConfigServer::getAddress() const
{
    return this->ip_address;
}

int ConfigServer::getPort() const
{
    return this->port;
}