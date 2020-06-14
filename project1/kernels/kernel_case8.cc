#include "../run.h"

void kernel_case8(float (&B)[8][16], float (&A)[8][2][16])  {
  float TEMP__1[8][2][16];
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 2; j++){
      for( int k = 0; k < 16; k++){
        TEMP__1[i][j][k]=0;
      }
    }
  }
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 2; j++){
      for( int k = 0; k < 16; k++){
        TEMP__1[i][j][k]=TEMP__1[i][j][k]+B[i][k];
      }
    }
  }
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 2; j++){
      for( int k = 0; k < 16; k++){
        A[i][j][k]=TEMP__1[i][j][k];
      }
    }
  }
}
