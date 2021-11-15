#ifndef CONFIGLOCATION_HPP
# define CONFIGLOCATION_HPP

#include <iostream>
#include <map>
#include <vector>

class ConfigLocation
{
    private:
        std::string url;
        std::map<std::string, std::string> props;
        std::vector<ConfigLocation> sub_locations;
   
    public:
        typedef  std::map<std::string, std::string> props_type;
        typedef  std::vector<ConfigLocation> sub_loc_type;

        ConfigLocation();
        ConfigLocation(std::string path);
        ~ConfigLocation();

        ConfigLocation(const ConfigLocation& cs);
        ConfigLocation & operator=(const ConfigLocation& cs);

        props_type  getProps();
        std::string getUrl();
        void        setUrl(const std::string& path, int del_end);
        sub_loc_type    get_SubLocations();
        void     setProperty(std::string name, std::string value);
        std::string getProperty(std::string);
        void     setLocation(ConfigLocation cl);
};

#endif