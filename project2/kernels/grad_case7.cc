#include "../run2.h"

void grad_case7(float (&dB)[16][32], float (&dA)[32][16])  {
  float TEMP__1[32][16];
  for( int j = 0; j < 32; j++){
    for( int i = 0; i < 16; i++){
      TEMP__1[j][i]=0;
    }
  }
  for( int j = 0; j < 32; j++){
    for( int i = 0; i < 16; i++){
      TEMP__1[j][i]=TEMP__1[j][i]+dB[i][j];
    }
  }
  for( int j = 0; j < 32; j++){
    for( int i = 0; i < 16; i++){
      dA[j][i]=TEMP__1[j][i];
    }
  }
}
