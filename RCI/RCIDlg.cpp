
// RCIDlg.cpp : implementation file
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "RCI.h"
#include "RCIDlg.h"
#include "afxdialogex.h"


RCServer_T gServer;
RCServer_T * __GetSvrObj() { return &gServer; }

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define ENABLE  ( 1 )
#define DISABLE ( 0 )

bool gThreadMainEnable = DISABLE; // 0: disable thread, 1: enable thread

HANDLE gHandle_ThreadMain = NULL;
DWORD WINAPI _Thread_Main( LPVOID lpParameter );
DWORD gId_ThreadMain = 0;



I32_T gDevID = 0;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRCIDlg dialog




CRCIDlg::CRCIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRCIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRCIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRCIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CRCIDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CRCIDlg::OnBnClickedButton2)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CRCIDlg message handlers

BOOL CRCIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

    GUI_Initial();






    // Initial motion
    //---------------------------
    //I32_T       simulationEn = 0;
    //OpMode_T    opMode;
    //RTN_ERR     ret          = 0;




    //simulationEn = GUI_GetSimulationEn();
    //if( simulationEn == ENABLE )
    //{
    //     opMode = SIMULATION;
    //}
    //else
    //{
    //     opMode = OPERATION;
    //}
    //
    //ret = RCMotion_Initial( opMode, &gDevID );
    //if( ret != ERR_RCSERVER_SECCESS )
    //{
    //    RCMotion_Close( gDevID );
    //    GUI_SetMotionInitFlag( DISABLE );

    //    //RCServer_Close( pObj );
    //    GUI_ShowErr( "RCMotion_Initial", ret );
    //    //return 0;
    //}
    //else
    //{
    //    GUI_SetMotionInitFlag( ENABLE );
    //}
    //---------------------------











	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRCIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRCIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRCIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRCIDlg::OnBnClickedButton1()
{
    GUI_CreateThreadForMainFlow();

    SetTimer( 0, 1, NULL );
}


void CRCIDlg::OnBnClickedButton2()
{
    //-----------
    bool motionIniFlag = 0;
    //-----------

    motionIniFlag = GUI_GetMotionInitFlag( );
    if( motionIniFlag == ENABLE )
    {
        //Stop motion
        RCMotion_Stop( gDevID );
    }









    gThreadMainEnable = DISABLE;

    KillTimer( 0 );
}


void CRCIDlg::GUI_CreateThreadForMainFlow()
{
    //-----------------------
	DWORD err = 0;
    CString str;
	//-----------------------


	gThreadMainEnable = ENABLE;

	gHandle_ThreadMain = CreateThread( NULL, 0, _Thread_Main, this, CREATE_SUSPENDED, &gId_ThreadMain );
    
    
    if( !gHandle_ThreadMain )
	{
		//create thread failed!			
		err = GetLastError();
    }
    
    if( !SetThreadPriority( gHandle_ThreadMain, THREAD_PRIORITY_HIGHEST ) )
	{
		err = GetLastError();
    }
    
    if( ResumeThread( gHandle_ThreadMain ) == -1 )
	{
		err = GetLastError();
    }

}


DWORD WINAPI _Thread_Main( LPVOID lpParameter )
{
	//--------------------------------------
    RTN_ERR     ret          = 0;
    RCServer_T *pObj         = __GetSvrObj();
    U32_T       portNum      = 0;
    CString     str;
    U32_T       timeCnt      = 0;
    const U32_T waitTimeMs   = 6000;
    RCPackage_T packageRecv;
    RTN_ERR     retRecv      = 0;
    I32_T       pointCnt     = 0;
    RCPackage_T  packageSend;
    char       *pIP          = NULL;
    JointVel_T  jointVel;
    I32_T       axis         = 0;
    JointAcc_T  jointAcc;
    Path_T     *PPath        = NULL;
    I32_T       simulationEn = 0;
    OpMode_T    opMode;
    
	//--------------------------------------

	CRCIDlg *ptr = ( CRCIDlg * ) lpParameter;
	if( ptr == NULL )
		return 0;
    
    ptr->MessageBox( "Server  Running", NULL,MB_OK );
    ptr->GUI_ShowMsg_PInfo( "Server  Running" );
    
    
    // Initial server
    //------------------------------------------------------
    ret = RCServer_Initial( pObj );

    if( ret != ERR_RCSERVER_SECCESS )
    {
        RCServer_Close( pObj );
        
        ptr->GUI_ShowErr( "RCServer_Initial", ret );
        ptr->GUI_Close();
        return 0;
    }
    //------------------------------------------------------



    // Initial communication
    //------------------------------------------------------
    ptr->GetDlgItemText( IDC_EDIT1, str );
    portNum = _tcstol( str, NULL, 10 );
    
    ret = RCServer_CommunicationInitial( pObj, portNum );
    
    if( ret != ERR_RCSERVER_SECCESS )
    {
        RCServer_Close( pObj );
        
        ptr->GUI_ShowErr( "RCServer_CommunicationInitial", ret );
        ptr->GUI_Close();
        return 0;
    }
    //------------------------------------------------------


    // Get server local IP address
    //-----------------------------------------------------------------------
    ret = RCServer_GetIPAddress( pObj, &pIP );
    if( ret != ERR_RCSERVER_SECCESS )
    {
        RCServer_Close( pObj );
        ptr->GUI_ShowErr( "RCServer_GetIPAddress", ret );
        ptr->GUI_Close();
        return 0;
    }

    ptr->GUI_ShowIPAddress( pIP );
    //-----------------------------------------------------------------------





    // Create thread to run client to test GUI
    //------------------------------------------------------
    //ptr->GUI_CreateThreadForTest();
    //------------------------------------------------------


    //-------------------------------------------------------
    ptr->GUI_ShowMsg_PInfo( "Wait Client Connect ..." );
    //-------------------------------------------------------



    // Wait client connect
    //--------------------------------------------------------

    do
    {
        timeCnt++;

        ret = RCServer_WaitClientConnect( pObj, 1 );

        if( ret == ERR_RCSERVER_SECCESS )
        {
            // Client connect success
            ptr->GUI_ShowMsg_PInfo( "Client Connect Success" );
            break;
        }
        else if( ret != ERR_RCSERVER_TIMEROUT )
        {
            // Error occur
            RCServer_Close( pObj );

            ptr->GUI_ShowErr( "RCServer_WaitClientConnect", ret );
            ptr->GUI_Close();
            return 0;
        }

        

    }
    while( ( timeCnt <= waitTimeMs ) && ( gThreadMainEnable == ENABLE ) );

    
    if( ret == ERR_RCSERVER_TIMEROUT )
    {
        RCServer_Close( pObj );


        if( timeCnt > waitTimeMs )
        {
            ptr->GUI_ShowMsg_ErrMsg( "Client Connect Timeout" );
        }
        else if( gThreadMainEnable == DISABLE )
        {
            ptr->GUI_ShowMsg_ErrMsg( "User Stop Process" );
        }
        
        ptr->GUI_Close();
        return 0;
    }
    //--------------------------------------------------------



    //-------------------------------
    ptr->GUI_ShowMsg_PInfo( "Motion Initialization... ( Start to run homing )" );
    //-------------------------------




    // Initial motion
    //---------------------------
    simulationEn = ptr->GUI_GetSimulationEn();
    if( simulationEn == ENABLE )
    {
         opMode = SIMULATION;
    }
    else
    {
         opMode = OPERATION;
    }
     
    
    ret = RCMotion_Initial( opMode, &gDevID );
    if( ret != ERR_RCSERVER_SECCESS )
    {
        RCMotion_Close( gDevID );
        ptr->GUI_SetMotionInitFlag( DISABLE );

        RCServer_Close( pObj );
        ptr->GUI_ShowErr( "RCMotion_Initial", ret );
        ptr->GUI_Close();
        return 0;
    }

    ptr->GUI_SetMotionInitFlag( ENABLE );
    //---------------------------




    //-------------------------------------
    ptr->GUI_ShowMsg_PInfo( "Start to receive package data..." );
    //-------------------------------------







    do
    {
        //Clear Buff
        memset( &packageRecv, 0, sizeof( RCPackage_T ) );
        
        retRecv = RCServer_RecvPackage( pObj, &packageRecv );


        if( retRecv > 0 )
        {
            //Recv package
            if( retRecv != sizeof( RCPackage_T ) )
            {
                RCMotion_Close( gDevID );
                ptr->GUI_SetMotionInitFlag( DISABLE );

                RCServer_Close( pObj );
                ptr->GUI_ShowMsg_ErrMsg( "Recv package size error" );
                ptr->GUI_Close();
                return 0;
            }
            
            ptr->GUI_ShowRecvPackage_PInfo( retRecv, packageRecv.cmd, packageRecv.pointData.index, packageRecv.pointData.data, MAX_POINT_DATA_SIZE );





            if( packageRecv.cmd == RCSVR_CMD_SET_PARAMETER_VEL )
            {
                // First package
                // Set speed ratio for velocity
                // Set max velocity for each axis

                jointVel.speedRatio = packageRecv.pointData.data[0];

                for( axis = 0; axis < MINIBOTAXES; axis++ )
                {
                    jointVel.maxVel[axis] = packageRecv.pointData.data[axis+1];
                }
                
                ret = RCMotion_SetVelParam( gDevID, &jointVel );
                if( ret != ERR_RCSERVER_SECCESS )
                {
                    RCMotion_Close( gDevID );
                    ptr->GUI_SetMotionInitFlag( DISABLE );

                    RCServer_Close( pObj );
                    ptr->GUI_ShowErr( "RCMotion_SetVelParam", ret );
                    ptr->GUI_Close();
                    return 0;
                }
            }
            else if( packageRecv.cmd == RCSVR_CMD_SET_PARAMETER_ACC )
            {
                // Second package
                // Set speed ratio for acc
                // Set max acc for each axis

                jointAcc.accRatio = packageRecv.pointData.data[0];

                for( axis = 0; axis < MINIBOTAXES; axis++ )
                {
                    jointAcc.maxAcc[axis] = packageRecv.pointData.data[axis+1];
                }
                
                ret = RCMotion_SetAccParam( gDevID, &jointAcc );
                if( ret != ERR_RCSERVER_SECCESS )
                {
                    RCMotion_Close( gDevID );
                    ptr->GUI_SetMotionInitFlag( DISABLE );

                    RCServer_Close( pObj );
                    ptr->GUI_ShowErr( "RCMotion_SetAccParam", ret );
                    ptr->GUI_Close();
                    return 0;
                }
            }
            else
            {
                ret = RCServer_ProcessPackage( pObj, &packageRecv );
                if( ret != ERR_RCSERVER_SECCESS  && ret != RTN_RCSERVER_RECV_POINT_DONE )
                {
                    RCMotion_Close( gDevID );
                    ptr->GUI_SetMotionInitFlag( DISABLE );

                    RCServer_Close( pObj );
                    ptr->GUI_ShowErr( "RCServer_ProcessPackage", ret );
                    ptr->GUI_Close();
                    return 0;
                }
                else if( ret == RTN_RCSERVER_RECV_POINT_DONE )
                {
                

                    // Push state
                    __RCServer_SetState( pObj, RCSVR_STATE_RUN_MOTION );

                    ptr->GUI_ShowMsg_PInfo( "Start to run motion..." );
                
                    
                    
                    
                    // Run motion here
                    //--------------------------
                    for( pointCnt = 0; pointCnt < pObj->totalPoint; pointCnt++ )
                    {
                        ptr->GUI_ShowPointBuff_PInfo( pObj->PPointBuf[pointCnt].index, pObj->PPointBuf[pointCnt].data, MAX_POINT_DATA_SIZE );
                    }
                    
                    // Create motion buff
                    PPath = ( Path_T * ) malloc( sizeof( Path_T ) * pObj->totalPoint );
                    __ASSERT( PPath != NULL );
                    
                    for( pointCnt = 0; pointCnt < pObj->totalPoint; pointCnt++ )
                    {
                        for( axis = 0; axis < MINIBOTAXES; axis++ )
                        {
                            PPath[pointCnt].jointPoint[axis] = pObj->PPointBuf[pointCnt].data[axis];
                        }
                    }

                    // Run motion API
                    ret = RCMotion_Run( gDevID, PPath, pObj->totalPoint );
                    if( ret != ERR_RCSERVER_SECCESS )
                    {
                        if( PPath != NULL )
                        {
                            free( PPath );
                            PPath = NULL;
                        }
                        
                        RCMotion_Close( gDevID );
                        ptr->GUI_SetMotionInitFlag( DISABLE );

                        RCServer_Close( pObj );
                        ptr->GUI_ShowErr( "RCMotion_Run", ret );
                        ptr->GUI_Close();
                        return 0;
                    }
                    
                    // Free motion buff
                    if( PPath != NULL )
                    {
                        free( PPath );
                        PPath = NULL;
                    }
                    
                    // Free SvrObj - Point buff
                    if( pObj->PPointBuf != NULL )
                    {
                        free( pObj->PPointBuf );
                        pObj->PPointBuf = NULL;
                    }
                    //--------------------------





                
                    ptr->GUI_ShowMsg_PInfo( "Motion done" );
                
                    // Push state
                    __RCServer_SetState( pObj, RCSVR_STATE_STANDBY );
                
                
                    // Send motion done to client
                    packageSend.cmd             = RCSVR_CMD_MOTION_DONE;
                    packageSend.pointData.index = pObj->totalPoint;
        
                    ret = RCServer_SendPackage( pObj, &packageSend );
                    if( ret < 0 )
                    {
                        RCMotion_Close( gDevID );
                        ptr->GUI_SetMotionInitFlag( DISABLE );

                        RCServer_Close( pObj );
                        ptr->GUI_ShowErr( "RCServer_SendPackage", ret );
                        ptr->GUI_Close();
                        return 0;
                    }

                }
            }

        }
        else if( retRecv == 0 )
        {
            //No package
            ptr->GUI_ShowMsg_PInfo( "Client Disconnect..." );
        }
        else
        {
            //Error occur
            RCMotion_Close( gDevID );
            ptr->GUI_SetMotionInitFlag( DISABLE );

            RCServer_Close( pObj );
            ptr->GUI_ShowErr( "RCServer_RecvPackage", retRecv );
            ptr->GUI_Close();
            return 0;
        }
    
    }
    while( gThreadMainEnable == ENABLE );
    
    
    //------------------------
    //Close motion
    RCMotion_Close( gDevID );
    ptr->GUI_SetMotionInitFlag( DISABLE );
    //------------------------
    
    
    
    RCServer_Close( pObj );
    
    ptr->GUI_ShowMsg_PInfo( "Server Stop" );


    //Sleep( 100 );
    ptr->GUI_Close();

    return 0;
}


void CRCIDlg::GUI_Initial()
{
    //-----------
    //-----------

    mpListBox_PInfo  = NULL;
    mpListBox_ErrMsg = NULL;
    
    //ListBox
    mpListBox_PInfo = ( CListBox * ) GetDlgItem( IDC_LIST1 );
    mpListBox_ErrMsg = ( CListBox * ) GetDlgItem( IDC_LIST2 );
    
    //Edit
    SetDlgItemText( IDC_EDIT1, "27015" );

    mMotionInitialFlag = DISABLE;

    //CheckBox
    mpChkBox_Simulation =  (CButton *)GetDlgItem( IDC_CHECK1 );
    mpChkBox_Simulation -> SetCheck(1);
    
    //Botton
    mpBtn_Start = (CButton *)GetDlgItem( IDC_BUTTON1 );
    mpBtn_Stop  = (CButton *)GetDlgItem( IDC_BUTTON2 );


}


I32_T CRCIDlg::GUI_GetSimulationEn()
{
    return mpChkBox_Simulation -> GetCheck();
}


void CRCIDlg::GUI_Close()
{
    MessageBox( "Server Stop !! Please Close the Program", NULL,MB_OK );

    mpBtn_Start -> EnableWindow( DISABLE );
    mpBtn_Stop  -> EnableWindow( DISABLE );
}


void CRCIDlg::GUI_ShowMsg_PInfo( const char *Message )
{
    CString str;

    if( Message != NULL )
    {
        str.Format( "%s", Message );
        mpListBox_PInfo->AddString( str );
        mpListBox_PInfo->SetCurSel( ( mpListBox_PInfo->GetCount() ) - 1 );
    }
}


void CRCIDlg::GUI_ShowMsg_ErrMsg( const char *Message )
{
    CString str;

    if( Message != NULL )
    {
        str.Format( "%s", Message );
        mpListBox_ErrMsg->AddString( str );
        mpListBox_ErrMsg->SetCurSel( ( mpListBox_ErrMsg->GetCount() ) - 1 );
    }
}


void CRCIDlg::GUI_ShowErr( const char *FuncName, int ErrCode )
{
    CString str;

    if( FuncName != NULL )
    {
        str.Format( "%s  Err: ( %d )", FuncName, ErrCode );
        mpListBox_ErrMsg->AddString( str );
        mpListBox_ErrMsg->SetCurSel( ( mpListBox_ErrMsg->GetCount() ) - 1 );
    }
}


void CRCIDlg::GUI_ShowRecvPackage_PInfo( I32_T RecvSize, I32_T Cmd, I32_T Index, F64_T *PDataArr, I32_T DimOfArr )
{
    //----------------
    CString str;
    I32_T   arrIndex = 0;

    //----------------
    
    
    str.Format( "Recevie Size ( Package ): ( %d ) Bytes", RecvSize );
    mpListBox_PInfo->AddString( str );
    
    
    str.Format( "Recevie Data ( Package ): cmd = ( 0x%x )", Cmd );
    mpListBox_PInfo->AddString( str );
    
    
    str.Format( "Recevie Data ( Package ): index = ( %d )", Index );
    mpListBox_PInfo->AddString( str );
    
    
    
    for( arrIndex = 0; arrIndex < DimOfArr; arrIndex++ )
    {
        str.Format( "Recevie Data ( Package ): data[%d] = ( %lf )", arrIndex, PDataArr[arrIndex] );
        mpListBox_PInfo->AddString( str );
    }
    
    
    mpListBox_PInfo->SetCurSel( ( mpListBox_PInfo->GetCount() ) - 1 );
    

}




void CRCIDlg::GUI_ShowPointBuff_PInfo( I32_T Index, F64_T *PDataArr, I32_T DimOfArr )
{
    //----------------
    CString str;
    I32_T   arrIndex = 0;

    //----------------
    
    
    str.Format( "PointBuff: Index = ( %d )", Index );
    mpListBox_PInfo->AddString( str );
    
    
    for( arrIndex = 0; arrIndex < DimOfArr; arrIndex++ )
    {
        str.Format( "PointBuff: data[%d] = ( %lf )", arrIndex, PDataArr[arrIndex] );
        mpListBox_PInfo->AddString( str );
    }
    
    mpListBox_PInfo->SetCurSel( ( mpListBox_PInfo->GetCount() ) - 1 );
}











void CRCIDlg::GUI_ShowIPAddress( const char *PIP )
{
    //-------------

    //-------------

    if( PIP != NULL )
    {
        SetDlgItemText( IDC_EDIT2, PIP );
    }
}


void CRCIDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    //------------------
    RCServer_T   *pObj         = __GetSvrObj();
    RcSvrState_T  state;
    //------------------
    
    
    // Show server state
    RCServer_GetState( pObj, &state );
    GUI_ShowSvrState( (I32_T) state );

    // Show receive total point
    GUI_ShowTotalPoint( pObj->totalPoint );



    CDialogEx::OnTimer(nIDEvent);
}




void CRCIDlg::GUI_ShowSvrState( I32_T State )
{
    //-----------------
    char *PStr = NULL;
    //-----------------

    if( State == RCSVR_STATE_INIT )
    {
        PStr = "INITIAL";
    }
    else if( State == RCSVR_STATE_STANDBY )
    {
        PStr = "STANDBY";
    }
    else if( State == RCSVR_STATE_RECV )
    {
        PStr = "RECEIVE";
    }
    else if( State == RCSVR_STATE_RUN_MOTION )
    {
        PStr = "RUN_MOTION";
    }
    
    SetDlgItemText( IDC_EDIT3, PStr );

}




void CRCIDlg::GUI_ShowTotalPoint( I32_T TotalPoint )
{
    //------------
    CString str;
    //------------
    
    str.Format( "%d", TotalPoint );
    SetDlgItemText( IDC_EDIT4, str );
}



void CRCIDlg::GUI_SetMotionInitFlag( bool InitFlag )
{
    mMotionInitialFlag = InitFlag;
}



bool CRCIDlg::GUI_GetMotionInitFlag( )
{
    return mMotionInitialFlag; 
}
