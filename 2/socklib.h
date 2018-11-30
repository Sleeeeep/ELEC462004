#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<signal.h>
#include<sys/time.h>
#include<limits.h>

int make_server_socket(int);
int make_server_socket_q(int, int);
int connect_to_server(char *, int);
