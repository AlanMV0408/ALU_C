#include <stdio.h>
#include <stdint.h>

typedef struct 
{
    uint32_t signo;
    int32_t exponente;
    uint32_t mantissa;
}DecodeFloat;

DecodedFloat decode(float f){
    uint32_t reg;

    reg = *(uint32_t*)&f;
    DecodedFloat df;
    df.signo = (reg >> 31) & 1;
    df.exponente = ((reg >> 23) & 0xFF) - 127;
    
    df.mantissa = (reg & 0x7FFFFF) | 0X800000;

    return df;
}

