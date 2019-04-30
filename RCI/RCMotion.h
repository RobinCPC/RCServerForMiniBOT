#include "NexMotion.h"
#include "NexMotionError.h"
//#include "platform_dependent.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define GROUPSTATUS_TARGETREACHED    ( 1 << NMC_GROUP_STATUS_CSTP )//1 left shift 9 bits

#define RadianToDegree               57.2957795
#define MINIBOTAXES                  6

 #ifdef _DEBUG 
    #include <assert.h>
    #define __ASSERT( _x_ )          assert( (_x_) )
#else  //Release
    #define __ASSERT( _x_ )
#endif


typedef enum
{
	  SIMULATION = 0
    , OPERATION
}OpMode_T;

typedef struct 
{
	F64_T maxVel[MINIBOTAXES];
	F64_T speedRatio;
}JointVel_T;

typedef struct
{
	F64_T maxAcc[MINIBOTAXES];
	F64_T accRatio;
}JointAcc_T;

typedef struct
{
	F64_T jointPoint[MINIBOTAXES];
}Path_T;

RTN_ERR RCMotion_Initial( OpMode_T opMode, I32_T *PRetDevID );
RTN_ERR RCMotion_Close( const I32_T DevID );
RTN_ERR RCMotion_SetVelParam( const I32_T DevID, const JointVel_T *JointVel );
RTN_ERR RCMotion_SetAccParam( const I32_T DevID, const JointAcc_T *JointAcc );
RTN_ERR RCMotion_Run( const I32_T DevID, const Path_T *PathArr, const I32_T totalPointNum );
void RCMotion_Stop( const I32_T DevID );

RTN_ERR __CheckPosReach( const I32_T DevID );
