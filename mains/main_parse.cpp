#include "../includes/Configuration.hpp"
#include "../includes/ConfigServer.hpp"
#include "../includes/ConfigLocation.hpp"
#include "../includes/Webserver.hpp"

void    print_allow_meths(std::vector<std::string> meths)
{
    if (meths.size() > 0)
    {
        std::cout << "Allowed methods: \n";
        std::vector<std::string>::iterator bg = meths.begin();
        while (bg != meths.end())
        {
            std::cout << (*bg) << "\n";
            ++bg;
        }
    }
}

void   print_location_properties(std::vector<ConfigLocation> cl)
{
    std::vector<ConfigLocation>::iterator beg = cl.begin();

    while (beg != cl.end())
    {
        std::cout << "\nLocationConfig " << (*beg).getUrl() << "\n";
        std::map<std::string, std::string> props = (*beg).getProps();
        std::map<std::string, std::string>::iterator bg = props.begin();
        while (bg != props.end())
        {
            std::cout << (*bg).first << " : " << (*bg).second << "\n";
            ++bg;
        }
        print_allow_meths((*beg).getAllowMethods());
        print_location_properties((*beg).get_SubLocations());
        std::cout << "end location config\n\n";
        ++beg;
    }
}

void    print_error_pages(std::map<int, std::string> error_pages)
{
    std::map<int, std::string>::iterator   beg = error_pages.begin();
    std::cout << "ERROR_PAGES:\n";
    while (beg != error_pages.end())
    {
        std::cout << (*beg).first << ": " << (*beg).second << "\n";
        ++beg;
    }
}

int    main()
{
    Configuration *conf = new Configuration("./test2.conf");
    

    try{
        conf->loadConfig();


        std::vector<ConfigServer> cs = conf->getServers();
        std::vector<ConfigServer>::iterator bg = cs.begin();

        while(bg != cs.end())
        {
            std::cout << "ServerConfig\n";
            std::cout << "Address: " << (*bg).getAddress() << "\n";
            std::cout << "Port: " << (*bg).getPort() << "\n";
            std::map<std::string, std::string> props = (*bg).getProps();
            std::map<std::string, std::string>::iterator beg = props.begin();
            while (beg != props.end())
            {
                std::cout << (*beg).first << " : " << (*beg).second << "\n";
                ++beg;
            }
            print_location_properties((*bg).getLocations());
            print_error_pages((*bg).getErrorPages());
            print_allow_meths((*bg).getAllowMethods());
            ++bg;
            std::cout << "end server config\n\n";
        }
    }
    catch(std::exception e)
    {
        std::cout << e.what() << "\n";
    }
}