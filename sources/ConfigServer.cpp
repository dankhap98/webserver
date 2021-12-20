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
    this->config[0].error_pages[code] = this->getErrorUrl(code, url);
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

t_server_config ConfigServer::getConfigByName(std::string host)
{
    if (this->config.size() == 1)
        return this->config[0];
    std::vector<t_server_config>::iterator  bg = this->config.begin();
    int pos = host.find(":");
    std::string hostname = host.substr(0, pos);

    while (bg != this->config.end())
    {
        if (hostname == (*bg).props["server_name"])
            return (*bg);
        ++bg;
    }
    return this->config[0];
}

ConfigLocation  ConfigServer::getConfigLocationByUrl(t_server_config conf, std::string url)
{
    std::vector<ConfigLocation>::iterator bg = conf.locations.begin();

    while (bg != conf.locations.end())
    {
        std::string loc_url = (*bg).getUrl();
        if (loc_url == url || std::strncmp(loc_url.c_str(), url.c_str(), loc_url.size()) == 0)
            return (*bg);
        if (loc_url[0] == '*')
        {
            //TD not 0 index, must be i - index
            loc_url = loc_url.substr(1, loc_url.size() - 1);
            if (std::count(url.begin(), url.end(), '/') == 1)
            {
                std::string tmp_url = url.substr(url.size() - loc_url.size(), loc_url.size());
                if (loc_url == tmp_url)
                    return  (*bg);
            }
        }
        //check regex *.bla
        ++bg;
    }
    return  ConfigLocation(); //to think
}

uint32_t    ConfigServer::getIpAddressInt()
{
    int pos = 0;
    int start = 0;
    int res = 0;
    int d = 24;
    int bt = 0;
    std::string ip = this->ip_address;
    std::string::iterator beg = ip.begin();

    if (std::count(ip.begin(), ip.end(), '.') != 3)
        return (0);

    while (beg != ip.end())
    {
        pos = ip.find_first_of(".", pos);
        if (pos == -1)
            pos = ip.size();
        bt = std::atoi((ip.substr(start, pos - start)).c_str());
        if (bt > 256)
            return (0);
        if (d == 0)
            res += bt;
        else
            res += (bt << d);

        if ((size_t)pos == ip.size())
            beg = ip.end();
        else
            beg += pos + 1;
        pos++;
        start = pos;
        d-=8;
    }

    return (res);
}

std::string     ConfigServer::getErrorUrl(int code, std::string url)
{
    int q = code % 10;
    int d = ((code - q) / 10) % 10;
    int h = code / 100;
    int pos = 0;
    if (std::count(url.begin(), url.end(), 'x') != 0)
    {
        pos = url.find_first_of("x", 0);
        if (url[pos + 1] == '.' && url[pos - 1] == (std::to_string(d))[0] && url[pos - 2] == (std::to_string(h))[0])
            url[pos] =  (std::to_string(q))[0];
    }
    return url;
}

std::string     ConfigServer::getRootFromLocation(ConfigLocation cl, std::string root)
{
    if (cl.getUrl().size() == 0)
        return root;
    else if (cl.getProperty("root").size() == 0)
        root += cl.getUrl();
    else
        root = cl.getProperty("root");
    return root;
}

std::string     ConfigServer::getRootPath(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string root = conf.props["root"];
    int lt_pos =  url.find_last_of("/");
    std::string url_last = url.substr(lt_pos, url.size() - lt_pos);
    
    if (root.size() == 0)
        root = "./";
   
    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    root = this->getRootFromLocation(cl, root);
    ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
    root = this->getRootFromLocation(sub_cl, root);
    
    std::string ind =  root + url_last; //is dir? if not search index in root path TD

    return root;
}