#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>

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
        void    setMethod(std::string method);
        void    setUrl(std::string url);
        void    setStatus(std::string status);
        void    addHeader(std::string key, std::string value);
        void    addParam(std::string key, std::string value);
        void    setBody(std::string content);

        void    show();
};

#endif