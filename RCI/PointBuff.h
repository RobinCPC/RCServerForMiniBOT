#ifndef __POINTBUFF_H
#define __POINTBUFF_H


#ifdef __cplusplus
extern "C" {
#endif


#include "nex_type.h"



#define MAX_POINT_DATA_SIZE (8)

typedef struct
{
    I32_T index;
    F64_T data[MAX_POINT_DATA_SIZE];

} RCPointData_T;
    







#ifdef __cplusplus
}
#endif




#endif