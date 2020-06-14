#include "../run2.h"

void grad_case10(float (&dA)[8][8], float (&dB)[10][8])  {
  float TEMP__1[10][8];
  float TEMP__2[10][8];
  float TEMP__3[10][8];
  for( int i = 0; i < 10; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=0;
    }
  }
  for( int i = 0; i < 10; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+dB[i][j];
    }
  }
  for( int i = 0; i < 8; i++){
    for( int j = 0; j < 8; j++){
      TEMP__1[i][j]=TEMP__1[i][j]+dA[i][j]/3.0;
    }
  }
  for( int i = 0; i < 10; i++){
    for( int j = 0; j < 8; j++){
      dB[i][j]=TEMP__1[i][j];
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      TEMP__2[__a][j]=0;
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      TEMP__2[__a][j]=TEMP__2[__a][j]+dB[__a][j];
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      if (__a-1 < 8 && __a-1 >= 0) {
        TEMP__2[__a][j]=TEMP__2[__a][j]+dA[__a-1][j]/3.0;
      } else {
        
      }
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      dB[__a][j]=TEMP__2[__a][j];
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      TEMP__3[__a][j]=0;
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      TEMP__3[__a][j]=TEMP__3[__a][j]+dB[__a][j];
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      if (__a-2 < 8 && __a-2 >= 0) {
        TEMP__3[__a][j]=TEMP__3[__a][j]+dA[__a-2][j]/3.0;
      } else {
        
      }
    }
  }
  for( int __a = 0; __a < 10; __a++){
    for( int j = 0; j < 8; j++){
      dB[__a][j]=TEMP__3[__a][j];
    }
  }
}
