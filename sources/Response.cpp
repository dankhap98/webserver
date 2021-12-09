//
// Created by Stoops Ysilla on 12/7/21.
//

#include "../includes/Response.hpp"

Response::Response() {
    error_404 = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>404</title><link rel=\"stylesheet\" href=\"bootstrap.min.css\" type=\"text/css\"/></head><body> <header id=\"header\"><h1>404</h1></header></body></html>";
}

Response::~Response() {

}

std::string		Response::readHtml(const std::string& path)
{
    std::ofstream		file;
    std::stringstream	buffer;

    file.open(path.c_str(), std::ifstream::in);
    if (file.is_open() == false)
        return (error_404);

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

void            Response::SetResponseMsg(Request &request)
{
    if(request.getMethod().empty())
    {
        std::cerr<<"Request parsing error\n";
        return ;
    }
    SetPath(request.getUrl());
    if (request.getMethod() == "GET")
        GETResponse();
    if (request.getMethod() == "POST")
        POSTResponse();
    if (request.getMethod() == "DELETE")
        remove(Path.c_str());
}

void            Response::POSTResponse()
{}

void            Response::GETResponse()
{
    SetContentType();
    if (!(content_type.empty()))
    {
        Html_text = readHtml(Path);
        ResponseMsg = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(Html_text.size()) + "\n\n" + Html_text;
    }
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
    else if (type == "jpeg" || type == "jpg")
        content_type = "image/jpeg";
}

std::string     Response::GetResponseMsg() {return ResponseMsg;}