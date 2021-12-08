#include "../includes/ConfigServer.hpp"

ConfigServer::ConfigServer(): props(), error_pages(), locations(), allow_methods(), ip_address(), port()
{
    this->error_pages[404] = "/";
    this->error_pages[403] = "/";
    this->error_pages[500] = "/";
}

ConfigServer::~ConfigServer() {}

ConfigServer::ConfigServer(const ConfigServer& cs)
{
    *this = cs;
}

ConfigServer & ConfigServer::operator=(const ConfigServer& cs)
{
    this->props.clear();
    this->locations.clear();
    this->error_pages.clear();
    this->allow_methods.clear();

    this->props = (std::map<std::string, std::string>(cs.props)); //std::move(cs.props);
    this->locations = cs.locations;
    this->ip_address = cs.ip_address;
    this->port = cs.port;
    this->error_pages = cs.error_pages;
    this->allow_methods = cs.allow_methods;

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

ConfigServer::error_page_type   ConfigServer::getErrorPages()
{
    return this->error_pages;
}

std::vector<std::string>        ConfigServer::getAllowMethods()
{
    return  this->allow_methods;
}

void    ConfigServer::setProperty(std::string name, std::string value)
{
    if (name == "allow_methods")
        this->addAllowMethod(value);
    else
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

void    ConfigServer::setErrorPage(int code, std::string url)
{
    this->error_pages[code] = url;
}

void    ConfigServer::addAllowMethod(std::string meth)
{
    this->allow_methods.push_back(meth);
}

std::string ConfigServer::getAddress() const
{
    return this->ip_address;
}

int ConfigServer::getPort() const
{
    return this->port;
}