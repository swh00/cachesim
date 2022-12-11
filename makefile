#makefile

CC = gcc -g

#
all : csim

#
csim : cacheSim.o cache.o
	$(CC) -o csim cacheSim.o cache.o

#
cacheSim.o : cacheSim.c 
	$(CC) -c cacheSim.c -o cacheSim.o

#
cache.o : cache.c cache.h
	$(CC) -c cache.c -o cache.o

#
clean : 
	rm *.o csim