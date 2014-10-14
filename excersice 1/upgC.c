
int kernel(int npoints, int nclusters) {

  int i, j, delta = 0, index;
  
  #pragma omp parallel for default(shared) \
                            private(i,j,index) \
                            schedule(static) \
                            reduction(+:delta)

  for(i = 0; i < npoints; i++) {
    
    index = find_nearest_point(feature[i], 2, clusters, nclusters);
    
    if (membership[i] != index) {
      delta += 1;
    }
    
    membership[i] = index;
    
    #pragma omp critical
    { 
      new_centers_len[index]++;
      for(j = 0; j < 2; j++) {
        new_centers[index][j] += feature[i][j];
      }
    }

  }
  return delta;
}

int kernel(int npoints, int nclusters) {
  
  int i, j, delta = 0, index;
  
  #pragma omp parallel for default(shared) \
                            private(i,j,index) \
                            schedule(static) \
                            reduction(+:delta)
  
  for(i = 0; i < npoints; i++) {
    
    index = find_nearest_point(feature[i], 2, clusters, nclusters);
    
    if (membership[i] != index) {
      delta += 1;
    }
    
    membership[i] = index;
    
    #pragma omp atomic
    new_centers_len[index]++;

    for(j = 0; j < 2; j++) {
      #pragma omp atomic
      new_centers[index][j] += feature[i][j];
    }

  }

  return delta;
}