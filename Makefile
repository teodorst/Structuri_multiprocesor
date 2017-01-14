CC=gcc
MPICC=mpicc
CFLAGS=-Wall -g

all: rule90_serial rule90_pthreads rule90_mpi rule90_omp role90_hibrid

rule90_serial: rule90_serial.o
	$(CC) $(CFLAGS) -o $@ $^

rule90_pthreads: rule90_pthreads.o
	$(CC) $(CFLAGS) -pthread -o $@ $^

rule90_mpi: rule90_mpi.o
	$(MPICC) $(CFLAGS) -o $@ $^

rule90_omp: rule90_omp.o
	$(CC) $(CFLAGS) -o $@ $^

role90_hibrid: role90_hibrid.o
	$(MPICC) $(CFLAGS) -o $@ $^



rule90_serial.o: rule90_serial.c
	$(CC) $(CFLAGS) -c -o $@ $^

rule90_pthreads.o: rule90_pthreads.c
	$(CC) $(CFLAGS) -c -o $@ $^

rule90_mpi.o: rule90_mpi.c
	$(MPICC) $(CFLAGS) -c -o $@ $^

rule90_omp.o: rule90_omp.c
	$(CC) $(CFLAGS) -c -o $@ $^

role90_hibrid.o: role90_hibrid.c
	$(MPICC) $(CFLAGS) -c -o $@ $^

clean:
	rm rule90_serial rule90_pthreads rule90_mpi rule90_omp role90_hibrid *.o



