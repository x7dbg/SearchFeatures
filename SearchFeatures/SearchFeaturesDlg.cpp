
// SearchFeaturesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchFeatures.h"
#include "SearchFeaturesDlg.h"
#include "afxdialogex.h"
#include "FeatureCode.h"
#include <tlhelp32.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CSearchFeaturesDlg 对话框



CSearchFeaturesDlg::CSearchFeaturesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchFeaturesDlg::IDD, pParent)
    , m_hProcess(INVALID_HANDLE_VALUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSearchFeaturesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_PROCESSLIST, m_comboProcessList);
}

BEGIN_MESSAGE_MAP(CSearchFeaturesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CSearchFeaturesDlg::OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_PROCESSLIST, &CSearchFeaturesDlg::OnCbnSelchangeComboProcesslist)
    ON_BN_CLICKED(IDC_BTN_SEARCH, &CSearchFeaturesDlg::OnBnClickedBtnSearch)
END_MESSAGE_MAP()


// CSearchFeaturesDlg 消息处理程序

BOOL CSearchFeaturesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    GetAllProcess();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSearchFeaturesDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSearchFeaturesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSearchFeaturesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSearchFeaturesDlg::OnBnClickedOk()
{
    // TODO:  在此添加控件通知处理程序代码
    //CDialogEx::OnOK();
}


BOOL CSearchFeaturesDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        return TRUE;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSearchFeaturesDlg::GetAllProcess()
{
    HANDLE hProcess;                        //进程句柄
    BOOL bProcess = FALSE;                  //获取进程信息的函数返回值

    PROCESSENTRY32 pe;                    //保存进程信息
    pe.dwSize = sizeof(PROCESSENTRY32);

    hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照
    CString strTitle;
    bProcess = Process32First(hProcess, &pe);              //获取第一个进程信息
    while (bProcess)
    {
//         strTitle.Format(_T("[%d] "), pe.th32ProcessID);
//         strTitle += pe.szExeFile;
        strTitle.Format(_T("%s [%d]"), pe.szExeFile, pe.th32ProcessID);
        m_comboProcessList.InsertString(-1, strTitle);
        //m_mapProcessList.insert(std::make_pair(pe.th32ProcessID,pe.szExeFile));
        bProcess = Process32Next(hProcess, &pe);
    }
}

void CSearchFeaturesDlg::OnCbnSelchangeComboProcesslist()
{
    if (m_hProcess != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hProcess);
    }
    //取进程ID
    CString strPid;
    m_comboProcessList.GetWindowText(strPid);

    int nPosLeft = strPid.Find(_T("["));
    int nPosRight = strPid.Find(_T("]"));
    if (nPosLeft == -1 || nPosRight == -1)
    {
        return;
    }
    strPid = strPid.Right(nPosRight-nPosLeft);
    strPid.Replace(_T("]"), _T(""));
    m_hProcess = GetProcessHandle(_ttoi(strPid));
}

inline HANDLE CSearchFeaturesDlg::GetProcessHandle(DWORD dwPid)
{
    TOKEN_PRIVILEGES priv = { 0 };
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        priv.PrivilegeCount = 1;
        priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
            AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);

        CloseHandle(hToken);
    }

    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
}

//////////////////////////////////////////////////
#include <windows.h>







/////////////////////////////

void CSearchFeaturesDlg::OnBnClickedBtnSearch()
{
    if (m_hProcess == INVALID_HANDLE_VALUE)
    {
        MessageBox(_T("还未选择进程"));
        return;
    }
    /*
    findMatchingCode() 参数说明：
    1) hProcess		要打开的进程句柄
    2) markCode		特征码,支持通配符（**），如: 55 8b ec ** 56 83 ec 20 ** ** 08 d9 ee
    3) memBeginAddr		起始搜索地址
    4) memEndAddr		结束搜索地址
    5) retAddr[]		记录找到的地址,传入这个参数前一定要清0，如 DWORD retAddr[32] = {0};  或者 DWORD *retAddr = new DWORD[32]();
    6) deviation		特征码地址离目标地址的偏移距离，上负下正
    7) isCall		是否为找CALL的跳转地址，true 则 retAddr[] 返回的是CALL跳转的地址
    8) isAll		是否查找所有符合的地址，false找到第一个符合的地址后就结束搜索，true继续搜索，直到搜索地址大于结束地址（memEndAddr）
    return返回值		找到的地址总数
    搜不到内存可能是保护属性没有选对
    */
    FeatureCode fc;
    DWORD retAddr[32] = { 0 };
    DWORD dwCount = fc.FindMatchingCode(m_hProcess, "85 C0 74 44 8B 40 ?? 85 C0 74 3D 83 B8 ?? ?? ?? ?? 00 74 34 8B 0D ?? ?? ?? ?? 85 C9 74 2A 8B 01 FF 50 ?? 85 C0", 0x00401000, 0x07FFFFFF, retAddr, -4, false, false);
    DWORD dwValue;
    ReadProcessMemory(m_hProcess,(LPVOID)retAddr[0],&dwValue,4,NULL);
}
