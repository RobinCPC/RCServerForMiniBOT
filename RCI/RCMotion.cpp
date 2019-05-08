#include "RCMotion.h"

I32_T motionCloseFlag = 0;

RTN_ERR RCMotion_Initial( const OpMode_T opMode, const I32_T &doHoming, I32_T *PRetDevID )
{
	RTN_ERR ret;
	

	I32_T   devType;
	I32_T   devIndex         = 0;
	I32_T   devState;
	I32_T   groupState;
	I32_T   groupIndex       = 0;
	U32_T   sleepTime;

	I32_T   groupAxesIdxMask = 0;
	I32_T   groupParaNum;
	I32_T   subIndex;
	I32_T   paraValueI32;
	F64_T   paraValueF64;

	Pos_T   actPosAcs;
	Pos_T   cmdPosAcs        = { 0 };


	

	if( opMode == SIMULATION )
	{
		devType = NMC_DEVICE_TYPE_SIMULATOR;
		sleepTime = 500;
	}
	else
	{
		devType = NMC_DEVICE_TYPE_ETHERCAT;
		sleepTime = 3000;
	}


	//===============================================
	//            Step 1 : Device open up
	//===============================================
	printf( "Start to openup device...\n" );
	NMC_MessageOutputEnable( TRUE );

	ret = NMC_DeviceOpenUp( devType, devIndex, PRetDevID );
	if( ret != 0 )
		return ret;
	else
		printf( "\nDevice open up succeed, device ID: %d.\n", PRetDevID[0] );

	ret = NMC_DeviceResetStateAll( PRetDevID[0] );
	if( ret != NMC_AXIS_STATE_DISABLE )
		return ret;
	else
		printf( "Device ID %d: Device Reset State All success.\n", PRetDevID[0] );

	//get device state
	ret = NMC_DeviceGetState( PRetDevID[0], &devState );
	if( devState != NMC_DEVICE_STATE_OPERATION )
		return ret;
	else
		printf( "Device ID %d: state is OPERATION.\n", PRetDevID[0] );

	printf( "\nReady to reset all drives in device...\n" );

	//===================================================
	//   Step 2 : Clean alarm of drives of each group
	//===================================================
	ret = NMC_GroupResetDriveAlmAll( PRetDevID[0], groupIndex );
	if( ret != 0 )
		return ret;

	//sleep
	Sleep( sleepTime );

	//check state
	ret = NMC_GroupGetState( PRetDevID[0], groupIndex, &groupState );
	if( ret != 0 )
		return ret;

	if( groupState != NMC_GROUP_STATE_DISABLE )
	{
		printf( "ERROR! Group reset failed.(group index %d)\n", groupIndex );
		return groupState;
	}
	else
		printf( "Group reset succeed.(group index %d)\n", groupIndex );


	//=================================================
	//            Step 3 : Enable groups
	//=================================================
	ret = NMC_DeviceEnableAll( PRetDevID[0] );
	if( ret != 0 )
		return ret;
	else
		printf( "\nReady to enable groups...\n" );

	//sleep
	Sleep( sleepTime );

	//check group state
	ret = NMC_GroupGetState( PRetDevID[0], groupIndex, &groupState );
	if( ret != 0 )
		return ret;

	if( groupState != NMC_GROUP_STATE_STAND_STILL )
	{
		printf( "ERROR! Group enable failed.(group index %d) (err code : %d) \n", groupIndex, groupState );
		return groupState;
	}
	else
		printf( "Group enable succeed.(group index %d)\n", groupIndex );

	//sleep
	Sleep( sleepTime );

	//=================================================
	//            Step 4 : 3D View Openup
	//=================================================
	if( opMode == SIMULATION )
	{
		ret = NMC_Group3DShow( PRetDevID[0], groupIndex );
		if( ret != 0 )
			return ret;
		Sleep( 5000 );
	}

	//=================================================
	//            Step 5 : Group homing
	//=================================================
  if (doHoming)  
  {
    if (opMode == SIMULATION)
    {
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_X;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Y;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Z;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_A;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_B;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_C;

      ret = NMC_GroupAxesHomeDrive(PRetDevID[0], groupIndex, groupAxesIdxMask);
      if (ret != 0)
        return ret;
      else
      {
        printf("NMC_GroupAxesHomeDrive Moving\n");
        ret = __CheckPosReach(PRetDevID[0]);
        if (ret != ERR_NEXMOTION_SUCCESS)
        {
          printf("ERROR! Group%d _targetAxisPosArrivalCheck\n", groupIndex);
          return ret;
        }
        else
          printf("\nNMC_GroupAxesHomeDrive Success!");
      }
    }
    else
    {
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_A;

      ret = NMC_GroupAxesHomeDrive(PRetDevID[0], groupIndex, groupAxesIdxMask);
      if (ret != 0)
      {
        printf("ERROR! NMC_GroupAxesHomeDrive: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, NULL, 0));
        return ret;
      }
      else
      {
        printf("NMC_GroupAxesHomeDrive Moving\n");
        ret = __CheckPosReach(PRetDevID[0]);
        if (ret != ERR_NEXMOTION_SUCCESS)
        {
          printf("ERROR! Group%d _targetAxisPosArrivalCheck\n", groupIndex);
          return ret;
        }
      }

      groupAxesIdxMask = 0;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_X;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Y;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Z;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_B;
      groupAxesIdxMask += NMC_GROUP_AXIS_MASK_C;

      ret = NMC_GroupAxesHomeDrive(PRetDevID[0], groupIndex, groupAxesIdxMask);
      if (ret != 0)
        return ret;
      else
      {
        printf("NMC_GroupAxesHomeDrive Moving\n");
        ret = __CheckPosReach(PRetDevID[0]);
        if (ret != ERR_NEXMOTION_SUCCESS)
        {
          printf("ERROR! Group%d _targetAxisPosArrivalCheck\n", groupIndex);
          return ret;
        }
        else
          printf("\nNMC_GroupAxesHomeDrive Success!");
      }
    }
  }

	//=================================================
	//          Step 6 : Posture initialize...
	//=================================================
	cmdPosAcs.pos[0] = 0;
	cmdPosAcs.pos[1] = 90;
	cmdPosAcs.pos[2] = 0;
	cmdPosAcs.pos[3] = 0;
	cmdPosAcs.pos[4] = 0;
	cmdPosAcs.pos[5] = 0;

    groupAxesIdxMask = 0;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_X;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Y;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Z;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_A;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_B;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_C;

	ret = NMC_GroupPtpAcsAll( PRetDevID[0], groupIndex, groupAxesIdxMask, &cmdPosAcs );
	if( ret != 0 )
		return ret;
	else
	{
		printf( "NMC_GroupPtpAcsAll Moving\n" );
		ret = __CheckPosReach( PRetDevID[0] );
		if( ret != ERR_NEXMOTION_SUCCESS )
		{
			printf( "ERROR! Group%d _targetAxisPosArrivalCheck\n", groupIndex );
			return ret;
		}
	}

	//sleep
	Sleep( sleepTime );

	ret = NMC_GroupGetActualPosAcs( PRetDevID[0], groupIndex, &actPosAcs );
	if( ret != 0 )
	{
		printf( "ERROR! NMC_GroupGetActualPosAcs: (%d)%s.\n", ret, NMC_GetErrorDescription( ret, NULL, 0 ) );
		return ret;
	}
	else
	{
		printf( "\nGroup NMC_GroupGetActualPosAcs succeed.(group index %d)\n", groupIndex );
		printf( "ActualPosAcs[0] = %f \n", actPosAcs.pos[0] );
		printf( "ActualPosAcs[1] = %f \n", actPosAcs.pos[1] );
		printf( "ActualPosAcs[2] = %f \n", actPosAcs.pos[2] );
		printf( "ActualPosAcs[3] = %f \n", actPosAcs.pos[3] );
		printf( "ActualPosAcs[4] = %f \n", actPosAcs.pos[4] );
		printf( "ActualPosAcs[5] = %f \n", actPosAcs.pos[5] );
		printf( "\n" );
	}
	//sleep
	Sleep( sleepTime );

	//=================================================
	//          Step 7 : Set blending mode...
	//=================================================
	//Set buffered mode (2:blending)
	groupParaNum = 0x36;
	subIndex     = 0;
	paraValueI32 = 2;

	ret = NMC_GroupSetParamI32( PRetDevID[0], groupIndex, groupParaNum, subIndex, paraValueI32 );
	if( ret != 0 )
		return ret;

	groupParaNum = 0x36;
	subIndex     = 1;
	paraValueI32 = 1;

	ret = NMC_GroupSetParamI32( PRetDevID[0], groupIndex, groupParaNum, subIndex, paraValueI32 );
	if( ret != 0 )
		return ret;

	groupParaNum = 0x36;
	subIndex     = 2;
	paraValueF64 = 50;

	ret = NMC_GroupSetParamF64( PRetDevID[0], groupIndex, groupParaNum, subIndex, paraValueF64 );
	if( ret != 0 )
		return ret;

	return ERR_NEXMOTION_SUCCESS;
}

RTN_ERR RCMotion_Close( const I32_T DevID )
{
	RTN_ERR ret;
	I32_T   groupCount = 1;
	I32_T   groupState;

	motionCloseFlag = 1;
    
    //=================================================
	//               Disable groups
	//=================================================
	ret = NMC_DeviceDisableAll( DevID );
	if( ret != 0 )
		goto _ERR_SHUTDOWN;
	else
		printf( "\nReady to disable groups...\n" );

	Sleep( 2000 );

	//check group state

	ret = NMC_GroupGetState( DevID, 0, &groupState );
	if( ret != 0 )
		goto _ERR_SHUTDOWN;

	if( groupState != NMC_GROUP_STATE_DISABLE )
	{
		printf( "ERROR! Group disable failed.(group index %d)\n", groupCount );
		goto _ERR_SHUTDOWN;
	}
	else
		printf( "Group disable succeed.(group index %d)\n", groupCount );


	//=================================================
	//              System shutdown...
	//=================================================
_ERR_SHUTDOWN:
	ret = NMC_DeviceShutdown( DevID );
	if( ret != 0 )
		return ret;
	else
		printf( "\nDevice shutdown succeed.\n" );

	motionCloseFlag = 0;
    
    return ERR_NEXMOTION_SUCCESS;
}

RTN_ERR RCMotion_SetVelParam( const I32_T DevID, const JointVel_T *JointVel )
{
	__ASSERT( JointVel != NULL );

	RTN_ERR ret;
	I32_T   axisIndex;
	I32_T   groupIndex = 0;
	F64_T   speedRatio = JointVel->speedRatio * 100.0;

    if( motionCloseFlag == 1 )
		return ERR_NEXMOTION_SUCCESS;

	for( axisIndex = 0; axisIndex < MINIBOTAXES; axisIndex++ )
	{
		F64_T jointMaxVel = RadianToDegree * JointVel->maxVel[axisIndex];

		ret = NMC_GroupAxSetParamF64( DevID, groupIndex, axisIndex, 0x32, 0, jointMaxVel );
		if( ret != 0 )
		{
			printf( "Axis %d max velocity is invalid\n", ( axisIndex + 1 ) );
			return ret;
		}
	}

	ret = NMC_GroupSetVelRatio( DevID, groupIndex, speedRatio );
	if( ret != 0 )
		return ret;

	Sleep( 1000 );

	for( axisIndex = 0; axisIndex < MINIBOTAXES; axisIndex++ )
	{
		F64_T jointMaxVel;

		ret = NMC_GroupAxGetParamF64( DevID, groupIndex, axisIndex, 0x32, 0, &jointMaxVel );
		if( ret != 0 )
			return ret;
		else
			printf( "Axis %d maximum velocity = %lf (degree/s) \n", ( axisIndex + 1 ), jointMaxVel );
	}

	ret = NMC_GroupGetVelRatio( DevID, groupIndex, &speedRatio );
	if( ret != 0 )
		return ret;
	else
		printf( "\nSpeed Ratio = %lf %%\n", speedRatio );

	return ERR_NEXMOTION_SUCCESS;
}

RTN_ERR RCMotion_SetAccParam( const I32_T DevID, const JointAcc_T *JointAcc )
{
	__ASSERT( JointAcc != NULL );

	RTN_ERR ret;
	I32_T   axisIndex;
	I32_T   groupIndex = 0;

    if( motionCloseFlag == 1 )
		return ERR_NEXMOTION_SUCCESS;

	for( axisIndex = 0; axisIndex < MINIBOTAXES; axisIndex++ )
	{
		F64_T jointMaxAcc = JointAcc->accRatio * RadianToDegree * JointAcc->maxAcc[axisIndex];

		ret = NMC_GroupAxSetParamF64( DevID, groupIndex, axisIndex, 0x33, 0, jointMaxAcc );
		if( ret != 0 )
		{
			printf( "Axis %d max acceleration is invalid\n", ( axisIndex + 1 ) );
			return ret;
		}

		ret = NMC_GroupAxSetParamF64( DevID, groupIndex, axisIndex, 0x34, 0, jointMaxAcc );
	}

	Sleep( 1000 );

	for( axisIndex = 0; axisIndex < MINIBOTAXES; axisIndex++ )
	{
		F64_T jointMaxAcc;

		ret = NMC_GroupAxGetParamF64( DevID, groupIndex, axisIndex, 0x33, 0, &jointMaxAcc );
		if( ret != 0 )
			return ret;
		else
		{
			printf( "Axis %d maximum acceleration = %lf (degree/s^2) \n", ( axisIndex + 1 ), jointMaxAcc );
			printf( "Axis %d maximum deceleration = %lf (degree/s^2) \n", ( axisIndex + 1 ), jointMaxAcc );
		}
	}

	return ERR_NEXMOTION_SUCCESS;
}

RTN_ERR RCMotion_GetActualPos(const I32_T DevID, Pos_T *JointPos)
{
	__ASSERT( JointPos != NULL );

	RTN_ERR ret;
	//I32_T   axisIndex;
	I32_T   groupIndex = 0;

  ret = NMC_GroupGetActualPosAcs(DevID, groupIndex, JointPos);

  if (ret != ERR_NEXMOTION_SUCCESS)
  {
    printf("get actual acs pose failed. error code: %d\n %s.\n", ret, NMC_GetErrorDescription(ret, NULL, 0));
  }

  return ret;
}

RTN_ERR RCMotion_Run( const I32_T DevID, const Path_T *PathArr, const I32_T totalPointNum )
{
	__ASSERT( PathArr != NULL );

	RTN_ERR ret;
	I32_T   groupAxesIdxMask = 0;
	I32_T   groupIndex       = 0;
	I32_T   pointIndex;
	Pos_T   cmdPosAcs        = { 0 };
	I32_T   groupStatus;

	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_X;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Y;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_Z;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_A;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_B;
	groupAxesIdxMask += NMC_GROUP_AXIS_MASK_C;

	ret = NMC_Group3DDrawPath( DevID, groupIndex, TRUE );

	for( pointIndex = 0; pointIndex < totalPointNum; pointIndex++ )
	{
		I32_T freeSpace;

        if( motionCloseFlag == 1 )
			return ERR_NEXMOTION_SUCCESS;

		cmdPosAcs.pos[0] = RadianToDegree * PathArr[pointIndex].jointPoint[0];
		cmdPosAcs.pos[1] = RadianToDegree * PathArr[pointIndex].jointPoint[1] + 90;
		cmdPosAcs.pos[2] = RadianToDegree * PathArr[pointIndex].jointPoint[2];
		cmdPosAcs.pos[3] = RadianToDegree * PathArr[pointIndex].jointPoint[3];
		cmdPosAcs.pos[4] = RadianToDegree * PathArr[pointIndex].jointPoint[4];
		cmdPosAcs.pos[5] = RadianToDegree * PathArr[pointIndex].jointPoint[5];

		ret = NMC_GroupPtpAcsAll( DevID, groupIndex, groupAxesIdxMask, &cmdPosAcs );
		if( ret != 0 )
			return ret;

		ret = NMC_GroupGetMotionBuffSpace( DevID, groupIndex, &freeSpace );
		if( ret != 0 )
			return ret;

		while( freeSpace == 0 )
		{
            Sleep( 100 );

            if( motionCloseFlag == 1 )
                return ERR_NEXMOTION_SUCCESS;

			ret = NMC_GroupGetMotionBuffSpace( DevID, groupIndex, &freeSpace );
			if( ret != 0 )
				return ret;
		}
	}

	//ret = NMC_GroupGetStatus( DevID, groupIndex, &groupStatus );
	//if( ret != 0 )
	//	return ret;

	//while( ( groupStatus & GROUPSTATUS_TARGETREACHED ) == 0 )
	//{
	//	Sleep( 500 );

  //      if( motionCloseFlag == 1 )
	//		return ERR_NEXMOTION_SUCCESS;

	//	ret = NMC_GroupGetStatus( DevID, groupIndex, &groupStatus );
	//	if( ret != 0 )
	//		return ret;
	//}

	//printf( "\nTarget Pos. reached!\n" );

	return ERR_NEXMOTION_SUCCESS;
}

bool RCMotion_CheckMotionDone(const I32_T DevID)
{
  // Check if Motion done. True = reach, False = Still moving. (or different error TODO)
  I32_T groupIndex = 0;
  I32_T groupStatus;
  I32_T ret = NMC_GroupGetStatus(DevID, groupIndex, &groupStatus);
  if (ret != 0)
    return false;

  if ((groupStatus & GROUPSTATUS_TARGETREACHED))
  {
    printf("\nTarget Pos. reached!\n");
    return true;
  }
  else
    return false;

}

RTN_ERR __CheckPosReach( const I32_T DevID )
{
	I32_T   groupStatus;
	I32_T   axisCount  = 6;
	I32_T   axisIndex;
	I32_T   groupIndex = 0;
	U32_T   time       = 0;
	Pos_T   axisActPos;
	RTN_ERR ret;
	U32_T   sleepTime  = 1000;
	U32_T   waitTime   = 2000;

	ret = NMC_GroupGetStatus( DevID, groupIndex, &groupStatus );
	if( ret != 0 )
		return ret;

	while( ( groupStatus & GROUPSTATUS_TARGETREACHED ) == 0 ) // When return value not equal to MC_GROUP_STATUS_MASK_CSTP
	{
		Sleep( sleepTime );

        if( motionCloseFlag == 1 )
			return ERR_NEXMOTION_SUCCESS;

		ret = NMC_GroupGetActualPosAcs( DevID, groupIndex, &axisActPos );
		if( ret != 0 )
			return ret;

		printf( "P_A = " );
		for( axisIndex = 0; axisIndex < axisCount; axisIndex++ )
			printf( " %.1f\t", axisActPos.pos[axisIndex] );
		printf( "\n" );

		ret = NMC_GroupGetStatus( DevID, groupIndex, &groupStatus );
		if( ret != 0 )
			return ret;

		if( waitTime < time )
		{
			printf( "targetAxisPosArrivalCheck TimeOut " );
			return ERR_NEXMOTION_PROCESS_TIMEOUT;
		}
		time++;
	}

	printf( "<Target arrival>\n" );
	return ERR_NEXMOTION_SUCCESS;
}

//CZ
void RCMotion_Stop( const I32_T DevID )
{
    RTN_ERR ret = 0;
    
    ret = NMC_GroupStopAll( DevID );
}
