#include <complex>

#define CORES 16
#define NUM_OF_DIFS 128
#define MAX 200
#define M_PI 3.1415926535897932384

typedef struct
{
    int inputnum[NUM_OF_DIFS];
    std::complex<double> result[NUM_OF_DIFS];
    int flag[CORES];
} Mailbox;