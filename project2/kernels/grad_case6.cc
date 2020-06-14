#include "../run2.h"

void grad_case6(float (&C)[8][16][3][3], float (&dA)[2][8][5][5], float (&dB)[2][16][7][7])  {
  float TEMP__1[2][16][7][7];
  for( int n = 0; n < 2; n++){
    for( int c = 0; c < 16; c++){
      for( int __a = 0; __a < 7; __a++){
        for( int __b = 0; __b < 7; __b++){
          TEMP__1[n][c][__a][__b]=0;
        }
      }
    }
  }
  for( int n = 0; n < 2; n++){
    for( int c = 0; c < 16; c++){
      for( int __a = 0; __a < 7; __a++){
        for( int __b = 0; __b < 7; __b++){
          for( int k = 0; k < 8; k++){
            for( int p = 0; p < 5; p++){
              for( int q = 0; q < 5; q++){
                if (__a-p < 3 && __a-p >= 0 && __b-q < 3 && __b-q >= 0) {
                  TEMP__1[n][c][__a][__b]=TEMP__1[n][c][__a][__b]+dA[n][k][p][q]*C[k][c][__a-p][__b-q];
                } else {
                  
                }
              }
            }
          }
        }
      }
    }
  }
  for( int n = 0; n < 2; n++){
    for( int c = 0; c < 16; c++){
      for( int __a = 0; __a < 7; __a++){
        for( int __b = 0; __b < 7; __b++){
          dB[n][c][__a][__b]=TEMP__1[n][c][__a][__b];
        }
      }
    }
  }
}
