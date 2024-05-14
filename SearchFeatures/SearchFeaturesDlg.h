
// SearchFeaturesDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CSearchFeaturesDlg 对话框
class CSearchFeaturesDlg : public CDialogEx
{
// 构造
public:
	CSearchFeaturesDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SEARCHFEATURES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //用于获取当前所有进程
    void GetAllProcess();
private:
    // 用于显示进程列表的ComboBox控件
    CComboBox m_comboProcessList;
    HANDLE m_hProcess;
    CString m_szFeatureCode;
    UINT m_addrLen;
    int m_nOffset;
public:
    afx_msg void OnCbnSelchangeComboProcesslist();
    inline HANDLE GetProcessHandle(DWORD dwPid);
    afx_msg void OnBnClickedBtnSearch();
};
