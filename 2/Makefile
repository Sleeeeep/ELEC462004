RM = rm -f

sort: procTimer sort.o socklib.o
	gcc -o sort sort.o socklib.o

procTimer: procTimer.o socklib.o
	gcc -o procTimer procTimer.o socklib.o

socklib.o: socklib.c socklib.h
	gcc -Wall -g -c socklib.c

sort.o: sort.c socklib.h
	gcc -Wall -g -c sort.c

procTimer.o: procTimer.c socklib.h
	gcc -Wall -g -c procTimer.c

clean:
	$(RM) *.o
