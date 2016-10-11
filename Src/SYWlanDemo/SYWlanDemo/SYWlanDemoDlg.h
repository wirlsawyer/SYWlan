// SYWlanDemoDlg.h : header file
//

#pragma once


// CSYWlanDemoDlg dialog
class CSYWlanDemoDlg : public CDialog
{
// Construction
public:
	CSYWlanDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SYWLANDEMO_DIALOG };

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
	afx_msg void OnDestroy();

private:
	CListBox *_listBox1;
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
