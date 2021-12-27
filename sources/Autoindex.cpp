//
// Created by Nigellus Cammy on 12/20/21.
//

#include "../includes/Autoindex.hpp"

std::string cut_url(std::string url){
	long i = url.length() - 1;
	while (url[i - 1] && url[i - 1] != '/')
		--i;
	if (i > 0)
		url.resize(i);
	std::cout << "i after cut: " << i << std::endl;
	return (url);
}

std::string getFileModificationTime(std::string url, const char *path)
{
	url += path;
	struct stat elem;
	char s[100] = {};
	if (stat(url.c_str(), &elem) == 0)
		std::strftime(s, 20, "%d-%m-%Y %H:%M", std::localtime(&elem.st_ctime));
	return std::string(s);
}

std::string getFileSize(std::string url, const char *path)
{
	if (url.back() != '/')
		url += "/";
	url += path;
	struct stat elem;
	if (stat(url.c_str(), &elem) == 0)
	{
		url.clear();
		unsigned long long el_size = elem.st_size;
		unsigned long long size_el = 0;
		while (el_size > 0){
			size_el = size_el * 10 + el_size % 10;
			el_size /= 10;
		}
		while (size_el > 0){
			url += size_el % 10 + 48;
			size_el /= 10;
		}
		return url;
	}
	return "-";
}

std::string autoindex(std::string url, int root){
	DIR *dir;
	struct dirent *ent;
	std::string page;
	std::string url_without_root;
	if (url.back() != '/')
		url += "/";
	if ((dir = opendir(url.c_str())) != NULL){
		url_without_root = url.substr(root, url.length() - root);

		page = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\">";
		page += "<title>Index of ";
		page += url_without_root;
		page += "</title></head><body><h1>Index of ";
		page += url_without_root;
		page += "</h1><table><tbody><tr><th><a>Name</a></th>";
		page += "<th><a>Last modified</a></th><th><a>Size</a></th></tr>";
		page += "<tr><th colspan=\"3\"><hr></th></tr>";

		page += "<tr><td><a href=\"";
		page += cut_url(url_without_root);
		page += "\"> ../ </a></td></tr>";
		if (url.back() != '/')
			url += "/";
		while ((ent = readdir(dir)) != NULL){
			if (ent->d_name[0] == '.')
				continue;
			page += "<tr><td><a href=\"";
			page += url_without_root;
			page += ent->d_name;
			page += "\">";
			page += ent->d_name;
			page += "</a></td><td align=\"right\">";
			page += getFileModificationTime(url, ent->d_name);
			page += "</td><td align=\"right\">";
			page += getFileSize(url, ent->d_name);
			page += "</td></tr>";
		}
		page += "<th colspan=\"3\"><hr></th></tbody></table></body></html>";
		return page;
	}
	return NULL;
}