#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <e-hal.h>
#include <iostream>
#include <fstream>
#include "common.h"
#include <string>
#include <e-loader.h>
#include <complex>

#define _BufOffset (0x01000000)

using namespace std;

Mailbox mbox;

int main(int argc, char *argv[]){
  e_platform_t platform;
  e_epiphany_t dev;
  e_mem_t emem;

  int done[CORES], all_done;
  int sop;
  int i,j, mi=0, mj=0, dif[NUM_OF_DIFS];
  float prev_float, cur_float;
  int sections = NUM_OF_DIFS/CORES; //assumes N is evenly divisible by CORES
  unsigned corenum[CORES];
  unsigned int addr;
  string str, s;

  //Calculation being done
  printf("Calculating \n");
  printf("........\n");
  
  ifstream file("/home/parallella/Downloads/paral/dataset.txt");

  for(i=0 ; i<CORES ; i++){
  	corenum[i] = 0;
  }

  //Compute differences

  while (getline(file, str)){
	if(!(mi%2)){
		prev_float = strtof(str.c_str(), 0);
	}
	else{
		cur_float = strtof(str.c_str(), 0);
		cur_float = cur_float - prev_float;
		dif[mj] = ((cur_float*100) >=0) ? (int)((cur_float*100) + 0.5) : (int)((cur_float*100) - 0.5);
		if(dif[mj] > 512){
			dif[mj]=512;
		}
		else if (dif[mj] < -512){
			dif[mj]=-512;
		}
		mj++;
                if(mj<30){
                	printf("f %d \n", dif[mj-1]);
                }
	        char bufft[20];
	        sprintf(bufft, "%d", dif[mj-1]);
	        string stri(bufft);
	        s = s+ " " + stri;
	}
	mi++;		
  }


  //Initalize Epiphany device

  e_init(NULL);
  e_reset_system();                                      //reset Epiphany
  e_get_platform_info(&platform);

  //Initialize mailbox

  e_alloc(&emem, _BufOffset, sizeof(Mailbox));



  e_open(&dev, 0, 0, platform.rows, platform.cols); //open workgroup

  e_load_group("e_fft_task.elf", &dev, 0, 0, 4, 4, E_FALSE);

  //e_load_group("e_huf_task.elf", &dev, 0, 0, 2, 4, E_FALSE);
  //e_load_group("e_fft_task.elf", &dev, 2, 0, 2, 4, E_FALSE);



  // Input in shared dram

  for(i=0; i< CORES; i++){
	mbox.flag[i] = 0;
  }
  for(j=0; j<NUM_OF_DIFS; j++){
		mbox.inputnum[j] = dif[j];
		mbox.result_r[j] = 0;
		mbox.result_im[j] = 0;		
  }
  addr = offsetof(Mailbox, flag);
  e_write(&emem, 0, 0, addr, mbox.flag, sizeof(mbox.flag));

  addr = offsetof(Mailbox, inputnum);
  e_write(&emem, 0, 0, addr, mbox.inputnum, sizeof(mbox.inputnum));

  addr = offsetof(Mailbox, result_r);
  e_write(&emem, 0, 0, addr, mbox.result_r, sizeof(mbox.result_r));

  addr = offsetof(Mailbox, result_im);
  e_write(&emem, 0, 0, addr, mbox.result_im, sizeof(mbox.result_im));


  // start cores
  e_start_group(&dev);


  //Check if all cores are done
  j=0;
  all_done=0;
  addr = offsetof(Mailbox, flag);
  while(all_done != CORES){
	all_done = 0;
	j++;
	e_read(&emem, 0, 0, addr, mbox.flag, sizeof(mbox.flag));
	for(i=0 ; i<CORES ; i++){
		all_done += mbox.flag[i];
		if(j>20000 && j<20003){
			//cout << "Core " << i << " flag: " << mbox.flag[i] << endl;
		}
	}
  }




 //Copy all Epiphany results to host memory space
  addr = offsetof(Mailbox, result_r);
  e_read(&emem, 0, 0, addr, mbox.result_r, sizeof(mbox.result_r));

  addr = offsetof(Mailbox, result_im);
  e_read(&emem, 0, 0, addr, mbox.result_im, sizeof(mbox.result_im));

 //Print result
  for(i=0 ; i<NUM_OF_DIFS ; i++){
	cout << "Res: Real: " << mbox.result_r[i] << " im: " << mbox.result_im[i] << endl;
  }



  // Print core number (old impl)
  for (i=0; i<platform.rows; i++){
      for (j=0; j<platform.cols;j++){
        e_read(&dev, i, j, 0x7000, &corenum[(i*platform.cols+j)], sizeof(int));
      }
  }
  for(i=0 ; i<CORES ; i++){
	cout << "core id: " << corenum[i] << endl;
  }

  //Close down Epiphany device
  //Close down Epiphany device
  e_close(&dev);
  e_free(&emem);
  e_finalize();

  if(sop!=0){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }
}
