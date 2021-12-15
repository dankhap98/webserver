#include "../includes/Configuration.hpp"

Configuration::Configuration(): path(""), servers() {}

Configuration::Configuration(std::string filepath): path(filepath), servers() {}

Configuration::~Configuration() {
}

void    Configuration::setPath(const std::string& filepath) {
    this->path = filepath;
}

void    Configuration::loadConfig()
{
    std::ifstream conf_to_read;
    std::string line;
    std::vector<std::string> words;
    int multirow_flag = 0;
    int is_exist = 0;

    conf_to_read.open(this->path);
    if (!conf_to_read.is_open())
        throw   NoFileOpenException();
    while (std::getline(conf_to_read, line))
    {
        this->trim_line(line);
        
        if (line[0] != '#' && line.size() > 0)
        {
            this->parse_words(line, words);
            if ((words[0] == "server" && words[1] == "{") || (words[0] == "{" && multirow_flag)) // "{" may be in next row
            {
                ConfigServer new_server = this->loadServerConfig(conf_to_read, &is_exist);
                if (!is_exist)
                {
                    this->parseServerAddress(&new_server);
                    this->servers.push_back(new_server);
                }
                is_exist = 0;
                multirow_flag = 0;
            }
            else if (words[0] == "server")
                multirow_flag = 1;
            words.clear();
        }
        line.clear();
    }
    conf_to_read.close();
}

ConfigServer    Configuration::loadServerConfig(std::ifstream& conf, int *is_exist)
{
    ConfigServer *cs = new ConfigServer();
    ConfigServer *existed = 0;
    bool    server_parse_end = false;
    std::string line;
    std::string url;
    std::vector<std::string> words;
    std::vector<std::string> listen;
    int multirow_flag = 0;


    while (server_parse_end == false)
    {
        std::getline(conf, line);
        this->trim_line(line);
        
        if (line[0] != '#' && line.size() > 0)
        {
            this->parse_words(line, words);
            if ((words[0] == "location" && words[2] == "{") || (words[0] == "{" && multirow_flag))
            {
                ConfigLocation new_loc = this->loadLocationConfig(conf);
                if (multirow_flag)
                {
                    new_loc.setUrl(url);
                    url.clear();
                }
                else
                    new_loc.setUrl(words[1]);
                cs->setLocation(new_loc);
                multirow_flag = 0;
            }
            else if (words[0] == "location")
            {
                multirow_flag = 1;
                url = words[1];
            }
            else
            {
                if (words[0] == "listen")
                {
                    existed = server_exist(words);
                    if (existed)
                        *is_exist = 1;
                    listen.push_back(words[1]);
                }
                else if (words.size() == 2)
                    cs->setProperty(words[0], words[1]);
                else if(words.size() == 1 && words[0] != "}")
                    cs->setProperty(words[0], "");
                else if(words[0] == "}")
                    server_parse_end = true;
                else if (words.size() > 2)
                    this->parse_long_prop(cs, words);
            }
            words.clear();
        }
        line.clear();
    }
    this->parse_listen(*cs, listen);
    if (*is_exist)
        (*existed).addConfig((cs->getConfig())[0]);
    return *cs;
}

ConfigLocation  Configuration::loadLocationConfig(std::ifstream& conf)
{
    ConfigLocation *cl = new ConfigLocation();
    bool    loc_parse_end = false;
    std::string line;
    std::string url;
    std::vector<std::string> words;
    int multirow_flag = 0;

    while (loc_parse_end == false)
    {
        std::getline(conf, line);
        this->trim_line(line);
        
        if (line[0] != '#')
        {
            this->parse_words(line, words);
            if ((words[0] == "location" && words[2] == "{") || (words[0] == "{" && multirow_flag))
            {
                ConfigLocation new_loc = this->loadLocationConfig(conf);
                if (multirow_flag)
                {
                    new_loc.setUrl(url);
                    url.clear();
                }
                new_loc.setUrl(words[1]);
                cl->setLocation(new_loc);
                multirow_flag = 0;
            }
            else if(words[0] == "location")
            {
                multirow_flag = 1;
                url = words[1];
            }
            else
            {
                if (words.size() == 2)
                    cl->setProperty(words[0], words[1]);
                else if(words.size() == 1 && words[0] != "}")
                    cl->setProperty(words[0], "");
                else if(words[0] == "}")
                    loc_parse_end = true;
                else if(words.size() > 2)
                    this->parse_long_prop(cl, words);

            }
            words.clear();
        }
        line.clear();
    }
    return *cl;
}

void    Configuration::trim_line(std::string& line)
{
    const std::string space_chars = " \t\n\r\f\v";

    line.erase(0, line.find_first_not_of(space_chars));
    line.erase(line.find_last_not_of(space_chars) + 1);
}

void    Configuration::parse_words(std::string line, std::vector<std::string>& words)
{
    std::string::iterator beg = line.begin();
    int pos = 0;
    int start = 0;
    const std::string space_chars = " \t\n\r\f\v";

    while (beg != line.end())
    {
        pos = line.find_first_of(space_chars, pos);
        if (pos == -1)
            pos = line.size();
        words.push_back(line.substr(start, pos - start));
        if ((size_t)pos == line.size())
            beg = line.end();
        else
            beg+=pos + 1;
        pos++;
        start = pos;
    }
}

std::vector<ConfigServer>   Configuration::getServers()
{
    return this->servers;
}

void    Configuration::parseServerAddress(ConfigServer *cs)
{
    int pos = 0;
    std::map<std::string, std::string> props = (*cs).getConfig()[0].props;

    if (props.find("listen") != props.end())
    {
        int port = 0;
        std::string addr = "0.0.0.0";
        pos = props["listen"].find_first_of(":", pos);
        if ((size_t)pos == props["listen"].size())
            port = std::atoi(props["listen"].c_str());
        else
        {
            addr = props["listen"].substr(0, pos);
            port = std::atoi((props["listen"].substr(pos + 1, props["listen"].size() - pos - 1)).c_str());
        }
        //need to check if addr or port is null
        //need to do many key = listen params exist 
        (*cs).setAddress(addr);
        (*cs).setPort(port);
    }
}

void    Configuration::parse_long_prop(ConfigServer * cs, std::vector<std::string>& words)
{
    int len = words.size();

    if (words[0] == "error_page")
    {
        for (int i = 1; i < len - 1; i++)
        {
            cs->setErrorPage(std::atoi(words[i].c_str()), words[len -1]);
        }
    }
    else if(words[0] == "allow_methods")
    {
        for (int i = 1; i < len; i++)
        {
            cs->addAllowMethod(words[i]);
        }
    }
}

void    Configuration::parse_long_prop(ConfigLocation * cl, std::vector<std::string>& words)
{
    int len = words.size();

    if(words[0] == "allow_methods")
    {
        for (int i = 1; i < len; i++)
        {
            cl->addAllowMethod(words[i]);
        }
    }
}

ConfigServer    *Configuration::server_exist(std::vector<std::string>& words) 
{
    if (this->servers.size() == 0)
        return 0;
    std::vector<ConfigServer>::iterator bg = this->servers.begin();
    while (bg != this->servers.end())
    {
        std::vector<t_server_config> conf = (*bg).getConfig();
        if (conf[0].props["listen"] == words[1])
            return &(*bg);
        ++bg;
    }
    return (0);
}

void    Configuration::parse_listen(ConfigServer &cs, std::vector<std::string> listen)
{
    if (listen.size() == 0)
        return ;
        //error
    else if(listen.size() == 1)
         cs.setProperty("listen", listen[0]);
    else
    {
        cs.setProperty("listen", listen[0]);
        this->parseServerAddress(&cs);
        for (size_t i=1; i < listen.size(); i++)
        {
            ConfigServer *csnew = new ConfigServer(cs);
            csnew->setProperty("listen", listen[i]);
            this->parseServerAddress(csnew);
            csnew->setAddress(cs.getAddress());
            this->servers.push_back(*csnew);
        }
    }
}
