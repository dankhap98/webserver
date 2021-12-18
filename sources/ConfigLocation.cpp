#include "../includes/ConfigLocation.hpp"

ConfigLocation::ConfigLocation(): url(""), props(), sub_locations(), allow_methods() {}

ConfigLocation::ConfigLocation(std::string path): url(path), props(), sub_locations(), allow_methods() {}

ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation(const ConfigLocation& cl)
{
    *this = cl;
}

ConfigLocation & ConfigLocation::operator=(const ConfigLocation& cl)
{
    this->props.clear();
    this->sub_locations.clear();
    this->allow_methods.clear();

    this->props = std::move(cl.props);
    this->sub_locations = cl.sub_locations;
    this->url = cl.url;
    this->allow_methods = cl.allow_methods;

    return *this;
}

std::string     ConfigLocation::getUrl()
{
    return this->url;
}

std::vector<std::string>        ConfigLocation::getAllowMethods()
{
    return  this->allow_methods;
}

void     ConfigLocation::setUrl(const std::string& path)
{
    this->url = path;
}

ConfigLocation::props_type  ConfigLocation::getProps()
{
    return this->props;
}

ConfigLocation::sub_loc_type    ConfigLocation::get_SubLocations()
{
    return this->sub_locations;
}

void    ConfigLocation::setProperty(std::string name, std::string value)
{
    this->props[name] = value;
}

std::string ConfigLocation::getProperty(std::string name)
{
    return this->props[name]; //check if not exist!
}

void    ConfigLocation::setLocation(ConfigLocation cl)
{
    this->sub_locations.push_back(cl);
}

void    ConfigLocation::addAllowMethod(std::string meth)
{
    this->allow_methods.push_back(meth);
}

ConfigLocation  ConfigLocation::getConfigSubLocationByUrl(std::string url)
{
    std::string loc_url = this->getUrl();
    if (std::strncmp(loc_url.c_str(), url.c_str(), loc_url.size()) == 0)
    {
        std::string suburl = url.substr(loc_url.size(), url.size() - loc_url.size());
        std::vector<ConfigLocation>::iterator bg = this->sub_locations.begin();

        while (bg != this->sub_locations.end())
        {
            loc_url = (*bg).getUrl();
            if (loc_url == suburl || std::strncmp(loc_url.c_str(), suburl.c_str(), loc_url.size()) == 0)
                return (*bg);
            if (loc_url[0] == '*')
            {
                //TD not 0 index, must be i - index
                loc_url = loc_url.substr(1, loc_url.size() - 1);
                if (std::count(suburl.begin(), suburl.end(), '/') == 1)
                {
                    std::string tmp_url = suburl.substr(suburl.size() - loc_url.size(), loc_url.size());
                    if (loc_url == tmp_url)
                        return  (*bg);
                }
            }
            ++bg;
        }
    }
    return  ConfigLocation(); //to think
}