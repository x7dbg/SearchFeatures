
// SearchFeaturesDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "FeatureCode.h"


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

    afx_msg void OnCbnSelchangeComboProcesslist();

    afx_msg void OnBnClickedBtnTest();

    afx_msg void OnBnClickedBtnSearch();

    
private:
    /*
    GetProcessHandle
    通过进程id获取进程句柄
    参数为进程id
    返回值为进程句柄
    */
    inline HANDLE GetProcessHandle(DWORD dwPid);
    //用于获取当前所有进程
    void GetAllProcess();

    DWORD Seach(std::string markCode, DWORD retAddr[]);

private:
    // 用于显示进程列表的ComboBox控件
    CComboBox m_comboProcessList;

    //存储进程句柄
    HANDLE m_hProcess;

    // 特征码名字 如果要生成C++代码的话就是C++宏名称
    CString m_strName;

    // 特征码注释 说明
    CString m_strNotes;

    // 特征码
    CString m_strMarkCode;

    // 要读取的偏移 如果是往上就是整数 往下是负数
    int m_nOffset;

    // 要读取的长度
    UINT m_uLen;

    // 特征码搜索起始地址
    CString m_dwBeginAddr;

    //特征码搜索结束地址
    CString m_dwEndAddr;
};
