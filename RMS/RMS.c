#include <RMS.h>
#include "KaiserWin1_Int.h"

void RMS_Init(RMS_struct *Srms, uint32_t samples)
{
    Srms->RMS_samp = samples;
    Srms->RMS_N = 0;
    Srms->win_idx = 0;

    Srms->RMS_u = 0;
    Srms->DC_u = 0;

    Srms->DC_U = 0;
    Srms->RMS_U = 0;

    Srms->RMS_AC = 0;
    Srms->RMS_DC = 0;
    Srms->RMS_flag = 0;
    Srms->RMS_ADC = 0;
}

void RMS_Sample(RMS_struct *Srms, int32_t ADC_d)
{
    uint32_t w = kaiser_win[Srms->win_idx];

    int64_t u  = ADC_d;
    int64_t u2 = u * u;

    Srms->DC_u  += w * u;
    Srms->RMS_u += w * u2;

    Srms->win_idx++;

    if (++Srms->RMS_N >= Srms->RMS_samp) {
      
        
        Srms->DC_U  = Srms->DC_u;
        Srms->RMS_U = Srms->RMS_u;

        Srms->DC_u = 0;
        Srms->RMS_u = 0;
        Srms->RMS_N = 0;
        Srms->win_idx = 0;

        Srms->RMS_flag = 1;
        Srms->RMS_ADC = ADC_d;
    }
}   
void RMS_CalcResult(RMS_struct *Srms)
{
    if(Srms->RMS_flag)
    {
        float mean_sq = (float)Srms->RMS_U * kaiser_win_coeff;
        float mean    = (float)Srms->DC_U  * kaiser_win_coeff;

        float rms_ac = sqrtf(mean_sq - mean * mean);

        Srms->RMS_AC = rms_ac / 32767.0f * 4.096f * 1.1f;
        Srms->RMS_DC = mean    / 32767.0f * 4.096f * 1.1f;
        Srms->RMS_MG = Srms->RMS_ADC / 32767.0f * 4.096f * 1.1f * 1000.0f;

        Srms->RMS_flag = 0;
    }
}


RMS_struct ch[8];


