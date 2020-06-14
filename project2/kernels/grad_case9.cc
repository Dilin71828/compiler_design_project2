#include "../run2.h"

void grad_case9(float (&dB)[4][6], float (&dA)[4])  {
  float TEMP__1[4];
  for( int i = 0; i < 4; i++){
    TEMP__1[i]=0;
  }
  for( int i = 0; i < 4; i++){
    for( int j = 0; j < 6; j++){
      TEMP__1[i]=TEMP__1[i]+dB[i][j];
    }
  }
  for( int i = 0; i < 4; i++){
    dA[i]=TEMP__1[i];
  }
}
