//
// Created by Stoops Ysilla on 12/7/21.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "server.hpp"
#include "Request.hpp"
#include "ConfigServer.hpp"
#include "CGIClass.hpp"

class Response {
public:
    Response();
    Response(ConfigServer &config);
    Response(ConfigServer &config, Request& req);
    ~Response();

    void            SetContentType();
    void            SetResponseMsg(Request &request);
    void            SetPath(std::string url);
    void            GETResponse();
    void            POSTResponse(Request  &request);
    void            DELETEResponse();
    std::string		readHtml(const std::string& path);
    std::string     GetResponseMsg();
private:
    std::string     ResponseMsg;
    std::string     Html_text;
    std::string     Path;
    std::string     content_type;
	std::string		true_path;


    std::string     error_404;
    std::string     error_403;
    std::string     error_204;

    bool            open_err;
	bool			redirect;
};


#endif
