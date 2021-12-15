//
// Created by Stoops Ysilla on 12/15/21.
//

#include "CGIClass.hpp"

CGIClass::CGIClass() {

}

CGIClass::~CGIClass() {

}

CGIClass::CGIClass(Request &request) {
    RequestBody = request.getBody();
    RequestHeader = request.getHeader();
    SetEviroment(request);
    SetArguments(request);
}

void    CGIClass::SetEviroment(Request &request)
{
//    RequestEnviroment["AUTH_TYPE"] = "";
    RequestEnviroment["CONTENT_LENGTH"] = request.getBody().length();
    RequestEnviroment["CONTENT_TYPE"] = request.getHeader()["Content-Type"];
    RequestEnviroment["GATEWAY_INTERFACE"] = "CGI/1.1"
    RequestEnviroment["PATH_INFO"] = request.getUrl();
    RequestEnviroment["PATH_TRANSLATED"] = "/";
    RequestEnviroment["QUERY_STRING"] = request.getQuery(); //какой он будет при пост?
//    RequestEnviroment["REMOTE_ADDR"] = request.getHeader()["Host"]; //нужно доделать
    RequestEnviroment["REMOTE_HOST"] = request.getHeader()["Host"];
//    RequestEnviroment["REMOTE_IDENT"]  = "";
//    RequestEnviroment["REMOTE_USER"] = "";
    RequestEnviroment["REQUEST_METHOD"] = request.getMethod();
//    RequestEnviroment["SCRIPT_NAME"] =
//    RequestEnviroment["SERVER_NAME"] =
//    RequestEnviroment["SERVER_PORT"] =
    RequestEnviroment["SERVER_PROTOCOL"] = "HTTP/1.1";
//    RequestEnviroment["SERVER_SOFTWARE"] =
}

void    CGIClass::SetArguments(Request &request)
{
    int i = 0;
    argv[0][] = "/cgi/cgi_tester";
    for (auto it = request.getParams().begin(); it != request.getParams().end(), it++)
    {
        i++;
        argv[i][] = it->second.c_str();
    }
}