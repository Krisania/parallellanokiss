#include "e-lib.h"
#include "common.h"

using namespace std;



int main(void)
{		
  int *a, *c;
  unsigned *d, *b;
  int i;

  a    = (int *) 0x2000;//unused 
  b    = (unsigned *) 0x4000;//Address of matrix
  c    = (int *) 0x6000;//size of Result
  d    = (unsigned *) 0x7000;//Done flag


//  (*(b)) = e_get_coreid();

  //Raising "done" flag
  (*(d)) = 0x00000001;

  //Put core in idle state
  __asm__ __volatile__("idle");
}
