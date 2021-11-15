#include "ConfigServer.hpp"

ConfigServer::ConfigServer(): props() {}

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