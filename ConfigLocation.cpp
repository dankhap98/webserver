#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation(): url(""), props(), sub_locations() {}

ConfigLocation::ConfigLocation(std::string path): url(path), props(), sub_locations() {}

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