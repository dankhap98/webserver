//
// Created by Stoops Ysilla on 12/7/21.
//

#include "../includes/Response.hpp"
#include "../includes/Autoindex.hpp"

Response::Response() {
    error_404 = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>404</title><link rel=\"stylesheet\" href=\"bootstrap.min.css\" type=\"text/css\"/></head><body> <header id=\"header\"><h1>404</h1></header></body></html>";
}

Response::Response(ConfigServer &config) {
	t_server_config conf = config.getConfig()[0];
    open_err = false;
    error_404 = readHtml(conf.error_pages[404]);
    error_403 = readHtml("403.html");
    error_204 = readHtml("204.html");
}

Response::Response(ConfigServer &config, Request& req) {
    t_server_config conf = config.getConfigByName(req.getHeader("Host"));
	redirect = config.isRedirect(req.getHeader("Host"), req.getUrl());
    if (redirect)
		true_path = config.getRedirect(req.getHeader("Host"), req.getUrl());
	open_err = false;
    error_404 = readHtml(conf.error_pages[404]);
    error_403 = readHtml("403.html");
    error_204 = readHtml("204.html");
	error_413 = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>404</title><link rel=\"stylesheet\" "
				"href=\"bootstrap.min.css\" type=\"text/css\"/></head><body> <header "
				"id=\"header\"><h1>413</h1></header></body></html>";
	Path = config.getRootPath(req.getHeader("Host"), req.getUrl());
    //std::cout << Path << "\n";
    this->SetResponseMsg(req, config);
}

Response::~Response() {

}

std::string		Response::readHtml(const std::string& path)
{
    std::ofstream		file;
    std::stringstream	buffer;

    file.open(path.c_str(), std::ifstream::in);
    if (file.is_open() == false)
    {
        open_err = true;
        return (error_404);
    }
    open_err = false;
    buffer << file.rdbuf();
    file.close();
    return (buffer.str());
}

void     Response::SetPath(std::string url)
{
    int end_path;
    if (url.empty())
        std::cerr<<"ЮСУФ КАК ОНО НЕ СЕГНУЛОСЬ БЕЗ ЮРЛ??\n";
    end_path = url.find_first_of("?");
    if(end_path == (int)std::string::npos)
        Path = url.substr(1, url.size());
    else
    {
        Path = url.substr(1, end_path);
        std::cout<<"\n\nPATH: "<<Path<<std::endl;
    }
    if (Path == "")
        Path = "test.html";
}

void            Response::SetResponseMsg(Request &request, ConfigServer& config)
{
    if(request.getMethod().empty())
    {
        std::cerr<<"Request parsing error\n";
        return ;
    }
	std::cout << Path << "\n";
    //SetPath(request.getUrl());
	if (!(redirect))
	{
		if (request.getBody().size() > std::atoi(config.getBufferSize(request.getHeader("Host"), request.getUrl()).c_str()))
			ResponseMsg = BodiLimit();
		if (file_exist(Path) == 1)
		{
			if (request.getMethod() == "GET" && request.getParams().empty())
				GETResponse();
			else if (request.getMethod() == "POST" || (request.getMethod() == "GET" && !(request.getParams().empty())))
				POSTResponse(request, config);
			else if (request.getMethod() == "DELETE")
				DELETEResponse();
		}
		else if (file_exist(Path) == 2){
			if (config.getAutoIndex(request.getHeader ("Host"), request.getUrl()))
			{
				t_server_config conf = config.getConfigByName(
						request.getHeader("Host"));
				std::string autoindex_html = autoindex(Path, conf.props["root"].length());
				ResponseMsg ="HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:  "
						+ std::to_string(autoindex_html.size()) + "\n\n" +autoindex_html;
			}
			else {
				ResponseMsg = "HTTP/1.1 403 Forbidden\nContent-Type: Forbidden\nContent-Length:  " +
							  std::to_string(error_403.size()) + "\n\n" +error_403;
			}
		}
		else
			ResponseMsg = "HTTP/1.1 404 Not found\nContent-Type: " + content_type + "\nContent-Length:  " +
						  std::to_string(error_404.size()) + "\n\n" + error_404;
	}
	else
		ResponseMsg = "HTTP/1.1 301 Moved Permanently\nLocation: " + true_path;
}

void            Response::POSTResponse(Request  &request, ConfigServer& config)
{
	if (config.getCGIPath(request.getHeader("Host"), request.getUrl()).empty() && request.getHeader("Content-Type")
	.find( "multipart/form-data") != std::string::npos)
	{
		std::cout<<"\n\n\n\n\n\n\n\nEEEE\n\n\n\n\n\n\n\n";
		setBoundary(request);
		setPostHeader(request);
		std::cout<<postFileName<<"\n\n";
		setPostBody(request);
	}
	else
	{
		CGIClass cgi(request);
		Html_text = cgi.startCGI(request);
//		std::cout<<Html_text;
		ResponseMsg = Html_text;
	}
}

void            Response::GETResponse()
{
    SetContentType();
    Html_text = readHtml(Path);
    if (!(content_type.empty()) && content_type != "no type")
        ResponseMsg = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(Html_text.size()) + "\n\n" + Html_text;
    else if (!(open_err))
        ResponseMsg = "HTTP/1.1 403 Forbidden\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(error_403.size()) + "\n\n" + error_403;
    else
        ResponseMsg = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(Html_text.size()) + "\n\n" + Html_text;
}

void            Response::DELETEResponse()
{
    if (file_exist(Path) > 0)
    {
        if (remove(Path.c_str()) == -1)
            ResponseMsg = "HTTP/1.1 403 Forbidden\nContent-Type: " + content_type + "\nContent-Length:  " +
                          std::to_string(error_403.size()) + "\n\n" + error_403;
        else
            ResponseMsg = "HTTP/1.1 204 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                          std::to_string(error_204.size()) + "\n\n" + error_204;
    }
    else
        ResponseMsg = "HTTP/1.1 404 Not found\nContent-Type: " + content_type +
				"\nContent-Length:  " +
                      std::to_string(error_404.size()) + "\n\n" + error_404;
}

void            Response::SetContentType()
{
    std::string type;
    type = Path.substr(Path.rfind(".") + 1, Path.size() - Path.rfind("."));
    if (type == "html")
        content_type = "text/html";
    else if (type == "css")
        content_type = "text/css";
    else if (type == "js")
        content_type = "text/javascript";
    else if (type == "php")
        content_type = "text/php";
    else if (type == "mp4")
        content_type = "video/mp4";
    else if (type == "jpeg" || type == "jpg")
        content_type = "image/jpeg";
    else
        content_type = "no type";
}

std::string		Response::BodiLimit()
{
	std::string msg;

	msg = "HTTP/1.1 413 Payload Too Large\nContent-Type: " + content_type +
		  "\nContent-Length:  " +
		  std::to_string(error_413.size()) + "\n\n" + error_413;

	return msg;
}

std::string     Response::GetResponseMsg() {return ResponseMsg;}

void 			Response::setBoundary(Request  &request)
{
	int i = request.getHeader("Content-Type")
					.find( "boundary=") + 9;
	//boundary = request.getHeader("Content-Type").substr(i, request.getHeader("Content-Type").size());
	//std::cout << "CT: " << request.getHeader("Content-Type").size() << " " << request.getHeader("Content-Type").find
	//m("\n") << "\n";
	boundary = request.getHeader("Content-Type").substr(i, request.getHeader("Content-Type").size() - i - 1);
}

void 			Response::setPostHeader(Request &request)
{
	int nameS = request.getBody().find("filename=") + 10;
	int nameF = request.getBody().find("Type:");
	postFileName = request.getBody().substr(nameS, 9);

}

void 			Response::setPostBody(Request &request)
{
	std::cout << "start\n\n";
	std::string body = request.getBody();
	int ct = body.find("Content-Type");
	int start = body.find("\n", ct);
	int size = body.find(boundary, start);
	postFileData = body.substr(start, size - start - 2);
	//std::cout << "test " << body.substr(start, body.size()) << "\n";
	std::cout << "pfdstart " << postFileData << " pfd\n";
}