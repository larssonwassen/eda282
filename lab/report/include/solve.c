void Solve(double **A) {
    int i, j, done = 0;
    double temp, diff;
    #pragma omp parallel default(shared) private(i, j, temp)    
    while(!done){
   	 	#pragma omp single
			diff = 0;
			#pragma omp for schedule(static) reduction(+:diff)
			for(i=1; i<=n; ++i) {
			    for(j=i%2 ? 1:2; j<=n; j=j+2) {
					temp = A[i][j];
					A[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
					diff += abs(A[i][j] - temp);
			    }
			}
			#pragma omp for schedule(static) reduction(+:diff)
			for(i=1; i<=n; ++i) {
			    for(j=i%2 ? 2:1; j<=n; j=j+2) {
					temp = A[i][j];
					A[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
					diff += abs(A[i][j] - temp);
			    }
			}
			#pragma omp single {
				if(diff / (n*n) < 0.01)
				    done = 1;
	    	}
    }
}