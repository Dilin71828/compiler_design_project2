#include "../run2.h"

void grad_case4(float (&B)[16][32], float (&C)[32][32], float (&dA)[16][32], float (&dB)[16][32], float (&dC)[32][32])  {
  float TEMP__1[16][32];
  float TEMP__2[32][32];
  for( int i = 0; i < 16; i++){
    for( int k = 0; k < 32; k++){
      TEMP__1[i][k]=0;
    }
  }
  for( int i = 0; i < 16; i++){
    for( int k = 0; k < 32; k++){
      for( int j = 0; j < 32; j++){
        TEMP__1[i][k]=TEMP__1[i][k]+dA[i][j]*C[k][j];
      }
    }
  }
  for( int i = 0; i < 16; i++){
    for( int k = 0; k < 32; k++){
      dB[i][k]=TEMP__1[i][k];
    }
  }
  for( int k = 0; k < 32; k++){
    for( int j = 0; j < 32; j++){
      TEMP__2[k][j]=0;
    }
  }
  for( int k = 0; k < 32; k++){
    for( int j = 0; j < 32; j++){
      for( int i = 0; i < 16; i++){
        TEMP__2[k][j]=TEMP__2[k][j]+B[i][k]*dA[i][j];
      }
    }
  }
  for( int k = 0; k < 32; k++){
    for( int j = 0; j < 32; j++){
      dC[k][j]=TEMP__2[k][j];
    }
  }
}
