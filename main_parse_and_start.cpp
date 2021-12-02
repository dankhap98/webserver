#include "Configuration.hpp"
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "Webserver.hpp"

typedef struct s_server_item
{
    Webserver *wb;
}   t_server_item;


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

void    *start_server(void *args)
{
    t_server_item *si = (t_server_item *)args;
    si->wb->start();
    return (0);
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
        t_server_item   sitems[cs.size()];
        pthread_t threads[cs.size()];
        int i = 0;
        //int count = 0;
        while(bg != cs.end())
        {
            //std::cout << (*bg).getPort() << "--port\n";
            //Webserver wb((*bg).getPort());
            //t_server_item sitem;
            //wb.start();
            sitems[i].wb = new Webserver((*bg).getPort());
            //pthread_create(&threads[i], NULL, start_server, (void *)(&sitem));
            ++bg;
            i++;
            /*std::cout << "ServerConfig\n";
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
            std::cout << "end server config\n\n";*/
        }
        bg = cs.begin();
        i = 0;
        while (bg != cs.end())
        {
            pthread_create(&threads[i], NULL, start_server, (void *)(&sitems[i]));
            //pthread_join(threads[i], NULL);
            ++bg;
            i++;
        }

        bg = cs.begin();
        i = 0;
        while (bg != cs.end())
        {
            pthread_join(threads[i], NULL);
            ++bg;
            i++;
        }
    }
    catch(std::exception e)
    {
        std::cout << e.what() << "\n";
    }
}