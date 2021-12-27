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

void    print_config_struct(t_server_config conf)
{
    std::map<std::string, std::string>::iterator beg = conf.props.begin();
    while (beg != conf.props.end())
    {
            std::cout << (*beg).first << " : " << (*beg).second << "\n";
            ++beg;
    }
    print_location_properties(conf.locations);
    print_error_pages(conf.error_pages);
    print_allow_meths(conf.allow_methods);
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
            std::vector<t_server_config>    sconf = (*bg).getConfig();
            std::vector<t_server_config>::iterator beg = sconf.begin();
            while (beg != sconf.end())
            {
                std::cout << "CONFIG STRUCT:\n";
                print_config_struct((*beg));
                std::cout << "END CONFIG STRUCT\n";
                ++beg;
            }
            std::cout << "end server config\n\n";
            //std::cout << "|___________________|\n";

            t_server_config conf_test = (*bg).getConfigByName("localhost:8000");
            std::cout << "GET CONF TEST. size: " << (*bg).getConfig().size() << "\n";
            std::cout << conf_test.props["server_name"] <<  "\n";
            std::cout << "________END OF TEST______\n";
            /*ConfigLocation  cl = (*bg).getConfigLocationByUrl((*bg).getConfig()[0], "/directory");
            std::cout << "GET LOC TEST. size: " << (*bg).getConfig()[0].locations.size() << "\n";
            if (cl.getUrl().size() != 0)
                std::cout << cl.getUrl() <<  "\n";
            else
                std::cout << "in root\n";
            std::cout << "________END OF TEST______\n";
           
            //print_location_properties((*cl).get_SubLocations());
            if (cl.getUrl().size() != 0)
            {
                ConfigLocation  cl2 = cl.getConfigSubLocationByUrl("/directory/test");
                std::cout << "GET SUB_LOC TEST. size: " << (cl.get_SubLocations()).size() << "\n";
                if(cl2.getUrl().size() != 0)
                    std::cout << cl2.getUrl() <<  "\n";
                else
                    std::cout << "in loc root\n";
                std::cout << "________END OF TEST______\n";
            }
            std::cout << "sub_loc test 0\n";*/
            /*std::cout << "GET IP TEST. ip: " << (*bg).getAddress() << "\n";
            std::cout << (*bg).getIpAddressInt() <<  "\n";
            std::cout << "________END OF TEST______\n";*/
            /*std::cout << "GET ROOT TEST. \n";
            std::cout << (*bg).getRootPath("localhost:8000", "/put_test/test") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/post_body/test") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/directory/test/some.txt") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/404.css") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/some/path") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/") << "\n";
            std::cout << (*bg).getRootPath("localhost:8000", "/images/logo.jpg") << "\n";
            std::cout << "________END OF TEST______\n";*/
            /*std::cout << "GET REDIRECT TEST. \n";
            std::cout << (*bg).isRedirect("localhost:8000", "/put_test/test") << (*bg).getRedirect("localhost:8000", "/put_test/test") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/post_body/test") << (*bg).getRedirect("localhost:8000", "/post_body/test") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/directory/test/some.txt") << (*bg).getRedirect("localhost:8000", "/directory/test/some.txt") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/404.css") << (*bg).getRedirect("localhost:8000", "/404.css") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/some/path") << (*bg).getRedirect("localhost:8000", "/some/path") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/") << (*bg).getRedirect("localhost:8000", "/") << "\n";
            std::cout << (*bg).isRedirect("localhost:8000", "/images/logo.jpg") << (*bg).getRedirect("localhost:8000", "/images/logo.jpg") << "\n";
            std::cout << "________END OF TEST______\n";*/
            /*std::cout << "GET IP TEST. \n";
            std::cout<< conf->valid_ip("127.0.0.1") << '\n';
            std::cout<< conf->valid_ip("0.0.0.0") << '\n';
            std::cout<< conf->valid_ip("127.0.0.-1") << '\n';
            std::cout<< conf->valid_ip("127.300.0.1") << '\n';
            std::cout<< conf->valid_ip("127..0.1") << '\n';
            std::cout<< conf->valid_ip("127.0.0.1.") << '\n';
            std::cout<< conf->valid_ip("") << '\n';
            std::cout << "________END OF TEST______\n";
            std::cout << "GET PORT TEST. \n";
            std::cout<< conf->valid_port("127.0.0.1") << '\n';
            std::cout<< conf->valid_port("test") << '\n';
            std::cout<< conf->valid_port("80") << '\n';
            std::cout<< conf->valid_port("080") << '\n';
            std::cout<< conf->valid_port("") << '\n';
            std::cout << "________END OF TEST______\n";*/
            /*std::cout << "GET AUTOINDEX TEST. \n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/put_test/test") << "\n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/post_body/test") << "\n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/directory/test") << "\n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/404.css") << "\n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/some/path") << "\n";
            std::cout << (*bg).getAutoIndex("localhost:8000", "/") << "\n";
            std::cout << "________END OF TEST______\n";
            std::cout << "GET BSIZE TEST. \n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/put_test/test") << "\n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/post_body/test") << "\n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/directory/test") << "\n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/404.css") << "\n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/some/path") << "\n";
            std::cout << (*bg).getBufferSize("localhost:8000", "/") << "\n";
            std::cout << "________END OF TEST______\n";*/
            /*std::cout << "GET ALLOW_METHODS TEST. \n";
            print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/put_test/test"));
             print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/post_body/test"));
             print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/directory/test"));
             print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/404.css"));
             print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/some/path"));
             print_allow_meths((*bg).getAllowMethodsForUrl("localhost:8000", "/"));
            std::cout << "________END OF TEST______\n";*/
            std::cout << "GET CGIPATH TEST. \n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/put_test/test")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/post_body/test")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/directory/test/some.txt")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/404.css")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/some/path")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/")  << "\n";
            std::cout << (*bg).getCGIPath("localhost:8000", "/mycgitotest")  << "\n";
            std::cout << "________END OF TEST______\n";
            ++bg;
        }
        
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << "\n";
    }

    delete conf;
}