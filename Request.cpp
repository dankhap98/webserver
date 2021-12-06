#include "Request.hpp"

Request::Request(): _method(), _headers(), _params(), _body() {}

Request::~Request() {}

std::string Request::getMethod() const
{
    return this->_method;
}

std::string Request::getUrl() const
{
    return this->_url;
}
        
void    Request::setMethod(std::string method)
{
    this->_method = method;
}

void    Request::setUrl(std::string url)
{
    this->_url = url;
}
        
void    Request::setStatus(std::string status)
{
    this->_status = status;
}
        
void    Request::addHeader(std::string key, std::string value)
{
    this->_headers[key] = value;
}
        
void    Request::addParam(std::string key, std::string value)
{
    this->_params[key] = value;
}

void    Request::setBody(std::string content)
{
    this->_body = content;
}

void    Request::show() //temporary func
{
    std::cout << "\n\n__REQUEST PARSED:__\n";
    std::cout << "Method: " << this->_method << "\n";
    std::cout << "Url: " << this->_url << "\n";
    std::cout << "Status: " << this->_status << "\n\n";

    //show headers
    std::cout << "__HEADERS__\n";
    std::map<std::string, std::string>::iterator beg = this->_headers.begin();
    while (beg != this->_headers.end())
    {
        std::cout << (*beg).first << ": " << (*beg).second << "\n";
        ++beg;
    }
    std::cout << "\n";

     //show params
    std::cout << "__PARAMS__\n";
    std::map<std::string, std::string>::iterator bg = this->_params.begin();
    while (bg != this->_params.end())
    {
        std::cout << (*bg).first << ": " << (*bg).second << "\n";
        ++bg;
    }
    std::cout << "\n";

    std::cout << "__BODY__\n";
    std::cout << this->_body << "\n";
}