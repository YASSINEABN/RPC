
#define _XOPEN_SOURCE 500
#include "P3.h"
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



void *write_chat_1_svc(char **argp, struct svc_req *rqstp){
	static char * result;
	int fd;
	xdr_free((xdrproc_t)xdr_string, (char *)&result);

	fd = open("logs.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
	write(fd, *argp, strlen(*argp));
	close(fd);

	return (void *) &result;
}


 
char **get_chat_1_svc(int *argp, struct svc_req *rqstp){
	static char *result;
	char buffer[1024];
	FILE *fp;
	int num_lines = 0;
	
	xdr_free((xdrproc_t)xdr_string, (char *)&result);

	
	if ((fp = popen("cat logs.txt | wc -l", "r")) == NULL) {
  	printf("Error. Server was unable to obtain the number of lines in the file.\n");
		exit(-1);
  }

	bzero(buffer, sizeof(buffer));
	fgets(buffer, 1024, fp);
	pclose(fp);
	num_lines = atoi(buffer);

	
	if(*argp <= num_lines){
		
		sprintf(buffer, "tail -n +%d logs.txt | head -n 1", (*argp));
		if ((fp = popen(buffer, "r")) == NULL) {
			printf("Error. Server was unable to get the last message.\n");
			exit(-1);
		}

		
		bzero(buffer, sizeof(buffer));
		fgets(buffer, 1024, fp);
		pclose(fp);
		result = (char *)malloc(sizeof(char)*(strlen(buffer) + 1));
		bzero(result, strlen(buffer) + 1);
		strcpy(result, buffer);
	}
	return &result;
}
