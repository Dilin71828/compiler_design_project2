#include "../run2.h"

void grad_case3(float (&B)[16][16], float (&dC)[4][16], float (&dA)[4][16])  {
  float TEMP__1[4][16];
  for( int i = 0; i < 4; i++){
    for( int k = 0; k < 16; k++){
      TEMP__1[i][k]=0;
    }
  }
  for( int i = 0; i < 4; i++){
    for( int k = 0; k < 16; k++){
      for( int j = 0; j < 16; j++){
        TEMP__1[i][k]=TEMP__1[i][k]+dC[i][j]*B[k][j];
      }
    }
  }
  for( int i = 0; i < 4; i++){
    for( int k = 0; k < 16; k++){
      dA[i][k]=TEMP__1[i][k];
    }
  }
}
