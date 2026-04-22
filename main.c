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

    if(numA.mantissa == 0)
        return b;
    if(numB.mantissa == 0)
        return a;

    while(numA.exponente < numB.exponente){
        numA.mantissa >>= 1;
        numA.exponente++;
    }
    while(numB.exponente < numA.exponente){
        numB.mantissa >>= 1;
        numB.exponente++;
    }

    uint32_t res_mantissa;
    uint32_t signo_res;
    int32_t res_exponente = numA.exponente;

    if(numA.signo == numB.signo){
        res_mantissa = numA.mantissa + numB.mantissa;
        signo_res = numA.signo;

        if(res_mantissa & 0x1000000){
            res_mantissa >>= 1;
            res_exponente++;
        }
    }
    else{
        if(numA.mantissa >= numB.mantissa){
            res_mantissa = numA.mantissa - numB.mantissa;
            signo_res = numA.signo;
        }
        else{
            res_mantissa = numB.mantissa - numA.mantissa;
            signo_res = numB.signo;
        }

        if(res_mantissa == 0)
            return 0.0f;

        while ((res_mantissa & 0x800000) == 0)
        {
            res_mantissa <<= 1;
            res_exponente--;
        }
    }

    return encode(signo_res, res_exponente, res_mantissa);

}

float resta_binaria(float a, float b){
    uint32_t bitsB = *(uint32_t*)&b;
    bitsB ^= 0x80000000;
    float b_negativo = *(float*)&bitsB;

    return suma_binaria(a, b_negativo);
}

int main(){
    float n1 = 3.5f;
    float n2 = 2.5f;

    float result = suma_binaria(n1,n2);

    printf("Resultado de la suma: %f\n",result);

    return 0;
}