#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** configuration(int game_size, int iter_number) {
	int i;

	char** game = (char**) malloc(iter_number * sizeof(char*));
	for (i = 0; i < iter_number; i ++) {
		game[i] = (char*) malloc(game_size * sizeof(char));
	}

	for (i = 0; i < game_size; i ++) {
		game[0][i] = rand() % 2;
	}

	return game;
}

void print_game(char **game_table, int iter_number,  int size) {
	int i, j;

	for (i = 0; i < iter_number; i ++) {
		for (j = 0; j < size; j ++) {
			printf("%c ", 48 + game_table[i][j]);	
		}
		printf("\n");	
	}
}


int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int iter_number = 1000;
	int size = 1000000;
	// int iter_number = 10;
	// int size = 1000;

	int iter;
	int i;
	int chunk_size = size / (world_size - 1);

	MPI_Status status;
	
	if (world_rank == 0) {
		char **game_table = configuration(size, iter_number);
		char *data_from_neighbours = (char*) malloc(size * sizeof(char));
		int vecin;
		for (iter = 1; iter < iter_number; iter ++) {
			for (vecin = 1; vecin < world_size; vecin ++) {
				MPI_Send(game_table[iter-1], size, MPI_CHAR, vecin, 0, MPI_COMM_WORLD);
			}
			for (vecin = 1; vecin < world_size; vecin ++) {
				MPI_Recv(data_from_neighbours, size, MPI_CHAR, vecin, 0, MPI_COMM_WORLD, &status);
				int lower_bound = (status.MPI_SOURCE - 1) * chunk_size;
				int upper_bound = status.MPI_SOURCE * chunk_size;
				memcpy(game_table[iter] + lower_bound, data_from_neighbours + lower_bound, chunk_size);
			}
		}
		free(data_from_neighbours);
		// print_game(game_table, iter_number, size);
	} else {
		char *last_iter = (char*) malloc(size * sizeof(char));
		char *current_itter  = (char*)malloc(size * sizeof(char));
		
		int lower_bound = (world_rank - 1) * chunk_size;
		int upper_bound = world_rank * chunk_size;
		int omp_chunk = chunk_size / 4;

		#pragma omp parallel private(iter)
		for (iter = 1; iter < iter_number; iter++) {
			#pragma omp single
			{
				memset(last_iter, '\0', size);
				MPI_Recv(last_iter, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
				memset(current_itter, '\0', size);
			}
			#pragma omp parallel for private(i) schedule(static, omp_chunk)
			for (i = lower_bound; i < upper_bound; i ++) {
				if (i == 0) {
					current_itter[0] = last_iter[1] ^ last_iter[size-1];
				} else if (i == size - 1) {
					current_itter[i] = last_iter[0] ^ last_iter[size-2];
				}
				else {
					current_itter[i] = last_iter[i - 1] ^ last_iter[i + 1];
				}
			}
			#pragma omp single
			{
				MPI_Send(current_itter, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			}
		}

		free(last_iter);
		free(current_itter);
	}



    // Finalize the MPI environment.
    MPI_Finalize();
}