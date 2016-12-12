#include <omp.h>



// #pragma omp parallel private(nthreads, tid)
//   {

//   /* Obtain thread number */
//   tid = omp_get_thread_num();
//   printf("Hello World from thread = %d\n", tid);

//    Only master thread does this 
//   if (tid == 0) 
//     {
//     nthreads = omp_get_num_threads();
//     printf("Number of threads = %d\n", nthreads);
//     }

//   }  /* All threads join master thread and disband */

// }


#include <stdio.h>
#include <stdlib.h>

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

int main() {
  // int iter_number = 1000;
  // int size = 1000000;

  int iter_number = 10;
  int size = 1000;

  int iter;
  int i;
  int chunk = size/2;

  char **game_table = configuration(size, iter_number);

  #pragma omp parallel private(iter) 
  for (iter = 1; iter < iter_number; iter ++) {
    #pragma omp for private(i) schedule(static, chunk)
    for (i = 0; i < size; i ++ ) {
      printf("iter %d - i %d \n", iter, i);
      if (i == 0) {
        game_table[iter][i] = game_table[iter - 1][1] ^ game_table[iter - 1][size-1];
      } else if (i == size - 1) {
        game_table[iter][i] = game_table[iter - 1][0] ^ game_table[iter - 1][size-2];
      }
      else {
        game_table[iter][i] = game_table[iter - 1][i - 1] ^ game_table[iter - 1][i + 1];
      }
    }
  }

  // print_game(game_table, iter_number, size);

  for (i = 0; i < iter_number; i ++) {
    free(game_table[i]);
  }

  return 0;
}