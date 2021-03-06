#include "e-lib.h"
#include "common.h"
#include <math.h>
#include <complex>

using namespace std;


int log2(int N)    /*function to calculate the log2(.) of int numbers*/
{
  int k = N, i = 0;
  while(k) {
    k >>= 1;
    i++;
  }
  return i - 1;
}

int check(int n)    //checking if the number of element is a power of 2
{
  return n > 0 && (n & (n - 1)) == 0;
}

int reverse(int N, int n)    //calculating revers number
{
  int j, p = 0;
  for(j = 1; j <= log2(N); j++) {
    if(n & (1 << (log2(N) - j)))
      p |= 1 << (j - 1);
  }
  return p;
}

void ordina(complex<double>* f1, int N) //using the reverse order in the array
{
  complex<double> f2[MAX];
  for(int i = 0; i < N; i++)
    f2[i] = f1[reverse(N, i)];
  for(int j = 0; j < N; j++)
    f1[j] = f2[j];
}

void transform(complex<double>* f, int N) //
{
  ordina(f, N);    //first: reverse order
  complex<double> W[NUM_OF_DIFS/CORES/2];
//  W = (complex<double> *)malloc(N / 2 * sizeof(complex<double>));
  W[1] = polar(1., -2. * M_PI / N);
  W[0] = 1;
  for(int i = 2; i < N / 2; i++)
    W[i] = pow(W[1], i);
  int n = 1;
  int a = N / 2;
  for(int j = 0; j < log2(N); j++) {
    for(int i = 0; i < N; i++) {
      if(!(i & n)) {
        complex<double> temp = f[i];
        complex<double> Temp = W[(i * a) % (n * a)] * f[i + n];
        f[i] = temp + Temp;
        f[i + n] = temp - Temp;
      }
    }
    n *= 2;
    a = a / 2;
  }
}

void FFT(complex<double>* f, int N, double d)
{
  transform(f, N);
  for(int i = 0; i < N; i++)
    f[i] *= d; //multiplying by step
}

volatile Mailbox mbox SECTION("shared_dram");

int main(void)
{
  int *a;
  int i;
  unsigned int coreid, row, col, corenum;


  coreid = e_get_coreid();
  e_coords_from_coreid(coreid, &row, &col);
  corenum = row * e_group_config.group_cols + col;


 complex<double> vec[MAX];
 for (int ccounter =0 ; ccounter < NUM_OF_DIFS/CORES ; ccounter++){
	vec[ccounter] = mbox.inputnum[corenum*(NUM_OF_DIFS/CORES)+ccounter];
 }
 FFT(vec, NUM_OF_DIFS/CORES, 1);
  for (int ccounter =0 ; ccounter < NUM_OF_DIFS/CORES ; ccounter++){
	mbox.result_r[corenum*(NUM_OF_DIFS/CORES)+ccounter] = vec[ccounter].real();
	mbox.result_im[corenum*(NUM_OF_DIFS/CORES)+ccounter] = vec[ccounter].imag();
 }

  a = (int *) 0x7000;
  (*(a)) = corenum;


  mbox.flag[corenum] = 1;




  //Put core in idle state
  __asm__ __volatile__("idle");
}
