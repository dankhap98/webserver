//
// Created by Stoops Ysilla on 12/7/21.
//

#include "../includes/Response.hpp"

Response::Response() {
    error_404 = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>404</title><link rel=\"stylesheet\" href=\"bootstrap.min.css\" type=\"text/css\"/></head><body> <header id=\"header\"><h1>404</h1></header></body></html>";
}

Response::Response(ConfigServer &config) {
//    error_404 = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>404</title><link rel=\"stylesheet\" href=\"bootstrap.min.css\" type=\"text/css\"/></head><body> <header id=\"header\"><h1>404</h1></header></body></html>";
    t_server_config conf = config.getConfig()[0];
    open_err = false;
    error_404 = readHtml(conf.error_pages[404]);
    error_403 = readHtml("403.html");
    error_204 = readHtml("204.html");
}

Response::Response(ConfigServer &config, Request& req) {
    //std::cout << req.getHeader("Host") << "\n";
    t_server_config conf = config.getConfigByName(req.getHeader("Host"));
    open_err = false;
    error_404 = readHtml(conf.error_pages[404]);
    error_403 = readHtml("403.html");
    error_204 = readHtml("204.html");
    Path = config.getRootPath(req.getHeader("Host"), req.getUrl());
    std::cout << Path << "\n";
    this->SetResponseMsg(req);
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

void            Response::SetResponseMsg(Request &request)
{
    if(request.getMethod().empty())
    {
        std::cerr<<"Request parsing error\n";
        return ;
    }
    //SetPath(request.getUrl());
    if (file_exist(Path) > 0) {
        if (request.getMethod() == "GET" && request.getParams().empty())
            GETResponse();
        else if (request.getMethod() == "POST" || (request.getMethod() == "GET" && !(request.getParams().empty())))
            POSTResponse(request);
        else if (request.getMethod() == "DELETE")
            remove(Path.c_str());
    }
    else
        ResponseMsg = "HTTP/1.1 404 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(error_404.size()) + "\n\n" + error_404;
}

void            Response::POSTResponse(Request  &request)
{
//    int		fd[2];
//    pid_t	pid;
//    int		status = 0;
//    int		fdd = 0;

    CGIClass cgi(request);

//    if (TRUE)           //проверка на наличие cgi в конфиге
//        ResponseMsg = "HTTP/1.1 404 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
//                      std::to_string(error_404.size()) + "\n\n" + error_404;
//    else
//    {
//        pipe(fd);
//        switch (pid = fork()) {
//            case -1:
//                perror("fork");
//                exit(1);
//            case 0:
//                pid_zero("cgi/cgi.cgi", fdd, fd);
//            default:
//                pid_nonzero(status, fd);
//                if (fdd != NULL)
//                    close(fdd);
//                fdd = fd[0];
//        }
//    }
}

void            Response::GETResponse()
{
    SetContentType();
    Html_text = readHtml(Path);
    if (!(content_type.empty()) && content_type != "no type")
        ResponseMsg = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                      std::to_string(Html_text.size()) + "\n\n" + Html_text;
    else if (!(open_err))
        ResponseMsg = "HTTP/1.1 403 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
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
            ResponseMsg = "HTTP/1.1 403 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                          std::to_string(error_403.size()) + "\n\n" + error_403;
        else
            ResponseMsg = "HTTP/1.1 204 OK\nContent-Type: " + content_type + "\nContent-Length:  " +
                          std::to_string(error_204.size()) + "\n\n" + error_204;
    }
    else
        ResponseMsg = "HTTP/1.1 404 OK\nContent-Type: " + content_type +
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

std::string     Response::GetResponseMsg() {return ResponseMsg;}


//void	pid_zero(std::string Path, t_env *env, int fdd, int *fd)
//{
//    dup2(fdd, STDIN_FILENO);
//    dup2(fd[1], STDOUT_FILENO);
//    execve(Path.c_str(), argv, env);
//    exit(0);
//}
//
//void    pid_nonzero(int *fd)
//{
//    close(fd[1]);
//
//}