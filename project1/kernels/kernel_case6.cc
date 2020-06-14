#include "../run.h"

void kernel_case6(float (&B)[2][16][7][7], float (&C)[8][16][3][3], float (&A)[2][8][5][5])  {
  float TEMP__1[2][8][5][5];
  for( int n = 0; n < 2; n++){
    for( int k = 0; k < 8; k++){
      for( int p = 0; p < 5; p++){
        for( int q = 0; q < 5; q++){
          TEMP__1[n][k][p][q]=0;
        }
      }
    }
  }
  for( int n = 0; n < 2; n++){
    for( int k = 0; k < 8; k++){
      for( int p = 0; p < 5; p++){
        for( int q = 0; q < 5; q++){
          TEMP__1[n][k][p][q]=TEMP__1[n][k][p][q]+A[n][k][p][q];
        }
      }
    }
  }
  for( int n = 0; n < 2; n++){
    for( int k = 0; k < 8; k++){
      for( int p = 0; p < 5; p++){
        for( int q = 0; q < 5; q++){
          for( int c = 0; c < 16; c++){
            for( int r = 0; r < 3; r++){
              for( int s = 0; s < 3; s++){
                if (p+r < 7 && q+s < 7) {
                  TEMP__1[n][k][p][q]=TEMP__1[n][k][p][q]+B[n][c][p+r][q+s]*C[k][c][r][s];
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
    for( int k = 0; k < 8; k++){
      for( int p = 0; p < 5; p++){
        for( int q = 0; q < 5; q++){
          A[n][k][p][q]=TEMP__1[n][k][p][q];
        }
      }
    }
  }
}
