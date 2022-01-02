//
// Created by Stoops Ysilla on 12/15/21.
//

#include "../../includes/CGIClass.hpp"

CGIClass::CGIClass() {

}

CGIClass::~CGIClass() {
//	delete argv;
}

CGIClass::CGIClass(Request &request) {
    RequestBody = request.getBody();
    RequestHeader = request.getHeaders();
    SetEviroment(request);
    SetArguments(request);
}

void    CGIClass::SetEviroment(Request &request)
{
	RequestEnviromentForExec = new char *[11];
//    RequestEnviroment["AUTH_TYPE"] = "";
    RequestEnviroment["CONTENT_LENGTH"] = request.getBody().length();
    RequestEnviroment["CONTENT_TYPE"] = request.getHeaders()["Content-Type"];
    RequestEnviroment["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (request.getMethod() == "GET")
    	RequestEnviroment["PATH_INFO"] = request.getUrl().substr(0,  request.getUrl().find("?"));
	else
		RequestEnviroment["PATH_INFO"] = request.getUrl();
    RequestEnviroment["PATH_TRANSLATED"] = "/";
    RequestEnviroment["QUERY_STRING"] = request.getQuery(); //какой он будет при пост?
//    RequestEnviroment["REMOTE_ADDR"] = request.getHeader()["Host"]; //нужно доделать
    RequestEnviroment["REMOTE_HOST"] = request.getHeaders()["Host"];
//    RequestEnviroment["REMOTE_IDENT"]  = "";
//    RequestEnviroment["REMOTE_USER"] = "";
    RequestEnviroment["REQUEST_METHOD"] = request.getMethod();
    RequestEnviroment["SCRIPT_NAME"] = "cgi_tester";
//    RequestEnviroment["SERVER_NAME"] =
//    RequestEnviroment["SERVER_PORT"] =
    RequestEnviroment["SERVER_PROTOCOL"] = "HTTP/1.1";
//    RequestEnviroment["SERVER_SOFTWARE"] =
	std::map<std::string, std::string>::iterator it = RequestEnviroment.begin();
	for (int i = 0; i < 10; i++, it++)
		RequestEnviromentForExec[i] = strdup((it->first + "=" + it->second).c_str());
	RequestEnviromentForExec[10] = (char*)0;
}

void    CGIClass::SetArguments(Request &request)
{
	(void)request;
//	int i = 1;
//	std::map<std::string, std::string>::iterator it = request.getParams().begin();
//	std::map<std::string, std::string>::iterator end = request.getParams().end();
//	while (it != end)
//	{
//		std::cout<< (*it).first << " " << (*it).second << "\n";
//		std::cout<<it->second<<std::endl;
//		i++;
//		std::cout<<"fiasko\n";
//		it++;
//		std::cout<<"fiasko\n";
//	}
//	it = request.getParams().begin();
	argv = new char *[3];
	argv[0] = strdup(("mdeep" + RequestEnviroment["PATH_INFO"]).c_str());
	argv[1] = strdup("test.html");
	argv[2] = (char*)0;
}

std::string	CGIClass::startCGI(Request &rec)
{
	pid_t pid;
	int fdIn[2], fdOut[2];

	if ((pipe(fdIn) != 0 )|| (pipe(fdOut) != 0))
	{
		std::cerr << "Cannot create CGI pipe\n";
		exit(1);
	}
	char b;
	pid = fork();
	if (pid == 0)
	{
		std::cout << rec.getParamsRaw() << std::endl;
		dup2(fdOut[1], 1);
		dup2(fdIn[0], 0);

		write(fdIn[1], rec.getParamsRaw().c_str(), rec.getParamsRaw().length());
		close(fdIn[1]);
		close(fdOut[0]);
		execve(argv[0], argv, RequestEnviromentForExec);

		close(fdIn[0]);
		close(fdOut[1]);
		std::cout << argv[0] << "fiasko\n";

		exit(1);
	}
	else
	{
		close(fdIn[0]);
		close(fdOut[1]);
		close(fdIn[1]);
		std::cout << "\n\n";
		while (1)
		{
			if (waitpid(pid, &status, WNOHANG) > 0)
				break;
			read(fdOut[0], &b, 1);
			std::cout << b;
			bufferOut = bufferOut + b;
		}
		close(fdOut[0]);
		std::cout<<"\n\n";
	}
	delete RequestEnviromentForExec;
	return (bufferOut);
}