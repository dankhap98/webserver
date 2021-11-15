#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation(): props(), sub_locations(), url("") {}

ConfigLocation::ConfigLocation(std::string path): props(), sub_locations(), url(path) {}

ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation(const ConfigLocation& cl)
{
    *this = cl;
}

ConfigLocation & ConfigLocation::operator=(const ConfigLocation& cl)
{
    this->props.clear();
    this->sub_locations.clear();

    this->props = std::move(cl.props);
    this->sub_locations = cl.sub_locations;
    this->url = cl.url;

    return *this;
}

std::string     ConfigLocation::getUrl()
{
    return this->url;
}

void     ConfigLocation::setUrl(const std::string& path, int del_end)
{
    this->url = path;
    if (del_end)
        this->url.pop_back();
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