#include <cstdint>

#define CORES 16
#define NUM_OF_DIFS 128
#define MAX 200
#define M_PI 3.1415926535897932384

typedef struct
{
    int32_t inputnum[NUM_OF_DIFS];
    double result_r[NUM_OF_DIFS];
    double result_im[NUM_OF_DIFS];
    int32_t flag[CORES];
} Mailbox;