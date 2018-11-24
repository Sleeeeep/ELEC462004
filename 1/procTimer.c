#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<limits.h>

#define RESULT_I "insertion_sort_res.txt"
#define RESULT_Q "quick_sort_res.txt"
#define SWAP(x,y,t) (t)=(x), (x)=(y), (y)=(t)

int stringToint(char *av);
void childfunc(int thepipe[], int num, int size, int *dataArr);
void insertt(int e, int *sortedArr, int i);
void insertionSort(int *sortedArr, int size);
void quickSort(int *sortedArr, int left, int right, int size);

int main(int ac, char *av[])
{
    FILE *fp;
    int num, size = 0, pid, thepipe[2];
    int temp, i;
    int *dataArr;
    long timetemp;

    signal(SIGINT, SIG_IGN);	    // crtl + C ignore
    signal(SIGTERM, SIG_IGN);	    // crtl + Q ignore

    if(ac != 3)
    {
	fprintf(stderr, "incorrect form\n(./procTimer filename repetition)\n");
	exit(1);
    }
    if((fp = fopen(av[1], "r")) == NULL)
    {
	fprintf(stderr, "cannot find the data file\n");
	exit(1);
    }
    
    num = stringToint(av[2]);	    // string to int for repetition
    
    while(fscanf(fp, "%d", &temp) != EOF)	// count the number of integer
    {
	if(temp == '\r' || temp == '\n')
	    continue;
	size++;
    }
    fseek(fp, 0, SEEK_SET);	// fp reset

    dataArr = (int *)malloc((size+1) * sizeof(int));
    
    for(i = 1; i<size+1; i++)
	fscanf(fp, "%d", (dataArr+i));	// input data into dynamic array
    fclose(fp);
    
    if(pipe(thepipe) == -1)
    {
	fscanf(stderr, "cannot pipe");
	exit(1);
    }
    if((pid=fork()) == -1)
    {
	fscanf(stderr, "cannot fork");
	exit(1);
    }
    
    if(pid == 0)	// child
    {
	childfunc(thepipe, num, size, dataArr);
    }
    else
    {
	close(thepipe[1]);	// parent only read
	if(dup2(thepipe[0], 0) == -1)
	{
	    fprintf(stderr, "dup2 error in parent\n");
	    exit(1);
	}
	close(thepipe[0]);

	wait(NULL);
	
	printf("The elements in the input data file : %d\n", size);
	printf("The number of repetitions : %d\n", num);

	printf("Insertion Sort\n");
	scanf("%ld", &timetemp);
	printf("- The minimum elapsed time : %ld millseconds\n", timetemp);
 	scanf("%ld", &timetemp);
	printf("- The maximum elapsed time : %ld millseconds\n", timetemp);
       	scanf("%ld", &timetemp);
	printf("- The average elapsed time : %ld millseconds\n", timetemp);
    	
	printf("Quick Sort\n");
	scanf("%ld", &timetemp);
	printf("- The minimum elapsed time : %ld millseconds\n", timetemp);
    	scanf("%ld", &timetemp);
	printf("- The maximum elapsed time : %ld millseconds\n", timetemp);
    	scanf("%ld", &timetemp);
	printf("- The average elapsed time : %ld millseconds\n", timetemp);
    }
    
    free(dataArr);
    return 0;
}

void childfunc(int thepipe[], int num, int size, int *dataArr)
{
    FILE *fp_i, *fp_q;
    int *sortedArr = (int *)malloc((size+1) * sizeof(int));
    int i, j;
    struct timeval start, finish;    
    long timeresult;
    long max[2], min[2], sum[2];	// 0 - insertion , 1 - quick
    
    if((fp_i = fopen(RESULT_I, "w")) == NULL)
    {
	fprintf(stderr, "cannot open insertion result\n");
	exit(1);
    }
    if((fp_q = fopen(RESULT_Q, "w")) == NULL)
    {
	fprintf(stderr, "cannot open quick result\n");
	exit(1);
    }

    close(thepipe[0]);	    // child only write
    if(dup2(thepipe[1], 1) == -1)
    {
	fprintf(stderr, "dup2 error in child\n");
	exit(1);
    }
    close(thepipe[1]);

    for(i=0; i<2; i++)
    {
	max[i] = 0;
	min[i] = LONG_MAX;
	sum[i] = 0;
    }

    for(i=0; i<num; i++)
    {
	for(j=1; j<size+1; j++)		// copy data
	    *(sortedArr+j) = *(dataArr+j);
	
	gettimeofday(&start, NULL);	
	
	insertionSort(sortedArr, size);
	
	gettimeofday(&finish, NULL);

	timeresult = (finish.tv_sec - start.tv_sec) * 1000;
       	timeresult += (finish.tv_usec - start.tv_usec)/1000;
	
	if(max[0] < timeresult)
	    max[0] = timeresult;
	if(min[0] > timeresult)
	    min[0] = timeresult;
	sum[0] += timeresult;

	if(i == num-1)
	{
	    for(j=1; j<size+1; j++)
		fprintf(fp_i, "%d ", *(sortedArr+j));		
	}
	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	for(j=1; j<size+1; j++)		// copy data
	    *(sortedArr+j) = *(dataArr+j);
	
	gettimeofday(&start, NULL);	

	quickSort(sortedArr,1, size, size);
	
	gettimeofday(&finish, NULL);

	timeresult = (finish.tv_sec - start.tv_sec) * 1000;
       	timeresult += (finish.tv_usec - start.tv_usec)/1000;
	
	if(max[1] < timeresult)
	    max[1] = timeresult;
	if(min[1] > timeresult)
	    min[1] = timeresult;
	sum[1] += timeresult;
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	if(i == num-1)
	{
	    for(j=1; j<size+1; j++)
		fprintf(fp_q, "%d ", *(sortedArr+j));
	}

    }

    printf("%ld\n", min[0]);
    printf("%ld\n", max[0]);
    printf("%ld\n", sum[0]/num);
    
    printf("%ld\n", min[1]);
    printf("%ld\n", max[1]);
    printf("%ld\n", sum[1]/num);
    
    free(dataArr);
    free(sortedArr); 
    fclose(fp_i);
    fclose(fp_q);
    exit(0);
}

int stringToint(char *av)
{
    int i, temp = 0;
    
    for(i=0; av[i]; i++)
    {
	if(av[i] == '\n' || av[i] == '\r')
	    continue;
	if(av[i] < 48 || av[i] > 57)
	{
	    fprintf(stderr, "inappropriate input in av[2]\n");
	    exit(1);
	}
	temp = temp * 10 + av[i] - '0';
    }

    return temp;
}

void insert(int e, int *sortedArr, int i)
{
    sortedArr[0] = e;
    while(e < sortedArr[i])
    {
	sortedArr[i+1] = sortedArr[i];
	i--;
    }
    sortedArr[i+1] = e;
}

void insertionSort(int *sortedArr, int size)
{
    int j, temp;

    for(j=2; j<=size; j++)
    {
	temp = sortedArr[j];
	insert(temp, sortedArr, j-1);
    }
}

void quickSort(int *sortedArr, int left, int right, int size)
{
    int pivot, i, j, n;
    int temp = 0;

    if(left<right)
    {
	i = left;
      	j = right+1;
	pivot = sortedArr[left];

	do
	{
	    do
	    {
		i++;
	    }while(sortedArr[i] < pivot);
	    do
	    {
		j--;
	    }while(sortedArr[j] > pivot);

	    if(i < j)
		SWAP(sortedArr[i], sortedArr[j], temp);

	}while(i < j);

	SWAP(sortedArr[left], sortedArr[j], temp);

	quickSort(sortedArr, left, j-1, size);
	quickSort(sortedArr, j+1, right, size);
    }
}
