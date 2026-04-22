#include <stdio.h>
#include <stdint.h>
#include <math.h>
typedef struct 
{
    uint32_t signo;
    int32_t exponente;
    uint32_t mantissa;
}DecodedFloat;

DecodedFloat decode(float f){
    uint32_t bits = *(uint32_t*)&f;
    DecodedFloat num;

    num.signo = (bits >> 31) & 1;
    num.exponente = ((bits >> 23) & 0xFF) - 127;
    num.mantissa = (bits & 0x7FFFFF) | 0X800000;

    if((bits & 0x7FFFFFF) == 0){
        num.exponente = 0;
        num.mantissa = 0;
    }
    return num;
}

float encode(uint32_t signo, int32_t exponente, uint32_t mantissa){

    if(mantissa == 0) 
        return 0.0f;

    mantissa &= 0x7FFFFF;
    uint32_t bits = (signo << 31) | ((exponente + 127) << 23) | mantissa;

    return *(float*)&bits;
}



float suma_binaria(float a, float b){
    DecodedFloat numA = decode(a);
    DecodedFloat numB = decode(b);

    while(da.exponente < db.exponente){
        da.mantissa >>= 1;
        da.exponente++;
    }
    while(db.exponente < da.exponente){
        db.mantissa >>= 1;
        db.exponente++;
    }

    uint32_t res_mantissa = da.mantissa + db.mantissa;
    int32_t res_exponente = da.exponente;

    if(res_mantissa & 0x1000000){
        res_mantissa >>= 1;
        res_exponente++;
    }

    uint32_t res_reg = (da.signo << 31) | ((res_exponente + 127) << 23 | (res_mantissa & 0x7FFFFF));

    return *(float*)&res_reg;
}

int main(){
    float n1 = 3.5f;
    float n2 = 2.5f;

    float result = suma_binaria(n1,n2);

    printf("Resultado de la suma: %f\n",result);

    return 0;
}