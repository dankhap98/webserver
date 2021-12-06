#include "../includes/Configuration.hpp"

Configuration::Configuration(): path(""), servers() {}

Configuration::Configuration(std::string filepath): path(filepath), servers() {}

Configuration::~Configuration() {}

void    Configuration::setPath(const std::string& filepath) {
    this->path = filepath;
}

void    Configuration::loadConfig()
{
    std::ifstream conf_to_read;
    std::string line;
    std::vector<std::string> words;
    int multirow_flag = 0;

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
                ConfigServer new_server = this->loadServerConfig(conf_to_read);
                this->setServerAddress(&new_server);
                this->servers.push_back(new_server);
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

ConfigServer    Configuration::loadServerConfig(std::ifstream& conf)
{
    ConfigServer *cs = new ConfigServer();
    bool    server_parse_end = false;
    std::string line;
    std::string url;
    std::vector<std::string> words;
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
                    new_loc.setUrl(url, 0);
                    url.clear();
                }
                else
                    new_loc.setUrl(words[1], 1);
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
                if (words.size() == 2)
                    cs->setProperty(words[0], words[1]);
                else if(words.size() == 1 && words[0] != "}")
                    cs->setProperty(words[0], "");
                else if(words[0] == "}")
                    server_parse_end = true;
            }
            words.clear();
        }
        line.clear();
    }
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
                    new_loc.setUrl(url, 0);
                    url.clear();
                }
                new_loc.setUrl(words[1], 1);
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
        words.push_back(line.substr(start, pos));
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

void    Configuration::setServerAddress(ConfigServer *cs)
{
    int pos = 0;
    std::map<std::string, std::string> props = (*cs).getProps();

    if (props.find("listen") != props.end())
    {
        //std::cout << "--" << props["listen"] << "--\n";
        pos = props["listen"].find_first_of(":", pos);
        //std::cout << "--" << pos << "--\n";
        std::string addr = props["listen"].substr(0, pos);
        //std::cout << "--" << addr << "--\n";
        int port = std::atoi((props["listen"].substr(pos + 1, props["listen"].size())).c_str());
        //std::cout << "--" << port << "--\n";
        //need to check if addr or port is null
        //need to do many key = listen params exist 
        (*cs).setAddress(addr);
        //std::cout << "--" << cs.getAddress() << "--\n";
        (*cs).setPort(port);
        //std::cout << "--" << cs.getPort() << "--\n";
    }

    
}
