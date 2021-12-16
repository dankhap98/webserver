//
// Created by Stoops Ysilla on 12/15/21.
//

#ifndef WEBSERVER_CGICLASS_H
#define WEBSERVER_CGICLASS_H

#include "server.hpp"
#include "Request.hpp"

class CGIClass {
public:
    CGIClass();
    CGIClass(Request &request);
    ~CGIClass();

    int     startCGI();

private:
    void    SetEviroment(Request &request);
    void    SetArguments(Request &request);

    std::string RequestBody;
    std::map<std::string, std::string> RequestHeader;
    std::map<std::string, std::string> RequestEnviroment;
	char **RequestEnviromentForExec;
    char **argv;
};


#endif //WEBSERVER_CGICLASS_H
