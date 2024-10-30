CC 		= gcc
DEBUG	= -g
CFLAGS	= -Wall -std=c99 -pedantic -c $(DEBUG)
LFLAGS	= -std=c99 -Wall -pedantic $(DEBUG)

Simulator: driver.o simulator.o StringUtils.o metadataops.o configops.o simtimer.o
	$(CC) $(LFLAGS) driver.o \
					simulator.o \
					configops.o \
					StringUtils.o \
					metadataops.o \
					simtimer.o \
					-o sim03 -pthread

driver.o: driver.c
	$(CC) $(CFLAGS) driver.c

simulator.o: simulator.c simulator.h
	$(CC) $(CFLAGS) simulator.c

StringUtils.o: StringUtils.c StringUtils.h
	$(CC) $(CFLAGS) StringUtils.c

metadataops.o: metadataops.c metadataops.h
	$(CC) $(CFLAGS) metadataops.c 

configops.o: configops.c configops.h
	$(CC) $(CFLAGS) configops.c 

simtimer.o: simtimer.c simtimer.h
	$(CC) $(CFLAGS) simtimer.c

run:
	./sim03


clean:
	rm -f *.lgf *.o sim03

