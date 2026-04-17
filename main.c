#include <stdio.h>
#include <stdint.h>

typedef struct 
{
    uint32_t signo;
    int32_t exponente;
    uint32_t mantissa;
}DecodedFloat;

DecodedFloat decode(float f){
    uint32_t reg;

    reg = *(uint32_t*)&f;
    DecodedFloat df;
    df.signo = (reg >> 31) & 1;
    df.exponente = ((reg >> 23) & 0xFF) - 127;
    
    df.mantissa = (reg & 0x7FFFFF) | 0X800000;

    return df;
}

float suma_binaria(float a, float b){
    DecodedFloat da = decode(a);
    DecodedFloat db = decode(b);

    while(da.exponente < db.exponente){
        da.mantissa >>= 1;
        da.exponente++;
    }
    while(db.exponente < db.exponente){
        db.mantissa >>= 1;
        db.exponente++;
    }

    uint32_t res_mantissa = da.mantissa + db.mantissa;
    int32_t res_exponente = da.exponente;

    if(res_exponente & 0x1000000){
        res_mantissa >>= 1;
        res_exponente++;
    }

    uint32_t res_reg = (da.signo << 31) | ((res_exponente + 127) << 23 | (res_mantissa & 0x7FFFFF));

    return *(float*)&res_reg;
}

