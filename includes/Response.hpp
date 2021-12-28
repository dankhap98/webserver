//
// Created by Stoops Ysilla on 12/7/21.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "server.hpp"
#include "Request.hpp"
#include "ConfigServer.hpp"
#include "CGIClass.hpp"
#include "Autoindex.hpp"

class Response {
public:
    Response();
    Response(ConfigServer &config);
    Response(ConfigServer &config, Request& req);
    ~Response();

    void            SetContentType();
    void            SetResponseMsg(Request &request, ConfigServer& config);
    void            SetPath(std::string url);
    void            GETResponse();
    void            POSTResponse(Request  &request, ConfigServer& config);
    void            DELETEResponse();
	std::string		BodiLimit();
    std::string		readHtml(const std::string& path);
    std::string     GetResponseMsg();
private:
	void			setBoundary(Request  &request);
	void 			setPostHeader(Request  &request);
	void 			setPostBody(Request  &request);

	std::string		response_405(ConfigServer& config, std::string host);//It's ok? by ncammy

    std::string     ResponseMsg;
    std::string     Html_text;
    std::string     Path;
    std::string     content_type;
	std::string		true_path;
	std::string		boundary;
	std::string 	postFileName;
	std::string		postFileData;


	std::string 	error_413;
	std::string 	error_405;
	std::string 	error_405p2;
    std::string     error_404;
    std::string     error_403;
    std::string     error_204;

    bool            open_err;
	bool			redirect;
};


#endif
