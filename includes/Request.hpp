#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "server.hpp"

class Request
{
    private:
       std::string  _method;
       std::string  _url;
       std::string  _status;
       std::map<std::string, std::string> _headers;
       std::map<std::string, std::string> _params;
       std::string  _body; 

    public:
        Request();
        ~Request();

        std::string getMethod() const;
        std::string getUrl() const;
        std::map<std::string, std::string>  getParams() const;
//        std::map<std::string, std::string>  getHeader();
        void    setMethod(std::string method);
        void    setUrl(std::string url);
        void    setStatus(std::string status);
        void    addHeader(std::string key, std::string value);
        void    addParam(std::string key, std::string value);
        void    setBody(std::string content);
        void    parseRequest(std::string request);
        void    show();
        std::map<std::string, std::string>    getHeaders();
        std::string    getHeader(std::string name);
};

#endif