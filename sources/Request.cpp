#include "../includes/Request.hpp"

Request::Request(): _method(), _headers(), _params(), _body() {}

Request::~Request() {}

std::map<std::string, std::string> Request::getParams() const{
    return this->_params;
}

std::string	Request::getParamsRaw() const
{
	return this->params_row;
}

//std::map<std::string, std::string> Request::getHeader() const{
//    return this->_headers;
//}

std::string Request::getBody() const
{
    return this->_body;
}

std::string Request::getMethod() const
{
    return this->_method;
}

std::string Request::getUrl() const
{
    return this->_url;
}

std::string Request::getQuery() const
{
    return this->Query;
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

void    Request::parseRequest(std::string request)
{
    int start = 0;
    int pos = 0;
    int npos = 0;

    if (getMethod().size() == 0)
    {
        npos = request.find_first_of("\n", start);
        std::string method_row = request.substr(start, npos - start);
        pos = method_row.find_first_of(" \t", start);
        setMethod(method_row.substr(start, pos - start));
        start = pos + 1;
        pos = method_row.find_first_of(" \t", start);
        setUrl(method_row.substr(start, pos - start));
        start = pos + 1;
        pos = method_row.find_first_of(" \t", start);
        setStatus(method_row.substr(start, pos - start));
        start = npos + 1;
        pos = 0;
        npos = 0;
    }
    while (true)
    {
        npos = request.find_first_of("\n", start);
        std::string header_row = request.substr(start, npos - start);
        if (header_row.size() == 0 || header_row.size() == 1)
            break ;
        pos = header_row.find_first_of(":", 0);
        std::string key = header_row.substr(0, pos);
        std::string value = header_row.substr(pos + 2, header_row.size() - pos - 2);
        addHeader(key, value);
        start = npos + 1;
        pos = 0;
        if (npos == -1)
            break;
        npos = 0;
    }
    if (getMethod() == "POST" && npos > 0)
    {
        start = npos + 1;
        npos = request.find_first_of("\n", start);
        params_row = request.substr(start, npos - start);
        start = 0;
        if (npos != -1 || params_row.size() > 0)
        {
            while (true)
            {
                npos = params_row.find_first_of("&", start);
                std::string param = params_row.substr(start, npos - start);
                pos = param.find_first_of("=", 0);
                addParam(param.substr(0, pos), param.substr(pos + 1, param.size() - pos - 1));
                start = npos + 1;
                pos = 0;
                if (npos == (int)params_row.size() || npos == -1)
                    break;
                npos = 0;
            }
        }
    }
    else if (getMethod() == "GET" && npos > 0)
    {
        npos = getUrl().find_first_of("?", 0);
        if (npos != -1)
        {
            params_row = getUrl().substr(npos + 1, getUrl().size() - npos - 1);
            Query = params_row;
            start = 0;
            while (true)
            {
                npos = params_row.find_first_of("&", start);
                std::string param = params_row.substr(start, npos - start);
                pos = param.find_first_of("=", 0);
                addParam(param.substr(0, pos), param.substr(pos + 1, param.size() - pos - 1));
                start = npos + 1;
                pos = 0;
                if (npos == (int)params_row.size() || npos == -1)
                    break;
                npos = 0;
            }
        }
    }
    if (npos != -1)
        setBody(request.substr(npos, request.size() - npos));
}

std::map<std::string, std::string>  Request::getHeaders()
{
    return this->_headers;
}

std::string     Request::getHeader(std::string name)
{
    return this->_headers[name];
}

