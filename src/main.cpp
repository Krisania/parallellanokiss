#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>
#include <iostream>
#include <fstream>
#include "common.h"
#include <string>
#include <e-loader.h>
#include <complex>

using namespace std;

int main(int argc, char *argv[]){
  e_platform_t platform;
  e_epiphany_t dev;

  int done[CORES], all_done;
  size_t rescor[CORES];
  int sop;
  int i,j, mi=0, mj=0, dif[NUM_OF_DIFS];
  complex<double> resar[MAX];
  float prev_float, cur_float;
  int sections = NUM_OF_DIFS/CORES; //assumes N is evenly divisible by CORES
  unsigned clr = 0, corenum[CORES];
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
  e_open(&dev, 0, 0, platform.rows, platform.cols); //open first workgroup for huff


  e_load_group("e_huf_task.elf", &dev, 0, 0, 2, 4, E_FALSE);


  e_load_group("e_task.elf", &dev, 2, 0, 2, 4, E_FALSE);


/*
  //1. Copy data (N/CORE points) from host to Epiphany local memory
  //2. Clear the "done" flag for every core
  for (i=0; i<platform.rows; i++){
    for (j=0; j<platform.cols;j++){
      e_write(&dev, i, j, 0x4000, &dif[(i*platform.cols+j)*sections], sections*sizeof(int));
      e_write(&dev, i, j, 0x7000, &clr, sizeof(clr));
    }
  }
*/

  // start cores
  e_start_group(&dev);

  //Check if all cores are done
  while(1){
    all_done=0;
    for (i=0; i<platform.rows; i++){
      for (j=0; j<platform.cols;j++){
        e_read(&dev, i, j, 0x7000, &done[i*platform.cols+j], sizeof(unsigned));
	all_done+=done[i*platform.cols+j];
      }
    }
    if(all_done==CORES){
      break;
    }
  }

/*
  //Copy all Epiphany results to host memory space
  for (i=0; i<platform.rows; i++){
      for (j=0; j<platform.cols;j++){
        e_read(&dev, i, j, 0x4000, &resar[(i*platform.cols+j)*sections], sections*sizeof(complex<double>));
      }
  }

  //Print results
  sop=0;
  for (i=0; i<NUM_OF_DIFS; i++){
      cout << resar[i] << endl;
  }*/

/*
  for (i=0; i<platform.rows; i++){
      for (j=0; j<platform.cols;j++){
        e_read(&dev, i, j, 0x6000, &rescor[i*platform.cols+j], sizeof(size_t));
	printf("size is: %u \n", rescor[i*platform.cols+j]);
      }
  }
*/
/*
  for (i=0; i<platform.rows; i++){
      for (j=0; j<platform.cols;j++){
        e_read(&dev, i, j, 0x4000, &corenum[(i*platform.cols+j)], sizeof(unsigned));
      }
  }
  for(i=0 ; i<CORES ; i++){
	cout << "core id: " << corenum[i] << endl;
  }*/
  //Close down Epiphany device
  //Close down Epiphany device
  e_close(&dev);
  e_finalize();

  if(sop!=0){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }
}
