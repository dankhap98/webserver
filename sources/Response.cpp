//
// Created by Stoops Ysilla on 12/7/21.
//

#include "../includes/Response.hpp"

Response::Response() {

}

Response::~Response() {

}

std::string		Response::readHtml(const std::string& path)
{
    std::ofstream		file;
    std::stringstream	buffer;

    file.open(path.c_str(), std::ifstream::in);
    if (file.is_open() == false)
        return ("<!DOCTYPE html>\n<html><title>40404</title><body>"+path+"There was an error finding your error page</body></html>\n");

    buffer << file.rdbuf();
    file.close();
    return (buffer.str());
}

void     Response::SetPath(std::string url)
{
    int end_path;
//    if (!url)
//        std::cerr<<"ЮСУФ КАК ОНО НЕ СЕГНУЛОСЬ БЕЗ ЮРЛ??\n";
    end_path = url.find_first_of("?");
    if(end_path == (int)std::string::npos)
        Path = url.substr(1, url.size());
    else
    {
        Path = url.substr(1, end_path + 1);
        std::cout<<"\n\nPATH: "<<Path<<std::endl;
    }
}

void            Response::SetResponseMsg(Request &request)
{
//    if(!request)
//    {
//        std::cerr<<"Request parsing error\n";
//        return (NULL);
//    }
    if (request.getMethod() == "GET")
    {
        SetPath(request.getUrl());
        //ОПРЕДЕЛЕНИЕ ТИПА ФАЙЛА
        Html_text = readHtml(Path);
        ResponseMsg = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:  " + std::to_string(Html_text.size()) + "\n\n" + Html_text;
    }
}

std::string     Response::GetResponseMsg() {return ResponseMsg;}