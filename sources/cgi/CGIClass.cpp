//
// Created by Stoops Ysilla on 12/15/21.
//

#include "../../includes/CGIClass.hpp"

CGIClass::CGIClass() {

}

CGIClass::~CGIClass() {
	delete RequestEnviromentForExec;
	double argv;
}

CGIClass::CGIClass(Request &request) {
    RequestBody = request.getBody();
    RequestHeader = request.getHeaders();
    SetEviroment(request);
    SetArguments(request);
}

void    CGIClass::SetEviroment(Request &request)
{
	RequestEnviromentForExec = new char *[9];
//    RequestEnviroment["AUTH_TYPE"] = "";
    RequestEnviroment["CONTENT_LENGTH"] = request.getBody().length();
    RequestEnviroment["CONTENT_TYPE"] = request.getHeaders()["Content-Type"];
    RequestEnviroment["GATEWAY_INTERFACE"] = "CGI/1.1";
    RequestEnviroment["PATH_INFO"] = request.getUrl();
    RequestEnviroment["PATH_TRANSLATED"] = "/";
    RequestEnviroment["QUERY_STRING"] = request.getQuery(); //какой он будет при пост?
//    RequestEnviroment["REMOTE_ADDR"] = request.getHeader()["Host"]; //нужно доделать
    RequestEnviroment["REMOTE_HOST"] = request.getHeaders()["Host"];
//    RequestEnviroment["REMOTE_IDENT"]  = "";
//    RequestEnviroment["REMOTE_USER"] = "";
    RequestEnviroment["REQUEST_METHOD"] = request.getMethod();
//    RequestEnviroment["SCRIPT_NAME"] =
//    RequestEnviroment["SERVER_NAME"] =
//    RequestEnviroment["SERVER_PORT"] =
    RequestEnviroment["SERVER_PROTOCOL"] = "HTTP/1.1";
//    RequestEnviroment["SERVER_SOFTWARE"] =
	std::map<std::string, std::string>::iterator it = RequestEnviroment.begin();
	for (int i = 0; i < 9; i++, it++)
		RequestEnviromentForExec[i] = strdup((it->first + "=" + it->second).c_str());
}

void    CGIClass::SetArguments(Request &request)
{
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
	argv = new char *[2];
	argv[0] = strdup("/cgi/cgi_tester");
//	for (int iter = 1; iter != i; iter++)
//		argv[i] = strdup(it->second.c_str());
	argv[1] = strdup("test.html");
}

void 	CGIClass::RunCGI()
{
	if (!execve(argv[0], argv, RequestEnviromentForExec))
	{
		std::cerr<<"GG";
		exit(1);
	}
}


char*	CGIClass::startCGI(std::string Page)
{
	pid_t pid;
	int fd[2];

	if (pipe(fd) != 0)
	{
		std::cerr << "Cannot create CGI pipe\n";
		exit(1);
	}
	pid = fork();
	if (pid == 0)
	{
		dup2(fd[1], 1);
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		if (!execve(argv[0], argv, RequestEnviromentForExec))
		{
			std::cerr << "GG";
			exit(1);
		}
	}
	else
	{
		dup2(fd[0], 0);
//		write(fd[1], Page.c_str(), Page.length());
		read(0, bufferOut, 100000);
	}
	close(fd[1]);
	close(fd[0]);
	return (bufferOut);
}

//char*		CGIClass::startCGI(std::string Page)
//{
//	pid_t pid;
//	int fdStdInPipe[2], fdStdOutPipe[2];
//	fdStdInPipe[0] = fdStdInPipe[1] = fdStdOutPipe[0] = fdStdOutPipe[1] = -1;
//	if (pipe(fdStdInPipe) != 0 || pipe(fdStdOutPipe) != 0)
//	{
//		std::cerr << "Cannot create CGI pipe\n";
//		exit(1);
//	}
//
//	int fdOldStdIn = dup(fileno(stdin));
//	int fdOldStdOut = dup(fileno(stdout));
//
//	if ((dup2(fdStdOutPipe[1], fileno(stdout)) == -1) || (dup2(fdStdInPipe[0], fileno(stdin)) == -1))
//		exit(1);
//	close(fdStdInPipe[0]);
//	close(fdStdOutPipe[1]);
//
//	pid = fork();
//	if (pid < 0)
//		exit(1);
//	else if (!pid)
//	{
//		if (!execve(argv[0], argv, RequestEnviromentForExec))
//		{
//			std::cerr << "GG";
//			exit(1);
//		}
//	}
//	dup2(fdOldStdIn, fileno(stdin));
//	dup2(fdOldStdOut, fileno(stdout));
//	RequestBody = Page;
//	write(fdStdInPipe[1], RequestBody.c_str(), RequestBody.length());
//
//	while (1)
//	{
//		int n = read(fdStdOutPipe[0], bufferOut, 100000);
//		if (n > 0)
//		{
//			fwrite(bufferOut, 1, n, stdout);
//			fflush(stdout);
//		}
//		if (waitpid(pid, &status, WNOHANG) > 0)
//			break;
//	}
//	std::cout<<"--------------abaoba-------------\n"<<bufferOut;
//	return (bufferOut);
//}