/* The sequential version of equation solver kernel used in Ocean */
/* The Solve function is implemented with red-black ordering mechanism */
/* You should only modify the Solve function to paralize the kernel */

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>


int n;
double **A, diff;
int nthreads;

void print(double **A);
int input(char *filename);
void Solve(double **A);

int main(int argc, char **argv)
{

    if(argc != 3) {
	printf("Usage: %s <input file> <nthreads>\n", argv[0]);
	exit(1);
    }
            
    nthreads = atoi(argv[2]);
    if(input(argv[1]) < 0 && nthreads < 0 ) {
	printf("Usage error occured while opening/reading file, make sure file exists and is in the correct format or nthreads > 0\n");
	exit(1);
    }

    //Set number of threads in the parallel version
    omp_set_num_threads(nthreads);

    //kernel call
    Solve(A);

    //print(A);

    return 0;
}


void Solve(double **A)
{
    int i, j, done = 0;
    double temp;
	double diff;

    #pragma omp parallel default(shared) private(i, j, temp)    
    while(!done){

   	 	#pragma omp single
		diff = 0;

		/* sweep that updates red elements */
		#pragma omp for  schedule(static) reduction(+:diff)
		for(i=1; i<=n; ++i){
		    for(j=i%2 ? 1:2; j<=n; j=j+2){
				temp = A[i][j];
				A[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
				diff += abs(A[i][j] - temp);
		    }
		}

		/* sweep that updates black elements */
		#pragma omp for schedule(static) reduction(+:diff)
		for(i=1; i<=n; ++i){
		    for(j=i%2 ? 2:1; j<=n; j=j+2){
				temp = A[i][j];
				A[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
				diff += abs(A[i][j] - temp);
		    }
		}

		#pragma omp single
		{
			if(diff / (n*n) < 0.01)
			    done = 1;

			//printf("%.4lf\n", diff);
			//printf("%lu\n", sizeof(double *));
    	}
    }
}


void print(double **A)
{
    int i, j;
    for(i=1; i<=n; ++i){
	for(j=1; j<=n; ++j)
	    printf("%4.1lf ", A[i][j]);
    
	printf("\n");
    }
    printf("\n");
}


int input(char *filename)
{
    FILE *fPtr;
    int i, j;

    //Open filename...
    if( (fPtr = fopen(filename,"r")) == NULL)
	return -1;

    //Read matrix size...
    if(fscanf(fPtr, "%d", &n) != 1) {
		fclose(fPtr);
		return -2;
    }

    //Allocate memory for matrix...
    A = (double **) malloc( (n+2) * sizeof(double *));

    for(i=0; i<=n+1; i++)
	A[i] = (double *) malloc((n+2)*sizeof(double));

    //Read matrix from file...

    for(i=1; i<=n; i++){
		for(j=1; j<=n; j++){
	    	if(fscanf(fPtr, "%lf", &A[i][j]) != 1) {
				fclose(fPtr);
				return -3;
	    	}
    	} 
    }

    fclose(fPtr);
    return 0;
}
