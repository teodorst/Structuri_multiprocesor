	
	// problem size
	int iter_number = 1000;
	int size = 1000000;
    
	// serial
	./serial.out  4.69s user 0.44s system 98% cpu 5.201 total

    // 1 thread / 1 process

    ./omp.out  5.04s user 0.44s system 98% cpu 5.563 total
    ./pthreads.out  4.87s user 0.54s system 90% cpu 5.968 total
	mpirun -n 2 ./mpi.out  10.37s user 0.57s system 186% cpu 5.854 total
	mpirun -n 2 ./hib.out  8.94s user 0.57s system 192% cpu 4.939 total

    // 2 thread / 2 process
    ./omp.out  5.00s user 0.51s system 188% cpu 2.920 total
    ./pthreads.out  4.91s user 0.55s system 186% cpu 2.919 total
    mpirun -n 3 ./mpi.out  9.87s user 1.93s system 262% cpu 4.489 total
    mpirun -n 3 ./hib.out  9.99s user 1.93s system 260% cpu 4.581 total


    // 4 thread / 4 process
    ./omp.out  5.37s user 0.60s system 185% cpu 3.214 total
    ./pthreads.out  7.76s user 0.72s system 310% cpu 2.734 total
    mpirun -n 5 ./mpi.out  10.47s user 3.69s system 324% cpu 4.358 total
    mpirun -n 5 ./hib.out  10.78s user 3.73s system 323% cpu 4.478 total