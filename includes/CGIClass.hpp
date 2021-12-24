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

    std::string startCGI(Request &req);

private:
    void    SetEviroment(Request &request);
    void    SetArguments(Request &request);

	void	RunCGI();

	int 		status;
    std::string RequestBody;
    std::map<std::string, std::string> RequestHeader;
    std::map<std::string, std::string> RequestEnviroment;
	char **RequestEnviromentForExec;
    char **argv;
	std::string bufferOut;
};


#endif //WEBSERVER_CGICLASS_H
