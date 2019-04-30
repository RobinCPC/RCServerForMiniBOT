#ifndef __RCSERVER_H
#define __RCSERVER_H

#include <winsock2.h>
#include"RCPackage.h"
#include "nex_type.h"




#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
     RCSVR_STATE_INIT       = 0 
   , RCSVR_STATE_STANDBY
   , RCSVR_STATE_RECV
   , RCSVR_STATE_RUN_MOTION
} RcSvrState_T; 


typedef struct
{
    RcSvrState_T state;
    SOCKET listenSocket;
    SOCKET clientSocket;
    I32_T totalRecvSize;
    I32_T totalSendSize;
    I32_T totalPoint;
    RCPointData_T *PPointBuf;
    I32_T curRecvPointNum;

} RCServer_T;


#define RCSVR_CMD_SAVE_TOTAL_POINT  ( 0x1 )
#define RCSVR_CMD_SAVE_POINT_DATA   ( 0x2 )
#define RCSVR_CMD_MOTION_DONE       ( 0x3 )
#define RCSVR_CMD_SET_PARAMETER_VEL ( 0x4 )
#define RCSVR_CMD_SET_PARAMETER_ACC ( 0x5 )
#define RCSVR_CMD_GET_ACTUAL_POS    ( 0x6 )
#define RCSVR_CMD_SEND_ACTUAL_POS   ( 0x7 )





RTN_ERR RCServer_Initial( RCServer_T *PSvrObj ); //done
void    RCServer_Close( RCServer_T *PSvrObj ); //done
RTN_ERR RCServer_CommunicationInitial( RCServer_T *PSvrObj, U32_T PortNum ); //done
RTN_ERR RCServer_CommunicationClose( RCServer_T *PSvrObj ); //done
RTN_ERR RCServer_WaitClientConnect( RCServer_T *PSvrObj, I32_T TimeoutMs ); //done
RTN_ERR RCServer_RecvPackage( RCServer_T *PSvrObj, RCPackage_T *PRetRCPackage ); //done
void    RCServer_CalculateTotalRecvSize( RCServer_T *PSvrObj, I32_T SizeOfRecv ); //done
RTN_ERR RCServer_SendPackage( RCServer_T *PSvrObj, RCPackage_T *PRCPackage ); //done
void    RCServer_CalculateTotalSendSize( RCServer_T *PSvrObj, I32_T SizeOfSend ); //done
RTN_ERR RCServer_ProcessPackage( RCServer_T *PSvrObj, RCPackage_T *PRCPackage ); //done
RTN_ERR RCServer_GetIPAddress( RCServer_T *PSvrObj, char **PRetIP );



void __RCServer_SetState( RCServer_T *PSvrObj, RcSvrState_T State ); //done
void RCServer_GetState( RCServer_T *PSvrObj, RcSvrState_T *PRetState ); //done
RTN_ERR __RCServer_SaveTotalPoint( RCServer_T *PSvrObj, I32_T TotalPoint ); //done
RTN_ERR __RCServer_SavePointData( RCServer_T *PSvrObj, RCPointData_T *PPointData ); //done













#define RTN_RCSERVER_RECV_POINT_DONE ( 1 )


#define ERR_RCSERVER_SECCESS                   (  0 )
#define ERR_RCSERVER_POINTER_NULL              ( -1 )
#define ERR_RCSERVER_WSA_STARTUP_FAILED        ( -2 )
#define ERR_RCSERVER_GET_ADDRINFO_FAILED       ( -3 )
#define ERR_RCSERVER_SOCKET_FAILED             ( -4 )
#define ERR_RCSERVER_BIND_FAILED               ( -5 )
#define ERR_RCSERVER_LISTEN_FAILED             ( -6 )
#define ERR_RCSERVER_PARAMETER_FAILED          ( -7 )
#define ERR_RCSERVER_TIMEROUT                  ( -8 )
#define ERR_RCSERVER_SELECT_FAILED             ( -9 )
#define ERR_RCSERVER_ACCEPT_FAILED             ( -10 )
#define ERR_RCSERVER_CLIENTSOCKET_FAILED       ( -11 )
#define ERR_RCSERVER_SHUTDOWN_FAILED           ( -12 )
#define ERR_RCSERVER_PACKAGE_CMD_FAILED        ( -13 ) 
#define ERR_RCSERVER_STATE_STANDBY_FAILED      ( -14 )
#define ERR_RCSERVER_TOTAL_POINT_FAILED        ( -15 )
#define ERR_RCSERVER_STATE_RECV_FAILED         ( -16 )
#define ERR_RCSERVER_CUR_RECV_POINT_NUM_FAILED ( -17 )
#define ERR_RCSERVER_POINT_INDEX_FAILED        ( -18 )
#define ERR_RCSERVER_RECV_FAILED               ( -19 )
#define ERR_RCSERVER_SEND_FAILED               ( -20 )
#define ERR_RCSERVER_SAVE_POINT_DATA_FAILED    ( -21 )
#define ERR_RCSERVER_GET_IP_ADDRESS_FAILED     ( -22 )


#ifdef __cplusplus
}
#endif




#endif