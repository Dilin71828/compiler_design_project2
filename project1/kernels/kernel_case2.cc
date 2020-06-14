#include "../run.h"

void kernel_case2(float (&A)[16][8])  {
  float TEMP__1[16][8];
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=0;
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+A[i][j];
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+2;
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 8; j++){
      A[i][j]=TEMP__1[i][j];
    }
  }
}
