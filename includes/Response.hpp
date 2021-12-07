//
// Created by Stoops Ysilla on 12/7/21.
//

#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

#include "server.hpp"
#include "Request.hpp"

class Response {
public:
    Response();
    ~Response();

    void            SetResponseMsg(Request &request);
    std::string		readHtml(const std::string& path);
    void            SetPath(std::string url);
    std::string     GetResponseMsg();
private:
//    Request         request;
    std::string     ResponseMsg;
    std::string     Html_text;
    std::string     Path;

};


#endif
