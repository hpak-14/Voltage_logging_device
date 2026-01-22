#include <math.h>
#include "stm32f2xx_hal.h"

extern uint32_t RMS_N;
    
extern int64_t  RMS_u;
extern int64_t  DC_u;

typedef struct
{
    uint32_t RMS_samp;
    uint32_t RMS_N;
        
    int64_t  RMS_u;
    int64_t  DC_u;
        
    float    DC_U;
    float    RMS_U;
    float    RMS_AC;
    float    RMS_DC;
    uint8_t  RMS_flag;
    
} RMS_struct;

extern void RMS_Init(RMS_struct *Srms, uint32_t samples);
extern void RMS_Sample(RMS_struct *Srms, int32_t ADC_d);
extern void RMS_CalcResult(RMS_struct *Srms);

extern RMS_struct ch[8];

