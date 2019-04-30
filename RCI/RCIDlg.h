
// RCIDlg.h : header file
//

#pragma once

#include "RCMotion.h"
#include "RCServer.h"
#include "nex_type.h"

// CRCIDlg dialog
class CRCIDlg : public CDialogEx
{
// Construction
public:
	CRCIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RCI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();



    void GUI_CreateThreadForMainFlow();
    void GUI_Initial();
    void GUI_Close();
    void GUI_ShowMsg_PInfo( const char *Message );
    void GUI_ShowMsg_ErrMsg( const char *Message );
    void GUI_ShowErr( const char *FuncName, int ErrCode );
    void GUI_ShowRecvPackage_PInfo( I32_T RecvSize, I32_T Cmd, I32_T Index, F64_T *PDataArr, I32_T DimOfArr );
    void GUI_CreateThreadForTest();
    void GUI_ShowPointBuff_PInfo( I32_T Index, F64_T *PDataArr, I32_T DimOfArr );
    void GUI_ShowIPAddress( const char *PIP );
    void GUI_ShowSvrState( I32_T State );
    void GUI_ShowTotalPoint( I32_T TotalPoint );
    void GUI_SetMotionInitFlag( bool InitFlag );
    bool GUI_GetMotionInitFlag( );

    I32_T GUI_GetSimulationEn();







private:

    CListBox     *mpListBox_PInfo;
    CListBox     *mpListBox_ErrMsg;

    bool mMotionInitialFlag;

    CButton *mpChkBox_Simulation;


    CButton *mpBtn_Start;
    CButton *mpBtn_Stop;









public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};



//CZ 2019012801
