#include "e-lib.h"
#include "common.h"
#include "kiss_fft.h"

size_t TestFft(const kiss_fft_cpx in[NUM_OF_DIFS/CORES], kiss_fft_cpx out[NUM_OF_DIFS/CORES])
{
  kiss_fft_cfg cfg;
  void *b;
  size_t c;
  b = (void *) 0x4000;
  c = sizeof(b);
  cfg = kiss_fft_alloc(NUM_OF_DIFS/CORES, 0, b, &c);
    size_t i;
    kiss_fft(cfg, in, out);
return c;
    free(cfg);
}


int main(void)
{
  int *a, *c;
  unsigned *b, *d;
  int i;
  
  a    = (int *) 0x2000;//Address of matrix 
  b    = (unsigned *) 0x4000;//unused
  c    = (int *) 0x6000;//size of Result
  d    = (unsigned *) 0x7000;//Done flag

	kiss_fft_cpx in[NUM_OF_DIFS/CORES], out[NUM_OF_DIFS/CORES];
  	size_t ig;

  	for (ig = 0; ig < NUM_OF_DIFS/CORES; ig++){
    		in[ig].r = a[ig];
		in[ig].i = 0;
	}
  	size_t ksize = TestFft(in, out);
	(*(c)) = ksize;

  //Raising "done" flag
  (*(d)) = 0x00000001;

  //Put core in idle state
  __asm__ __volatile__("idle");
}
