#include "../includes/ConfigServer.hpp"

ConfigServer::ConfigServer(): config(), ip_address(), port()
{
    t_server_config conf;
    conf.error_pages[404] = "/";
    conf.error_pages[403] = "/";
    conf.error_pages[500] = "/";
    this->config.push_back(conf);
}

ConfigServer::~ConfigServer() {}

ConfigServer::ConfigServer(const ConfigServer& cs)
{
    *this = cs;
}

ConfigServer & ConfigServer::operator=(const ConfigServer& cs)
{
    /*this->props.clear();
    this->locations.clear();
    this->error_pages.clear();
    this->allow_methods.clear();

    this->props = (std::map<std::string, std::string>(cs.props)); //std::move(cs.props);
    this->locations = cs.locations;*/
    this->config.clear();

    this->ip_address = cs.ip_address;
    this->port = cs.port;
    this->config = cs.config;
    /*this->error_pages = cs.error_pages;
    this->allow_methods = cs.allow_methods;*/

    return *this;
}

/*ConfigServer::props_type  ConfigServer::getProps()
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
}*/

void    ConfigServer::setProperty(std::string name, std::string value)
{
    if (name == "allow_methods")
        this->addAllowMethod(value);
    else
        this->config[0].props[name] = value;
        //this->props[name] = value;
}

/*std::string ConfigServer::getProperty(std::string name)
{
    return this->props[name]; //check if not exist!
}*/

void    ConfigServer::setLocation(ConfigLocation cl)
{
    this->config[0].locations.push_back(cl);
    //this->locations.push_back(cl);
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
    this->config[0].error_pages[code] = url;
    //this->error_pages[code] = url;
}

void    ConfigServer::addAllowMethod(std::string meth)
{
    this->config[0].allow_methods.push_back(meth);
    //this->allow_methods.push_back(meth);
}

std::string ConfigServer::getAddress() const
{
    return this->ip_address;
}

int ConfigServer::getPort() const
{
    return this->port;
}

std::vector<t_server_config>  ConfigServer::getConfig()
{
    return this->config;
}

void    ConfigServer::addConfig(t_server_config conf)
{
    this->config.push_back(conf);
}
