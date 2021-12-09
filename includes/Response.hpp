//
// Created by Stoops Ysilla on 12/7/21.
//

#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

#include "server.hpp"
#include "Request.hpp"
#include "ConfigServer.hpp"

class Response {
public:
    Response();
    Response(ConfigServer &config);
    ~Response();

    void            SetContentType();
    void            SetResponseMsg(Request &request);
    void            SetPath(std::string url);
    void            GETResponse();
    void            POSTResponse();
    std::string		readHtml(const std::string& path);
    std::string     GetResponseMsg();
private:
    std::string     ResponseMsg;
    std::string     Html_text;
    std::string     Path;
    std::string     content_type;


    std::string     error_404;
    std::string     error_403;

    bool            open_err;
};


#endif
