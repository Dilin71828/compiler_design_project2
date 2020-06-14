#include "../run.h"

void kernel_case10(float (&B)[10][10], float (&A)[8][8])  {
  float TEMP__1[8][8];
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=0;
    }
  }
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 8; j++){
      if (i+1 < 10 && i+2 < 10) {
        TEMP__1[i][j]=TEMP__1[i][j]+(B[i][j]+B[i+1][j]+B[i+2][j])/3;
      } else {
        
      }
    }
  }
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 8; j++){
      A[i][j]=TEMP__1[i][j];
    }
  }
}
