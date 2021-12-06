#include "../Configuration.hpp"
#include "../ConfigServer.hpp"
#include "../ConfigLocation.hpp"
#include "../Webserver.hpp"

void   print_location_properties(std::vector<ConfigLocation> cl)
{
    std::vector<ConfigLocation>::iterator beg = cl.begin();

    while (beg != cl.end())
    {
        std::cout << "\nLocationConfig " << (*beg).getUrl() << "\n";
        //std::cout << (*beg).get_SubLocations().size() << "\n";
        std::map<std::string, std::string> props = (*beg).getProps();
        //std::cout << props.size() << "\n";
        std::map<std::string, std::string>::iterator bg = props.begin();
            //count = 0;
            //std::cout << props["root"] << "\n";
        while (bg != props.end())
        {
                //if (count < 2)
            std::cout << (*bg).first << " : " << (*bg).second << "\n";
            ++bg;
                //count++;
        }
        std::cout << "end location config\n\n";
        ++beg;
    }
}

int    main()
{
    Configuration *conf = new Configuration("./test2.conf");
    

    try{
        conf->loadConfig();


        std::vector<ConfigServer> cs = conf->getServers();
        //std::cout << cs.size() << "\n";
        //std::cout << cs[0].getProps().size() << "\n";
        std::vector<ConfigServer>::iterator bg = cs.begin();
        //Webserver wb[cs.size()];
       
        //int count = 0;
        while(bg != cs.end())
        {
            std::cout << "ServerConfig\n";
            std::cout << "Address: " << (*bg).getAddress() << "\n";
            std::cout << "Port: " << (*bg).getPort() << "\n";
            //std::cout << (*bg).getLocations().size() << "\n";
            std::map<std::string, std::string> props = (*bg).getProps();
            //std::cout << props.size() << "\n";
            std::map<std::string, std::string>::iterator beg = props.begin();
            //count = 0;
            //std::cout << props["root"] << "\n";
            while (beg != props.end())
            {
                //if (count < 2)
                std::cout << (*beg).first << " : " << (*beg).second << "\n";
                ++beg;
                //count++;
            }
            print_location_properties((*bg).getLocations());
            ++bg;
            std::cout << "end server config\n\n";
        }
    }
    catch(std::exception e)
    {
        std::cout << e.what() << "\n";
    }
}