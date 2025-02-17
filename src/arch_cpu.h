/* Copyright 2022 Sipeed Technology Co., Ltd. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "float.h"
#include "tinymaix.h"

#if (TM_MDL_TYPE != TM_MDL_FP8_143) && (TM_MDL_TYPE != TM_MDL_FP8_152)
//sum = SUM(Ai*Bi)
TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    sumtype_t sum=0;
    for(int i=0; i <size; i++){
        sum += sptr[i]*kptr[i];
    }
    *result = sum;
    return;
}

#else
/*************************** FP8 SIMULATION **********************************/
#define SUMSCALE 1.0

TM_INLINE void tm_dot_prod(mtype_t* sptr, mtype_t* kptr,uint32_t size, sumtype_t* result)
{
    sumtype_t sum=0;
    for(int i=0; i <size; i++){
        float _s = tm_fp8to32(sptr[i]);
        float _k = tm_fp8to32(kptr[i]);
        sum += _s*_k;
        //printf("%.3f*%.3f+",_s,_k);
    }
    //printf("\r\n");
    *result = sum;
    return;
}

TM_INLINE void l_postprocess_sum(sumtype_t sum, btype_t b, int act, mtype_t* outp, \
    sctype_t scale, sctype_t out_s, zptype_t out_zp)
{   //printf("sum=%.6f,", sum);
    sum += tm_fp8to32(b); //printf("%.6f,", sum);
    switch(act){    //activation func
    case TM_ACT_RELU:
        sum = sum>0?sum:0;
        break;
    case TM_ACT_RELU6:
        sum = sum>0?sum:0;
        sum = sum>6?6:sum;
        break;
    default:
        break;
    }
    //printf("%.6f,", sum);
    *outp = tm_fp32to8(sum);
    //printf("  %02x,%.6f\r\n", *outp, tm_fp8to32(*outp));
    return;
}

#endif


