// SYWlanDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SYWlanDemo.h"
#include "SYWlanDemoDlg.h"
#include "SYWlanManger.h"

SYWlanManger *g_syWlanMgr;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSYWlanDemoDlg dialog




CSYWlanDemoDlg::CSYWlanDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSYWlanDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSYWlanDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSYWlanDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSYWlanDemoDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CSYWlanDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSYWlanDemoDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CSYWlanDemoDlg message handlers

BOOL CSYWlanDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	g_syWlanMgr = SYWlanManger::Instance();

	_listBox1 = (CListBox*)GetDlgItem(IDC_LIST2);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSYWlanDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSYWlanDemoDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSYWlanDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSYWlanDemoDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	
	//clear listbox	
	_listBox1->SetCurSel(-1);
	_listBox1->ResetContent();

	std::vector<SYWlanInfo*> *pVecWlanInfo = g_syWlanMgr->Scan();
	for (unsigned int i=0; i<pVecWlanInfo->size(); i++)
	{
		SYWlanInfo *info = pVecWlanInfo->at(i);
		CString tmp = L"";
		tmp.AppendFormat(L"SSID:%s    ", info->wszSSID);
		tmp.AppendFormat(L"Connected:%s   ", info->bCurrentConnected?L"Yes":L"No");
		tmp.AppendFormat(L"Mac:%02x-%02x-%02x-%02x-%02x-%02x  ", info->MAC_ADDRESS[0], info->MAC_ADDRESS[1], info->MAC_ADDRESS[2], info->MAC_ADDRESS[3], info->MAC_ADDRESS[4], info->MAC_ADDRESS[5]);
		tmp.AppendFormat(L"AuthAlgorithm:%s  ", info->wszAuthAlgorithm);


		if ( info->MAC_ADDRESS[0] !=-1)
		{
			_listBox1->AddString(tmp);
		}
		

		//OutputDebugString(L"---------------------------------\n");
		//info->Print();
		//OutputDebugString(L"---------------------------------\n");
	}
}

void CSYWlanDemoDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	delete g_syWlanMgr;
}

void CSYWlanDemoDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CString tmp;
	int index = _listBox1->GetCurSel();
	if (index == -1) return;

	_listBox1->GetText(index, tmp);

	//get IP
	int iStart = tmp.FindOneOf(L":")+1;
	int iStop = tmp.FindOneOf(L" ");
	int iLen = iStop - iStart;

	WCHAR wszSSID[32];
	memset(wszSSID, 0, sizeof(wszSSID));
	wsprintf(wszSSID, L"%s", tmp.Mid(iStart, iLen));

	bool bResult = g_syWlanMgr->ConnetWithSSID(wszSSID);
	
	if (bResult)
	{
		OnBnClickedButton1();
		AfxMessageBox(L"Please push 'Scan' Btn again");
	}else{
		AfxMessageBox(L"Can't connected");
	}

}

void CSYWlanDemoDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	SYWlanInfo *info = g_syWlanMgr->GetNowConnectedSSID();
	info->Print();
}
