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
    else if (name == "index")
        this->addIndex(value);
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
}

void    ConfigServer::addAllowMethod(std::string meth)
{
    this->config[0].allow_methods.push_back(meth);
}

void    ConfigServer::addIndex(std::string ind_file)
{
    this->config[0].index.push_back(ind_file);
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
        if (loc_url[0] == '*' && url != "/")
        {
            
            loc_url = loc_url.substr(1, loc_url.size() - 1);
            if (std::count(url.begin(), url.end(), '/') == 1 && url.size() > loc_url.size())
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
    {
        std::map<std::string, std::string> props = cl.getProps();
        std::map<std::string, std::string>::iterator t = props.find("alias");
        if (t == props.end())
            root += cl.getUrl();
        else
            root += cl.getProperty("alias");
    }
    else
    {
        std::string root_prop = cl.getProperty("root");
        std::map<std::string, std::string> props = cl.getProps();
        std::map<std::string, std::string>::iterator t = props.find("alias");
        if (t != props.end())
            root += cl.getProperty("alias");
        if (root_prop[0] == '.')
            root = cl.getProperty("root");
        else if (root_prop[0] == '/')
            root += root_prop;
        else
            root = "./" + cl.getProperty("root");
    }
    return root;
}

std::string     ConfigServer::getRootWithIndex(std::vector<std::string> index, std::string root)
{
    std::vector<std::string>::iterator bg = index.begin();
    std::ofstream   file;
    while (bg != index.end())
    {
        file.open((root + "/" + (*bg)).c_str(),  std::ifstream::in);
        if (file.is_open())
        {
            file.close();
            return root + "/" + (*bg);
        }
        ++bg;
    }
    return root;
}

std::string     ConfigServer::getReturnUrl(ConfigLocation cl)
{
    std::map<std::string, std::string> props = cl.getProps();
    std::map<std::string, std::string>::iterator t = props.find("return");
    std::string empt = "";
    if (t != props.end())
        return (*t).second;
    return empt;
}

std::string   ConfigServer::getRedirect(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string ret = "";
    
    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    if (cl.getUrl().size() > 0)
    {
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        ret = this->getReturnUrl(cl);
        if (sub_cl.getUrl().size() > 0)
            ret = this->getReturnUrl(sub_cl);
    }
    return ret;
}


bool    ConfigServer::isRedirect(std::string host, std::string url)
{
    std::string ret = this->getRedirect(host, url);
    if (ret.size() == 0)
        return (0);
    return (1);
}

std::string     ConfigServer::getRootPath(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string root = conf.props["root"];
    std::string ret = "";
    std::ofstream   file;
    std::vector<std::string> index = conf.index;
    url = url.substr(0, url.find_first_of("?"));
    
    if (root.size() == 0)
        root = "./";
    
    if (url == "/")
        return  this->getRootWithIndex(index, root);
        
    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    root = this->getRootFromLocation(cl, root);
    if (cl.getUrl().size() > 0)
    {
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        root = this->getRootFromLocation(sub_cl, root);
        ret = this->getReturnUrl(cl);
        if (cl.getIndex().size() > 0)
            index = cl.getIndex();
        if (sub_cl.getUrl().size() > 0)
        {
            url = url.substr(sub_cl.getUrl().size(), url.size() - sub_cl.getUrl().size());
            if (sub_cl.getIndex().size() > 0)
                index = sub_cl.getIndex();
            ret = this->getReturnUrl(sub_cl);
        }
    }
    if (root[root.size() - 1] == '/')
        url = url.substr(1, url.size() - 1);
    if (ret.size() > 0)
        return root + ret;
    file.open((root + url).c_str(),  std::ifstream::in);
    if (file.is_open())
    {
        file.close();
        return root + url;
    }
    else
    {
        root += url;
        return  this->getRootWithIndex(index, root);
        /*std::vector<std::string>::iterator bg = index.begin();
        while (bg != index.end())
        {
            file.open((root + "/" + (*bg)).c_str(),  std::ifstream::in);
            if (file.is_open())
            {
                file.close();
                return root + "/" + (*bg);
            }
            ++bg;
        }*/
    }

    //return root;
}


bool    ConfigServer::getAutoIndex(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string aindex = conf.props["autoindex"];
    bool    is_on = true;

    if (aindex.size() == 0 || aindex == "off")
        is_on = false;
    
     if (url == "/")
        return is_on;


    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    if (cl.getUrl().size() > 0)
    {
        std::map<std::string, std::string> props = cl.getProps();
        std::map<std::string, std::string>::iterator t = props.find("autoindex");
        if (t != props.end())
        {
            if ((*t).second == "on")
                is_on = true;
            else
                is_on = false;
        }
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        if (sub_cl.getUrl().size() > 0)
        {
            std::map<std::string, std::string> s_props = sub_cl.getProps();
            std::map<std::string, std::string>::iterator s_t = s_props.find("autoindex");
            if (s_t != s_props.end())
            {
                if ((*s_t).second == "on")
                    is_on = true;
                else
                    is_on = false;
            }
        }
    }
    return is_on;
}

std::string    ConfigServer::getBufferSize(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string bsize = conf.props["client_body_buffer_size"];

    if (bsize.size() == 0)
        bsize = "8000";
    
     if (url == "/")
        return bsize;

    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    if (cl.getUrl().size() > 0)
    {
        std::map<std::string, std::string> props = cl.getProps();
        std::map<std::string, std::string>::iterator t = props.find("client_body_buffer_size");
        if (t != props.end())
            bsize = (*t).second;
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        if (sub_cl.getUrl().size() > 0)
        {
            std::map<std::string, std::string> s_props = sub_cl.getProps();
            std::map<std::string, std::string>::iterator s_t = s_props.find("client_body_buffer_size");
            if (s_t != s_props.end())
                bsize = (*t).second;
        }
    }
    return bsize;
}

std::vector<std::string>    ConfigServer::getAllowMethodsForUrl(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::vector<std::string> am = conf.allow_methods;
    
    if (url == "/")
        return am;


    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    if (cl.getUrl().size() > 0)
    {
        if (cl.getAllowMethods().size() > 0)
            am = cl.getAllowMethods();
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        if (sub_cl.getUrl().size() > 0)
        {
            if (sub_cl.getAllowMethods().size() > 0)
                am = sub_cl.getAllowMethods();
        }
    }
    return am;
}

std::string   ConfigServer::getCGIPath(std::string host, std::string url)
{
    t_server_config conf  = this->getConfigByName(host);
    std::string cpath = "";
    
    ConfigLocation cl = this->getConfigLocationByUrl(conf, url);
    if (cl.getUrl().size() > 0)
    {
        url = url.substr(cl.getUrl().size(), url.size() - cl.getUrl().size());
        std::map<std::string, std::string> props = cl.getProps();
        std::map<std::string, std::string>::iterator t = props.find("cgi_pass");
        std::cout << cl.getUrl() << " " << cpath;
        if (t != props.end())
            cpath = (*t).second;

        ConfigLocation sub_cl = cl.getConfigSubLocationByUrl(url);
        if (sub_cl.getUrl().size() > 0)
        {
            std::map<std::string, std::string> s_props = cl.getProps();
            std::map<std::string, std::string>::iterator s_t = s_props.find("cgi_pass");
            if (s_t != s_props.end())
                cpath = (*s_t).second;
        }
    }
    return cpath;
}