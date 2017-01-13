#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	4

// barriers

#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__


typedef struct thread_struct {
  int thread_number;
  int thread_size;
  int iter_number;
  char **game_table;
  int game_size;
} thread_struct;

pthread_barrier_t barrier;


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


void *PrintHello(void *thread_arg)
{
   thread_struct *arg = (thread_struct*) thread_arg;
   char** game_table = arg->game_table;
   int lower_bound = arg->thread_number * arg->thread_size;
   int upper_bound = (arg->thread_number + 1) * arg->thread_size;
   int size = arg->game_size;
   int i, iter;

   for (iter = 1; iter < arg->iter_number; iter ++) {
    for (i = lower_bound; i < upper_bound; i ++ ) {
      if (i == 0) {
        game_table[iter][i] = game_table[iter - 1][1] ^ game_table[iter - 1][size - 1];
      } else if (i == size - 1) {
        game_table[iter][i] = game_table[iter - 1][0] ^ game_table[iter - 1][size - 2];
      }
      else {
        game_table[iter][i] = game_table[iter - 1][i - 1] ^ game_table[iter - 1][i + 1];
      }
    }
    
    // Synchronization point
    int rc = pthread_barrier_wait(&barrier);
  }

   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

  int iter_number = 1000;
  int size = 1000000;


  int iter;
  int i;

  char **game_table = configuration(size, iter_number);

  thread_struct *arg_struct;

  pthread_t threads[NUM_THREADS];
  int rc;
  long t;


  if(pthread_barrier_init(&barrier, NULL, NUM_THREADS))
    {
        printf("Could not create a barrier\n");
        return -1;
    }

  for(t = 0; t < NUM_THREADS; t ++){
   
   arg_struct = (thread_struct*) malloc(sizeof(thread_struct));
   arg_struct->thread_number = t;
   arg_struct->thread_size = size / NUM_THREADS;
   arg_struct->iter_number = iter_number;
   arg_struct->game_table = game_table;
   arg_struct->game_size = size;

   rc = pthread_create(&threads[t], NULL, PrintHello, (void *)arg_struct);
   if (rc){
     printf("ERROR; return code from pthread_create() is %d\n", rc);
     exit(-1);
   }
  }

  for(t = 0; t < NUM_THREADS; t++){
      pthread_join(threads[t], NULL);
  }

  // print_game(game_table, iter_number, size);

  for (i = 0; i < iter_number; i ++) {
    free(game_table[i]);
  }

  /* Last thing that main() should do */
  return 0;
}