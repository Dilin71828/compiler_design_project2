#include "../run2.h"

void grad_case2(float (&A)[4][16], float (&dB)[4][16], float (&dA)[4][16])  {
  float TEMP__1[4][16];
  float TEMP__2[4][16];
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      TEMP__1[i][j]=0;
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+dB[i][j]*A[i][j];
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      dA[i][j]=TEMP__1[i][j];
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      TEMP__2[i][j]=0;
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      TEMP__2[i][j]=TEMP__2[i][j]+A[i][j]*dB[i][j];
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      TEMP__2[i][j]=TEMP__2[i][j]+dA[i][j];
    }
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 16; j++){
      dA[i][j]=TEMP__2[i][j];
    }
  }
}
