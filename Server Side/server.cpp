#include "server.h"

void error(const char *msg)
{
	perror(msg);
}

void connect(int *sockfd, int *newsockfd){
	 socklen_t clilen;
	 char buffer[256];
	 struct sockaddr_in serv_addr,	cli_addr;

	 (*sockfd) = socket(AF_INET, SOCK_STREAM,	0);
	 if ((*sockfd) < 0){
		error("ERROR opening socket");
     }
	 bzero((char *) &serv_addr, sizeof(serv_addr));
	 serv_addr.sin_family = AF_INET;
	 serv_addr.sin_addr.s_addr	= INADDR_ANY;
	 serv_addr.sin_port = htons(DEFAULT_PORT);
	 if (bind((*sockfd), (struct sockaddr	*) &serv_addr, sizeof(serv_addr)) <	0){ 
			  error("ERROR on binding");
     }
     bzero(buffer,256);

}
void clientConnect(int *sockfd, int *newsockfd){
     socklen_t clilen;
     struct sockaddr_in cli_addr;
     char buffer[256];
	 listen((*sockfd),5);
	 clilen = sizeof(cli_addr);
     printf("Waiting on Client Connection\n");
	 (*newsockfd) = accept((*sockfd), (struct sockaddr *)&cli_addr, &clilen);
	 if ((*newsockfd)	< 0) {
		  error("ERROR on accept");
     }
	 bzero(buffer,256);
}  
int recieve(int sockfd, int newsockfd, char* buffer){
   
    int n;
    char buffer2[256];
	n = read(newsockfd, buffer2 ,255);
	if (n < 0){ 
		printf("ERROR reading from socket\n");
		return 1;
	}
	else if(buffer2[0] != '\0')		   
        strcpy(buffer, buffer2);
	return 0;	
}
int send(int newsockfd, float distance){
    int n= 0;
    char buffer[256];
    sprintf(buffer, "%f", (distance));
	n = write(newsockfd, buffer ,255);
	if (n < 0) {
		printf("ERROR writing to socket\n");
		return 1;
	}
    return 0;
}
int closeSocket(int *newsockfd){
	 close(*newsockfd);
}
