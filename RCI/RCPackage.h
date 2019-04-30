#ifndef __RCPACKAGE_H
#define __RCPACKAGE_H

#include "PointBuff.h"
#include "nex_type.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RCPACKAGE_DATA_SIZE (8)


typedef struct
{
    I32_T cmd;
    RCPointData_T pointData;

} RCPackage_T;






#ifdef __cplusplus
}
#endif




#endif