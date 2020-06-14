#include "../run.h"

void kernel_case3(int (&B)[16][32], int (&C)[16][32], int (&A)[16][32])  {
  int TEMP__1[16][32];
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 32; j++){
      TEMP__1[i][j]=0;
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 32; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+B[i][j];
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 32; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+C[i][j];
    }
  }
  for( int i = 0; i < 16; i++){
    for( int j = 0; j < 32; j++){
      A[i][j]=TEMP__1[i][j];
    }
  }
}
