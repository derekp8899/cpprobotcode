
#include "server.h"

int main(int argc, char **argv){
	
  int sockfd, newsockfd, num = 0, num2 = 0;
  float distance = 10.0;
  char buffer[256];
  connect(&sockfd, &newsockfd);
  clientConnect(&sockfd, &newsockfd);
  while(true){
    num = recieve(sockfd, newsockfd, buffer);
    num2 = send(newsockfd, distance);
    printf("did a write\n");
    if(num2 == 1 || num == 1){
      printf("must reestablish\n\n");
      closeSocket(&newsockfd);
      clientConnect(&sockfd, &newsockfd);
    } else
      printf("Controller: %s\n\n", buffer);
  }
}
