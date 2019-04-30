

#include "RCServer.h"
#include "stdlib.h"

//Server APP-----------
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
//---------------------
 
 #ifdef _DEBUG 
    #include <assert.h>
    #define __ASSERT( _x_ )          assert( (_x_) )
#else  //Release
    #define __ASSERT( _x_ )
#endif

RTN_ERR RCServer_Initial( RCServer_T *PSvrObj )
{
    if( PSvrObj == NULL )
    {
        return ERR_RCSERVER_POINTER_NULL;
    }

    PSvrObj->state           = RCSVR_STATE_INIT;
    PSvrObj->listenSocket    = INVALID_SOCKET;
    PSvrObj->clientSocket    = INVALID_SOCKET;
    PSvrObj->totalRecvSize   = 0;
    PSvrObj->totalSendSize   = 0;
    PSvrObj->totalPoint      = 0;
    PSvrObj->PPointBuf       = NULL;
    PSvrObj->curRecvPointNum = 0;


    
    return ERR_RCSERVER_SECCESS;
}

void RCServer_Close( RCServer_T *PSvrObj )
{
    __ASSERT( PSvrObj != NULL ); 
    RCServer_CommunicationClose( PSvrObj );
    
    // Free point buff
    if( PSvrObj->PPointBuf != NULL )
    {
        free( PSvrObj->PPointBuf );
        PSvrObj->PPointBuf = NULL;
    }

}

RTN_ERR RCServer_CommunicationInitial( RCServer_T *PSvrObj, U32_T PortNum )
{
    //-------------------------
    const int buffSize      = 64;
    char portStr[buffSize]  = { "" };
    int ret = 0;
    WSADATA wsaData;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    SOCKET ListenSocket     = INVALID_SOCKET;
    //-------------------------

    if( PSvrObj == NULL )
    {
        return ERR_RCSERVER_POINTER_NULL;
    }

    // If user call this API continue without close communication then return error
    if( PSvrObj->listenSocket != INVALID_SOCKET )
    {
        return ERR_RCSERVER_SECCESS;
    }

    // Initialize Winsock
	//-------------------------------------------------------------
    ret = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if( ret != 0 ) 
	{
        return ERR_RCSERVER_WSA_STARTUP_FAILED;
    }
	//-------------------------------------------------------------


    //-------------------------------------------------------------
	ZeroMemory( &hints, sizeof( hints ) );

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;
	//-------------------------------------------------------------

    
    //-------------------------------------------------------------
    _itoa_s( PortNum, portStr, buffSize, 10 );

	// Resolve the server address and port
    ret = getaddrinfo( NULL, portStr /*DEFAULT_PORT*/, &hints, &result );
    if( ret != 0 ) 
	{
        WSACleanup();
        return ERR_RCSERVER_GET_ADDRINFO_FAILED;
    }
	//-------------------------------------------------------------


    //-------------------------------------------------------------
	// Create a SOCKET for connecting to server
    ListenSocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );

    if( ListenSocket == INVALID_SOCKET ) 
	{
		ret = WSAGetLastError();
        
        freeaddrinfo( result );

        WSACleanup();

        return ERR_RCSERVER_SOCKET_FAILED;
    }
	//-------------------------------------------------------------


    //-------------------------------------------------------------
	// Setup the TCP listening socket
    ret = bind( ListenSocket, result->ai_addr, ( int ) result->ai_addrlen );

    if( ret == SOCKET_ERROR ) 
	{
        ret = WSAGetLastError();
        
        freeaddrinfo( result );

        closesocket( ListenSocket );

        WSACleanup();

        return ERR_RCSERVER_BIND_FAILED;
    }
	//-------------------------------------------------------------

    //-------------------------------------------------------------
	 freeaddrinfo( result );
	//-------------------------------------------------------------


    //-------------------------------------------------------------
	ret = listen( ListenSocket, SOMAXCONN );

    if( ret == SOCKET_ERROR ) 
	{
        ret = WSAGetLastError();

        closesocket( ListenSocket );

        WSACleanup();

        return ERR_RCSERVER_LISTEN_FAILED;
    }
	//-------------------------------------------------------------


    PSvrObj->listenSocket = ListenSocket;

    return ERR_RCSERVER_SECCESS;
}

RTN_ERR RCServer_CommunicationClose( RCServer_T *PSvrObj )
{
    __ASSERT( PSvrObj != NULL );
  
    if( PSvrObj->clientSocket != INVALID_SOCKET )
    {
        int ret = shutdown( PSvrObj->clientSocket, SD_SEND );
        __ASSERT( ret != SOCKET_ERROR );

        closesocket( PSvrObj->clientSocket );
        PSvrObj->clientSocket = INVALID_SOCKET;
    }
  
    if( PSvrObj->listenSocket != INVALID_SOCKET )
    {
        closesocket( PSvrObj->listenSocket );
        PSvrObj->listenSocket = INVALID_SOCKET;
    }
    
    //Cleanup
    WSACleanup();
    
    return ERR_RCSERVER_SECCESS;
}


RTN_ERR RCServer_WaitClientConnect( RCServer_T *PSvrObj, I32_T TimeoutMs )
{
    //---------------------------
    fd_set  readSet;
    timeval timeout;
    int               ret = 0;
    const I32_T clientNum = 1; 
    //---------------------------

    if( PSvrObj == NULL )
    {
        return ERR_RCSERVER_POINTER_NULL;
    }

    if( TimeoutMs < 0 )
    {
        return ERR_RCSERVER_PARAMETER_FAILED;
    }


    //---------------------------------------------------------------
    // Wait for client connect in
    FD_ZERO( &readSet );

    FD_SET( PSvrObj->listenSocket, &readSet );
    
    timeout.tv_sec  = 0;  
    timeout.tv_usec = TimeoutMs * 1000;


    ret = select( PSvrObj->listenSocket, &readSet, NULL, NULL, &timeout );

    if( ret == 0 )
    {
        // Timeout
        
        //closesocket( PSvrObj->listenSocket );

        //WSACleanup();
        
        return ERR_RCSERVER_TIMEROUT;
    }
    else if( ret == clientNum )
    {
        // Client connected
    }
    else if( ret == SOCKET_ERROR )
    {
        // Error occurred
        ret = WSAGetLastError();

        closesocket( PSvrObj->listenSocket );

        WSACleanup();

        return ERR_RCSERVER_SELECT_FAILED;
    }
    //---------------------------------------------------------------



    //-------------------------------------------------------------
	// Accept a client socket
    PSvrObj->clientSocket = accept( PSvrObj->listenSocket, NULL, NULL );

    if( PSvrObj->clientSocket == INVALID_SOCKET ) 
	{
        ret = WSAGetLastError();

        closesocket( PSvrObj->listenSocket );

        WSACleanup();

        return ERR_RCSERVER_ACCEPT_FAILED;
    }
	//-------------------------------------------------------------



    //-------------------------------------------------------------
	// No longer need server socket
    closesocket( PSvrObj->listenSocket );
	//-------------------------------------------------------------


    
    
    
    // Push state
    //-------------------------------------
    __RCServer_SetState( PSvrObj, RCSVR_STATE_STANDBY );
    //-------------------------------------

    
    
    return ERR_RCSERVER_SECCESS;
}

RTN_ERR RCServer_RecvPackage( RCServer_T *PSvrObj, RCPackage_T *PRetRCPackage )
{
    //---------------------------
    SOCKET      clientSocket = INVALID_SOCKET;
    char       *pRecvBuf     = NULL;
    int         recvSizeByte = 0;
    int         retRecv      = 0; 
    //---------------------------


    __ASSERT( PSvrObj       != NULL );
    __ASSERT( PRetRCPackage != NULL );


    clientSocket = PSvrObj->clientSocket;
    __ASSERT( clientSocket != INVALID_SOCKET );

    pRecvBuf = ( char * ) ( PRetRCPackage );
    __ASSERT( pRecvBuf != NULL );

    recvSizeByte = sizeof( RCPackage_T );
    __ASSERT( recvSizeByte >= 0 );
    
    retRecv = recv( clientSocket, pRecvBuf, recvSizeByte, 0 );
    
    return retRecv;
}

void RCServer_CalculateTotalRecvSize( RCServer_T *PSvrObj, I32_T SizeOfRecv )
{
    __ASSERT( PSvrObj != NULL );
    __ASSERT( SizeOfRecv >= 0 );
    
    PSvrObj->totalRecvSize = PSvrObj->totalRecvSize + SizeOfRecv;
}

RTN_ERR RCServer_SendPackage( RCServer_T *PSvrObj, RCPackage_T *PRCPackage )
{
    //----------------------
    SOCKET clientSocket = INVALID_SOCKET;
    char  *pSendBuf     = NULL;
    int    sendSizeByte = 0;
    int    retSend      = 0;
    //----------------------

    __ASSERT( PSvrObj    != NULL );
    __ASSERT( PRCPackage != NULL );

    clientSocket = PSvrObj->clientSocket;
    __ASSERT( clientSocket != INVALID_SOCKET );

    pSendBuf = ( char * ) ( PRCPackage );
    __ASSERT( pSendBuf != NULL );

    sendSizeByte = sizeof( RCPackage_T );
    __ASSERT( sendSizeByte >= 0 );
    
    retSend = send( clientSocket, pSendBuf, sendSizeByte, 0 );

    return retSend;
}

void RCServer_CalculateTotalSendSize( RCServer_T *PSvrObj, I32_T SizeOfSend )
{
    __ASSERT( PSvrObj != NULL );
    __ASSERT( SizeOfSend >= 0 );

    PSvrObj->totalSendSize = PSvrObj->totalSendSize + SizeOfSend;
}

void __RCServer_SetState( RCServer_T *PSvrObj, RcSvrState_T State )
{
    __ASSERT( PSvrObj != NULL );
    __ASSERT( State >= 0 );

    PSvrObj->state = State;
}


void RCServer_GetState( RCServer_T *PSvrObj, RcSvrState_T *PRetState )
{
    __ASSERT( PSvrObj != NULL );
    __ASSERT( PRetState != NULL );
    
    *PRetState = PSvrObj->state;
}


RTN_ERR RCServer_ProcessPackage( RCServer_T *PSvrObj, RCPackage_T *PRCPackage )
{
    //--------------------------
    I32_T   cmd        = 0;
    RTN_ERR ret        = 0;
    I32_T   totalPoint = 0;
    //--------------------------

    if( PSvrObj == NULL )
        return ERR_RCSERVER_POINTER_NULL;
    
    if( PRCPackage == NULL )
        return ERR_RCSERVER_POINTER_NULL;
    
    
    //Get cmd
    cmd = PRCPackage->cmd;

    switch( cmd )
	{
    case RCSVR_CMD_SAVE_TOTAL_POINT: 
		
        //Get numbers of total point
        totalPoint = PRCPackage->pointData.index;
        if( totalPoint <= 0 )
            return ERR_RCSERVER_TOTAL_POINT_FAILED; 
        
        ret = __RCServer_SaveTotalPoint( PSvrObj, totalPoint );
        if( ret != ERR_RCSERVER_SECCESS )
            return ERR_RCSERVER_TOTAL_POINT_FAILED;
        
        break;

    
    case RCSVR_CMD_SAVE_POINT_DATA: 
		
        //Get point
        __ASSERT(  &( PRCPackage->pointData )  != NULL );
        
        ret = __RCServer_SavePointData( PSvrObj,  &( PRCPackage->pointData )  );
        if( ret != ERR_RCSERVER_SECCESS && ret != RTN_RCSERVER_RECV_POINT_DONE )
        {
            return ERR_RCSERVER_SAVE_POINT_DATA_FAILED;
        }
        else if( ret == RTN_RCSERVER_RECV_POINT_DONE )
        {
            return ret;
        }
            
        
        break;

	default:
        return ERR_RCSERVER_PACKAGE_CMD_FAILED;
    }
    
    return ERR_RCSERVER_SECCESS;
}

RTN_ERR __RCServer_SaveTotalPoint( RCServer_T *PSvrObj, I32_T TotalPoint )
{
    //--------------------
    RcSvrState_T state;
    //--------------------
    
    __ASSERT( PSvrObj != NULL );
    __ASSERT( TotalPoint > 0 );
    
    // Check state
    RCServer_GetState( PSvrObj, &state ); 
    if( state != RCSVR_STATE_STANDBY )
        return ERR_RCSERVER_STATE_STANDBY_FAILED;


    // Save numbers of total point
    PSvrObj->totalPoint = TotalPoint;


    // Create buff 
    PSvrObj->PPointBuf = ( RCPointData_T * ) malloc( sizeof( RCPointData_T ) * TotalPoint );
    __ASSERT( PSvrObj->PPointBuf != NULL );
    memset( PSvrObj->PPointBuf, 0, sizeof( RCPointData_T ) * TotalPoint );
    
    
    // Clear counter
    PSvrObj->curRecvPointNum = 0;
    
    
    // Push state 
    __RCServer_SetState( PSvrObj, RCSVR_STATE_RECV ); 
    
    return ERR_RCSERVER_SECCESS;
}


RTN_ERR __RCServer_SavePointData( RCServer_T *PSvrObj, RCPointData_T *PPointData )
{
    //----------------
    RcSvrState_T stateRead;
    U32_T        arrIndex    = 0;
    I32_T        dataDim     = 0;
    RTN_ERR      ret         = 0;
    //----------------

    __ASSERT( PSvrObj != NULL );
    __ASSERT( PPointData != NULL );


    // Check state
    RCServer_GetState( PSvrObj, &stateRead );
    if( stateRead != RCSVR_STATE_RECV )
        return ERR_RCSERVER_STATE_RECV_FAILED;
    
    
    // Add counter within current receive numbers of point
    ( PSvrObj->curRecvPointNum )++;
    
    
    // Check if exceed numbers of total point
    if( PSvrObj->curRecvPointNum > PSvrObj->totalPoint )
        return ERR_RCSERVER_CUR_RECV_POINT_NUM_FAILED;
    
    
    // Check if index of point error
    if( PPointData->index < 0 )
        return ERR_RCSERVER_POINT_INDEX_FAILED;

    if( PSvrObj->curRecvPointNum != PPointData->index )
        return ERR_RCSERVER_POINT_INDEX_FAILED;
    
    
    // Save index of point
    arrIndex = ( PSvrObj->curRecvPointNum ) -1;
    PSvrObj->PPointBuf[arrIndex].index = PPointData->index; 
    
    
    // Save data of point
    for( dataDim = 0; dataDim < MAX_POINT_DATA_SIZE; dataDim++ )
    {
        PSvrObj->PPointBuf[arrIndex].data[dataDim] = PPointData->data[dataDim];     
    }
    
    
    // Check if receive all points done
    if( PSvrObj->curRecvPointNum == PSvrObj->totalPoint )
    {
        return RTN_RCSERVER_RECV_POINT_DONE;
    }
    
    
    
    return ERR_RCSERVER_SECCESS;
}








RTN_ERR RCServer_GetIPAddress( RCServer_T *PSvrObj, char **PRetIP )
{
    //---------------------
    int ret = 0;
    char szBuffer[1024];
    struct hostent *host = NULL;


    //---------------------
    
    __ASSERT( PSvrObj != NULL );
    __ASSERT( PRetIP != NULL );
    
    
    ret = gethostname( szBuffer, sizeof( szBuffer ) );
    if( ret == SOCKET_ERROR )
    {
        return ERR_RCSERVER_GET_IP_ADDRESS_FAILED;
    }
    
    host = gethostbyname( szBuffer );
    if( host == NULL )
    {
        return ERR_RCSERVER_GET_IP_ADDRESS_FAILED;
    }
    
    *PRetIP = inet_ntoa (*(struct in_addr *)*host->h_addr_list);
    
    return ERR_RCSERVER_SECCESS;
}





