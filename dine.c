/*
 * ----------------------------------------------------------------------
 *  dine.c - A simple implementation of the dining philosopher's problem.
 *  usage: ./dine [philosophers] [meals]
 *
 *  Author: Matthew Breckon
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
 
sem_t lock;

/* argument structure to feed multiple arguments to philosopher pthread function */
struct arg_struct {
    int meals, philosopher;
    sem_t left_fork, right_fork;
};

/* utility sleep function with support for decimal seconds */
void rest(float s) {
	int time = s * 1000000000;
	nanosleep((struct timespec[]){{0, time}}, NULL);
}

/* philosopher function performs thinking and eating operations */
void *philosopher(void *args) {
    struct arg_struct *passed_args = args;
   
    int i;
    for(i = 0; i < passed_args->meals; i++) {
    	/* lock prevents delock of 5 philosopher's grabbing all 5 forks */
        sem_wait(&lock); //
        printf("Philosopher %d thinking\n", passed_args->philosopher);
        rest(0.25);
        /* current philosopher grabs left then right fork */
        sem_wait(&(passed_args->left_fork));
        sem_wait(&(passed_args->right_fork));
        printf("Philosopher %d eating\n", passed_args->philosopher);
        rest(0.25);
        /* current philosopher releases left then right fork */
        sem_post(&(passed_args->left_fork));
        sem_post(&(passed_args->right_fork));
        sem_post(&lock);
    }
    /* exit thinking statement */
    printf("Philosopher %d thinking\n", passed_args->philosopher);
    rest(0.25);
    pthread_exit(NULL);
}
 
void table(int p, int m) {
    /* initialize lock semaphore to avoid all philosophers from grabbing a fork simultaneously */
    sem_init(&lock, 0, p - 1);

    /* initialize forks equal to number of philosophers */
    sem_t forks[p];
    int i;
    for(i = 0; i < p; i++) sem_init(&forks[i], 0, 1);    
 
    /* initialize philosopher threads */
    struct arg_struct current_philosopher[p];
    pthread_t thread[p];
    for(i = 0; i < p; i++) {
        /* set up current philosopher structure to pass variables to pthread */
        current_philosopher[i].philosopher = i + 1;
        current_philosopher[i].meals = m;
        current_philosopher[i].left_fork = forks[i];
        if(i + 1 == p) current_philosopher[i].right_fork = forks[0]; //if last philosopher, use first fork
        else current_philosopher[i].right_fork = forks[i + 1];
	}
	/* create each philosopher pthreads */
	for(i = 0; i < p; i++) {
        pthread_create(&thread[i], NULL, philosopher, (void*)&current_philosopher[i]);
    }
    /* join philosopher threads */
    for(i = 0; i < p; i++) {
        pthread_join(thread[i], NULL);
    }
    for(i = 0; i < p; i++) {
        sem_destroy(&forks[i]);
    }
    sem_destroy(&lock);
}
 
int main(int argc, char *argv[]) {
    /* check arguments */
    if(argc != 3) {
        printf("error: invalid argument total\nusage: ./dine [philosophers] [meals]\n");
        exit(EXIT_FAILURE);
    }
    int philosophers, meals;
    /* set philosophers */
    if((philosophers = atoi(argv[1])) <= 1) {
        printf("error: philosophers set to %d; must be > 1\n", philosophers);
        exit(EXIT_FAILURE);
    }
    /* set meals */
    if((meals = atoi(argv[2])) <= 0) {
        printf("error: meals set to %d; must be > 0\n", meals);
        exit(EXIT_FAILURE);
    }
    table(philosophers, meals);
    return 0;
}