#include <RMS.h>

void RMS_Init(RMS_struct *Srms, uint32_t samples)
{
    Srms->RMS_samp = samples;
    Srms->RMS_N = 0;
    
    Srms->RMS_u = 0;
    Srms->DC_u = 0;
    
    Srms->DC_U = 0;
    Srms->RMS_U = 0;
    Srms->RMS_AC = 0;
    Srms->RMS_DC = 0;
    Srms->RMS_flag = 0;
}

void RMS_Sample(RMS_struct *Srms, int16_t *ADC_d){
    int32_t u = (int32_t)(*ADC_d);
    int32_t u2 = (int32_t)u * (int32_t)u;

    Srms->DC_u  += u;
    Srms->RMS_u += u2;

    if (++Srms->RMS_N >= Srms->RMS_samp) {
        Srms->DC_U = Srms->DC_u;
        Srms->RMS_U = Srms->RMS_u;

        Srms->DC_u = 0;
        Srms->RMS_u = 0;
        Srms->RMS_N = 0;

        Srms->RMS_flag = 1;
    }
}
        
void RMS_CalcResult(RMS_struct *Srms){
    if(Srms->RMS_flag){
    Srms->RMS_AC = sqrt(Srms->RMS_U/Srms->RMS_samp - (Srms->DC_U/Srms->RMS_samp)*(Srms->DC_U/Srms->RMS_samp)) / 32767.0f * 4.096f * 1.1f;
    Srms->RMS_DC = Srms->DC_U/Srms->RMS_samp / 32767.0f * 4.096f * 1.1f;
    Srms->RMS_flag = 0;
    }
}

RMS_struct RMS_ch1;
RMS_struct RMS_ch2;
RMS_struct RMS_ch3;
RMS_struct RMS_ch4;
RMS_struct RMS_ch5;
RMS_struct RMS_ch6;
RMS_struct RMS_ch7;
RMS_struct RMS_ch8;

