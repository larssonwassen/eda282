
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
omp_lock_t delta_lock;
omp_lock_t new_centers_locks*;
omp_lock_t new_centers_len_locks**;

// This function must be called before kernel is furst run.
void init_locks(int nclusters) {
  omp_init_lock(&delta_lock);
  for(int n = 0; n < nclusters; n++) {
    omp_init_lock(&new_centers_locks[n][0]);
    omp_init_lock(&new_centers_locks[n][1]);
    omp_init_lock(&new_centers_len_locks[n]);
  }
}
// This function is run after kernel are finished.
void destroy_locks(int nclusters) {
  omp_destroy_lock(&delta_lock);
  for(int n = 0; n < nclusters; n++) {
    omp_destroy_lock(&new_centers_locks[n][0]);
    omp_destroy_lock(&new_centers_locks[n][1]);
    omp_destroy_lock(&new_centers_len_locks[n]);
  }
}

int kernel(int npoints, int nclusters) {
  
  int i, j, delta = 0, index;

  #pragma omp parallel for default(shared) \
                            private(i,j,index) \
                            schedule(static)
  
  for(i = 0; i < npoints; i++) {
    
    index = find_nearest_point(feature[i], 2, clusters, nclusters);
    
    if (membership[i] != index) {
      omp_set_lock(&delta_lock);
        delta += 1;
      omp_unset_lock(&delta_lock);
    }
    
    membership[i] = index;
    
    omp_set_lock(&new_centers_len_locks[index]);
      new_centers_len[index]++;
    omp_unset_lock(&new_centers_len_locks[index]);

    for(j = 0; j < 2; j++) {
      omp_set_lock(&new_centers_locks[index][j]);
        new_centers[index][j] += feature[i][j];
      omp_unset_lock(&new_centers_locks[index][j]);   
    }
  }

  return delta;
}