#include "Configuration.hpp"
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "Webserver.hpp"

typedef struct s_server_item
{
    Webserver *wb;
}   t_server_item;

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
        std::vector<ConfigServer>::iterator bg = cs.begin();
        t_server_item   sitems[cs.size()];
        pthread_t threads[cs.size()];
        int i = 0;

        while(bg != cs.end())
        {
            sitems[i].wb = new Webserver((*bg).getPort());
            pthread_create(&threads[i], NULL, start_server, (void *)(&sitems[i]));
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