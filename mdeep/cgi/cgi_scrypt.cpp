//
// Created by Stoops Ysilla on 12/29/21.
//
#include "stdio.h"
#include "string"
#include "unistd.h"

int main(void)
{
	std::string params;
	scanf("%s", params.c_str());
	printf("Content-Type: text/plain;charset=us-ascii\n");
	printf("Content-Length: ");
	printf("%lu\n\n", strlen(params.c_str()));
	printf("%s\n\n", params.c_str());
	sleep(1);
	return 0;
}