#include "e-lib.h"
#include "common.h"

using namespace std;

Mailbox mbox SECTION("shared_dram");

int main(void)
{		

  int i;
  unsigned int coreid, row, col, corenum;

  coreid = e_get_coreid();
  e_coords_from_coreid(coreid, &row, &col);
  corenum = row * e_group_config.group_cols + col;

  mbox.flag[corenum] = 1;


  //Put core in idle state
  __asm__ __volatile__("idle");
}
