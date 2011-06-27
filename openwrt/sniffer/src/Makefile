CFLAGS += -DIS_EMB_DEV=1

all: sniff

sniff: sniff.o manifold_routing.o display_functions.o network_functions.o socket_functions.o stats_log_functions.o supporting_functions.o

	$(CC) -o sniff sniff.o manifold_routing.o display_functions.o network_functions.o socket_functions.o stats_log_functions.o supporting_functions.o

sniff.o: sniff.c

	$(CC) $(CFLAGS) -c -o sniff.o sniff.c

manifold_routing.o: manifold_routing.c

	$(CC) $(CFLAGS) -c -o manifold_routing.o manifold_routing.c

display_functions.o: display_functions.c

	$(CC) $(CFLAGS) -c -o display_functions.o display_functions.c

network_functions.o: network_functions.c

	$(CC) $(CFLAGS) -c -o network_functions.o network_functions.c

socket_functions.o: socket_functions.c

	$(CC) $(CFLAGS) -c -o socket_functions.o socket_functions.c

stats_log_functions.o: stats_log_functions.c

	$(CC) $(CFLAGS) -c -o stats_log_functions.o stats_log_functions.c

supporting_functions.o: supporting_functions.c

	$(CC) $(CFLAGS) -c -o supporting_functions.o supporting_functions.c

clean: *.o sniff

	rm *.o
	rm sniff
