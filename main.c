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

float multiplicacion_binaria(float a, float b){
    DecodedFloat numA = decode(a);
    DecodedFloat numB = decode(b);

    if(numA.mantissa == 0 || numB.mantissa == 0)
        return 0.0f;

    uint32_t signo_res = numA.signo ^ numB.signo;

    int32_t exponente_res = numA.exponente + numB.exponente;

    uint32_t producto_mantissa = (uint64_t)numA.mantissa * numB.mantissa;

    if(producto_mantissa & (1ULL << 47)){
        producto_mantissa >>= 24;
        exponente_res++;
    }
    else{
        producto_mantissa >>= 23;
    }

    return encode(signo_res, exponente_res, (uint32_t)producto_mantissa);
}

float division_binaria(float a, float b){
    DecodedFloat numA = decode(a);
    DecodedFloat numB = decode(b);

    if(numB.mantissa == 0){
        printf("Error: División por cero\n");
        return 0.0f;
    }
    if(numA.mantissa == 0)
        return 0.0f;

    uint32_t signo_res = numA.signo ^ numB.signo;

    int32_t exponente_res = numA.exponente - numB.exponente;

    uint32_t division_mantissa = ((uint64_t)numA.mantissa << 23) / numB.mantissa; 

    if((division_mantissa & 0x800000) == 0){
        division_mantissa <<= 1;
        exponente_res--;
    }

    return encode(signo_res, exponente_res, division_mantissa);
}

int main(){
    float a = 3.5f;
    float b = 2.5f;

    printf("------- OPERACIONES CON PUNTO FLOTANTE (IEEE 754) -------\n");
    printf("A = %f\nB = %f\n\n", a, b);

    printf("--- SUMA ---\n");
    printf("Hardware: %f\n", a + b);
    printf("Manual:   %f\n\n", suma_binaria(a, b));

    printf("--- RESTA ---\n");
    printf("Hardware: %f\n", a - b);
    printf("Manual:   %f\n\n", resta_binaria(a, b));

    printf("--- MULTIPLICACION ---\n");
    printf("Hardware: %f\n", a * b);
    printf("Manual:   %f\n\n", multiplicacion_binaria(a, b));

    printf("--- DIVISION ---\n");
    printf("Hardware: %f\n", a / b);
    printf("Manual:   %f\n\n", division_binaria(a, b));


    return 0;
}