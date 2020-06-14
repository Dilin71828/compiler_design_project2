#include "../run2.h"

void grad_case8(float (&dB)[32], float (&dA)[2][16])  {
  float TEMP__1[2][16];
  for( int __a = 0; __a < 2; __a++){
    for( int __b = 0; __b < 16; __b++){
      TEMP__1[__a][__b]=0;
    }
  }
  for( int __a = 0; __a < 2; __a++){
    for( int __b = 0; __b < 16; __b++){
      if (__a*16+__b < 32 && __a*16+__b >= 0) {
        TEMP__1[__a][__b]=TEMP__1[__a][__b]+dB[__a*16+__b];
      } else {
        
      }
    }
  }
  for( int __a = 0; __a < 2; __a++){
    for( int __b = 0; __b < 16; __b++){
      dA[__a][__b]=TEMP__1[__a][__b];
    }
  }
}
