CFLAGS += -DIS_EMB_DEV=1

all: inject

inject: inject.o file_handling_functions.o network_functions.o socket_functions.o supporting_functions.o

	$(CC) -o inject inject.o file_handling_functions.o network_functions.o socket_functions.o supporting_functions.o

inject.o: inject.c

	$(CC) $(CFLAGS) -c -o inject.o inject.c

file_handling_functions.o: file_handling_functions.c

	$(CC) $(CFLAGS) -c -o file_handling_functions.o file_handling_functions.c

network_functions.o: network_functions.c

	$(CC) $(CFLAGS) -c -o network_functions.o network_functions.c

socket_functions.o: socket_functions.c

	$(CC) $(CFLAGS) -c -o socket_functions.o socket_functions.c

supporting_functions.o: supporting_functions.c

	$(CC) $(CFLAGS) -c -o supporting_functions.o supporting_functions.c

clean: *.o inject

	rm *.o
	rm inject
