#include"socklib.h"

#define HOSTNAME "155.230.105.104"
#define RESULT_I "insertion_sort_res.txt"
#define RESULT_Q "quick_sort_res.txt"
#define SWAP(x,y,t) (t)=(x), (x)=(y), (y)=(t)

int sizecheck(char *);
void inputdata(char *, int *);
void insert(int, int *, int);
void insertionSort(int *, int);
void quickSort(int *, int, int, int);

int main(int ac, char *av[])
{
    int sock1, fd1;
	int sock2, fd2;
	char buf1[10], buf2[BUFSIZ];
	char buf[BUFSIZ];

	int *arr1, *arr2;	
	char filename[100];
	int repetition, size;

	int i, j;
	struct timeval start, finish;
	long timeresult;
	long min[2], max[2], sum[2];
	FILE *fpout_i, *fpout_q;

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

    if(ac != 3)
    {
		fprintf(stderr, "./sock port1 port2\n");
		exit(1);
    }
	//////////////////////////////////////////////////////////
	//						socket 1						//		
	//////////////////////////////////////////////////////////
    if((sock1 = make_server_socket(atoi(av[1]))) == -1)
    {
		fprintf(stderr, "socket 1 error\n");
		exit(1);
	}
	//////////////////////////////////////////////////////////
	//						socket 2						//
	//////////////////////////////////////////////////////////
	if((sock2 = make_server_socket(atoi(av[2]))) == -1)
    {
		fprintf(stderr, "socket 2 error\n");
		exit(1);
	}
	//////////////////////////////////////////////////////////
	//						execute							//
	//////////////////////////////////////////////////////////
	if((fd1 = accept(sock1, NULL, NULL)) == -1)
	{
		fprintf(stderr, "accept sock1 error\n");
		exit(1);
	}
	if((fd2 = accept(sock2, NULL, NULL)) == -1)
	{
		fprintf(stderr, "accept sock2 error\n");
		exit(1);
	}
	//	set array for result
	for(i=0; i<2; i++)
	{
		max[i] = 0;
		min[i] = LONG_MAX;
		sum[i] = 0;
	}
	//	wait to receive 'm' from sock1
	if(read(fd1, buf1, sizeof(buf1)) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	if(strcmp(buf1, "m") == 0)
		printf("Received  '%s'\n", buf1);
	else
	{
		printf("Received  '%s' but expect 'm'\n", buf1);
		exit(1);
	}
	//	wait to receive filename and repetiton from sock2
	if(read(fd2, buf2, sizeof(buf2)) == -1)
	{
		fprintf(stderr, "read sock2 error\n");
		exit(1);
	}
	sscanf(buf2, "%s %d", filename, &repetition);
	
	size = sizecheck(filename);
	arr1 = malloc((size+1)*sizeof(int));
	arr2 = malloc((size+1)*sizeof(int));
	inputdata(filename, arr1);
	//	send 'd' to sock1
	sprintf(buf, "d");
	if(write(fd1, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write 'd' error\n");
		exit(1);
	}
	//	wait to receive 's' from sock1	
	if(read(fd1, buf1, sizeof(buf1)) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	if(strcmp(buf1, "s") == 0)
		printf("Received  '%s'\n", buf1);
	else
	{
		printf("Received  '%s' but expect 's'\n", buf1);
		exit(1);
	}
	//	start sorting
	if((fpout_i = fopen(RESULT_I, "w")) == NULL)
	{
		fprintf(stderr, "cannot make INSERTION result file\n");
		exit(1);
	}
	if((fpout_q = fopen(RESULT_Q, "w")) == NULL)
	{
		fprintf(stderr, "cannot make QUICK result file\n");
		exit(1);
	}
	for(i=0; i<repetition; i++)
	{
		//	INSERTION SORT
		for(j=1; j<size+1; j++)
			*(arr2+j) = *(arr1+j);
		
		gettimeofday(&start, NULL);
		insertionSort(arr2, size);
		gettimeofday(&finish, NULL);

		timeresult = (finish.tv_sec - start.tv_sec)*1000;
		timeresult += (finish.tv_usec - start.tv_usec)/1000;

		if(max[0] < timeresult)
			max[0] = timeresult;
		if(min[0] > timeresult)
			min[0] = timeresult;
		sum[0] += timeresult;

		if(i == repetition-1)
		{
			for(j=1; j<size+1; j++)
				fprintf(fpout_i, "%d ", *(arr2+j));
		}
		//	QUICK SORT
		for(j=1; j<size+1; j++)
			*(arr2+j) = *(arr1+j);
		
		gettimeofday(&start, NULL);
		quickSort(arr2, 1, size, size);
		gettimeofday(&finish, NULL);
	
		timeresult = 0;
		timeresult = (finish.tv_sec - start.tv_sec)*1000;
		timeresult += (finish.tv_usec - start.tv_usec)/1000;

		if(max[1] < timeresult)
			max[1] = timeresult;
		if(min[1] > timeresult)
			min[1] = timeresult;
		sum[1] += timeresult;

		if(i == repetition-1)
		{
			for(j=1; j<size+1; j++)
				fprintf(fpout_q, "%d ", *(arr2+j));
		}
	}
	//	send 'f' to sock1
	sprintf(buf, "f");
	if(write(fd1, buf, strlen(buf)+1) == -1)
	{
		fprintf(stderr, "write 'f' error\n");
		exit(1);
	}
	//	send data to sock2
	sprintf(buf2, "%d %d %ld %ld %ld %ld %ld %ld", size, repetition, min[0], max[0], sum[0]/repetition, min[1], max[1], sum[1]/repetition);
	if(write(fd2, buf2, strlen(buf2)+1) == -1)
	{
		fprintf(stderr, "write data error\n");
		exit(1);
	}
	//	wait to receive 'k' from sock1	
	if(read(fd1, buf1, sizeof(buf1)) == -1)
	{
		fprintf(stderr, "read sock1 error\n");
		exit(1);
	}
	if(strcmp(buf1, "k") == 0)
		printf("Received  '%s'\n", buf1);
	else
	{
		printf("Received  '%s' but expect 'k'\n", buf1);
		exit(1);
	}

	free(arr1);
	free(arr2);
	fclose(fpout_i);
	fclose(fpout_q);
	return 0;
}

int sizecheck(char *filename)
{
	FILE *fp = fopen(filename, "r");
	int temp, size = 0;

	while(fscanf(fp, "%d", &temp) != EOF)
		size++;

	fclose(fp);
	return size;
}

void inputdata(char *filename, int *arr)
{
	FILE *fp = fopen(filename, "r");
	int temp, i = 1;

	while(fscanf(fp, "%d", &temp) != EOF)
	{
		*(arr+i) = temp;
		i++;
	}
	
	fclose(fp);
}

void insert(int e, int *arr2, int i)
{
	arr2[0] = e;

	while(e < arr2[i])
	{
		arr2[i+1] = arr2[i];
		i--;
	}
	arr2[i+1] = e;
}

void insertionSort(int *arr2, int size)
{
	int j, temp;

	for(j=2; j<=size; j++)
	{
		temp = arr2[j];
		insert(temp, arr2, j-1);
	}
}

void quickSort(int *arr2, int left, int right, int size)
{
	int pivot, i, j;
	int temp = 0;

	if(left<right)
	{
		i = left;
		j = right+1;
		pivot = arr2[left];
	
	do
	{
		do
		{
			i++;
		}while(arr2[i] < pivot);
		do
		{
			j--;
		}while(arr2[j] > pivot);
		
		if(i<j)
			SWAP(arr2[i], arr2[j], temp);
	}while(i<j);

	SWAP(arr2[left], arr2[j], temp);

	quickSort(arr2, left, j-1, size);
	quickSort(arr2, j+1, right, size);
	}
}
