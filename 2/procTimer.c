#include"socklib.h"

#define HOSTNAME "155.230.105.104"

int main(int ac, char *av[])
{
    int sock1, sock2;
	char buf[BUFSIZ];

	int repetition, size;
	long min[2], max[2], avg[2];

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

    if(ac != 5)
    {
		fprintf(stderr, "./procTimer .txt repetition port1 port2\n");
		exit(1);
    }
	//////////////////////////////////////////////////////////
	//						socket 1						//		
	//////////////////////////////////////////////////////////
    if((sock1 = connect_to_server(HOSTNAME, atoi(av[3]))) == -1)
    {
		fprintf(stderr, "socket 1 error\n");
		exit(1);
	}
	sleep(1);
	//////////////////////////////////////////////////////////
	//						socket 2						//		
	//////////////////////////////////////////////////////////
    if((sock2 = connect_to_server(HOSTNAME, atoi(av[4]))) == -1)
    {
		fprintf(stderr, "socket 2 error\n");
		exit(1);
	}
	//////////////////////////////////////////////////////////
	//						execute 						//		
	//////////////////////////////////////////////////////////
	//	send 'm' to sock1
	sprintf(buf, "m");
	if(write(sock1, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write 'm' error\n");
		exit(1);
	}
	//	send filename and repetition to sock2
	sprintf(buf, "%s %d", av[1], atoi(av[2]));
	if(write(sock2, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write filename and repetition error\n");
		exit(1);
	}
	//	wait to receive 'd' from sock1
	if(read(sock1, buf, BUFSIZ) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	if(strcmp(buf, "d") == 0)
		printf("Received  '%s'\n", buf);
	else
	{
		printf("Received  '%s' but expect 'd'\n", buf);
		exit(1);
	}
	//	send 's' to sock1
	sprintf(buf, "s");
	if(write(sock1, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write 's' error\n");
		exit(1);
	}
	//	wait to receive 'f' from sock1
	if(read(sock1, buf, BUFSIZ) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	if(strcmp(buf, "f") == 0)
		printf("Received  '%s'\n", buf);
	else
	{
		printf("Received  '%s' but expect 'f'\n", buf);
		exit(1);
	}
	//	wait to receive data from sock2
	if(read(sock2, buf, BUFSIZ) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	sscanf(buf, "%d %d %ld %ld %ld %ld %ld %ld", &size, &repetition, min, max, avg, min+1, max+1, avg+1);

	printf("### Received the measurement statistics ###\n");
	printf("The elements in the input data file : %d\n", size);
	printf("The number of repetitions : %d\n", repetition);

	printf("Insertion Sort\n");
	printf("- The minimum elapsed time : %ld millseconds\n", min[0]);
	printf("- The maximum elapsed time : %ld millseconds\n", max[0]);
	printf("- The average elapsed time : %ld millseconds\n", avg[0]);

	printf("Quick Sort\n");
	printf("- The minimum elapsed time : %ld millseconds\n", min[1]);
	printf("- The maximum elapsed time : %ld millseconds\n", max[1]);
	printf("- The average elapsed time : %ld millseconds\n", avg[1]);
	//	send 'k' to sock1
	sprintf(buf, "k");
	if(write(sock1, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write 'k' error\n");
		exit(1);
	}

	return 0;
}



