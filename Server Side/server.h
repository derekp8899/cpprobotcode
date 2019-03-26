#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>


#define DEFAULT_PORT 27015

void error(const char*);
void connect(int* , int*);
void clientConnect(int*, int*);
int recieve(int, int, char*);
int send(int, float);
int closeSocket(int*);

#endif
