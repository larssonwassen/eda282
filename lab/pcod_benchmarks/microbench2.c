#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define CSIZE 16

static pthread_mutex_t lock;
static pthread_barrier_t barrier;

int* counterArray;

int nthreads;
int ncounters;

void* chain(void* arglist)
{
    int i, j;
    long tid = (long) arglist;
 
    for (i = 0; i < nthreads; i++) { 

        for (j = 0; j < ncounters; j++)
	    if(j%nthreads == tid)
                counterArray[j*CSIZE]++;
       
        pthread_barrier_wait(&barrier);
    }
        
    return NULL;
}


int main(int argc, const char** const argv)
{
    if (argc != 3) {
       printf("Usage: ./microbench2 <nthreads> <ncounters>\n");
       exit(1);
    }
    
    nthreads = atoi(argv[1]);
    ncounters = atoi(argv[2]);

    if (nthreads < 2 || ncounters < 1) {
        printf("This test requires at least 2 CPUs, 1 counter variable\n");
        exit(1);
    }
    
    long i;
    pthread_t pth[nthreads];
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    pthread_mutex_init(&lock, NULL);
    printf("Init done\n");
    
    
    pthread_barrier_init(&barrier, NULL, nthreads);
    
    counterArray = (int*) calloc(ncounters*CSIZE, sizeof(int));
    
    for (i = 0; i < nthreads; i++) {
        pthread_create(&pth[i], &attr, chain, (void*)i);
    }
    
    for (i = 0; i < nthreads; i++) { 
        pthread_join(pth[i], NULL);
    }
    
    printf("Val: %d\n", counterArray[0]);
    printf("PASSED :-)\n");
    return 0;
}
