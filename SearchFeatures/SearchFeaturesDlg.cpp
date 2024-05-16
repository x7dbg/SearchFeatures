
// SearchFeaturesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchFeatures.h"
#include "SearchFeaturesDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define CODEPATH "./代码.txt"

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
    , m_strName(_T(""))
    , m_strNotes(_T(""))
    , m_strMarkCode(_T(""))
    , m_nOffset(0)
    , m_uLen(4)
    , m_dwBeginAddr(_T("0x00401000"))
    , m_dwEndAddr(_T("0x07FFFFFF"))
    , m_strMarkCodeList(_T(""))
    , m_btnType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSearchFeaturesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_PROCESSLIST, m_comboProcessList);
    DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
    DDX_Text(pDX, IDC_EDIT_NOTES, m_strNotes);
    DDX_Text(pDX, IDC_EDIT_MARKCODE, m_strMarkCode);
    DDX_Text(pDX, IDC_EDIT_OFFSET, m_nOffset);
    DDX_Text(pDX, IDC_EDIT_LENGTH, m_uLen);
    DDX_Text(pDX, IDC_EDIT_BEGINADDR, m_dwBeginAddr);
    DDX_Text(pDX, IDC_EDIT_ENDADDR, m_dwEndAddr);
    DDX_Text(pDX, IDC_EDIT_MARKCODELIST, m_strMarkCodeList);
    DDX_Radio(pDX, IDC_RADIO_BASEADDR, m_btnType);
    DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
    DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_comboBoxLanguage);
    DDX_Control(pDX, IDC_EDIT_MARKCODELIST, m_editMarkCodeList);
}

BEGIN_MESSAGE_MAP(CSearchFeaturesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CSearchFeaturesDlg::OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_PROCESSLIST, &CSearchFeaturesDlg::OnCbnSelchangeComboProcesslist)
    ON_BN_CLICKED(IDC_BTN_SEARCH, &CSearchFeaturesDlg::OnBnClickedBtnSearch)
    ON_BN_CLICKED(IDC_BTN_TEST, &CSearchFeaturesDlg::OnBnClickedBtnTest)
    ON_BN_CLICKED(IDC_BTN_ADDLIST, &CSearchFeaturesDlg::OnBnClickedBtnAddlist)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CSearchFeaturesDlg::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_LOAD, &CSearchFeaturesDlg::OnBnClickedBtnLoad)
    ON_BN_CLICKED(IDC_BTN_CREATECODE, &CSearchFeaturesDlg::OnBnClickedBtnCreatecode)
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
    InitUI();

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
}


BOOL CSearchFeaturesDlg::PreTranslateMessage(MSG* pMsg)
{
    //屏蔽Esc按钮 避免用户按一下Esc后直接退出程序
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
        strTitle.Format(_T("%s [%d]"), pe.szExeFile, pe.th32ProcessID);
        m_comboProcessList.InsertString(-1, strTitle);
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

void CSearchFeaturesDlg::OnBnClickedBtnSearch()
{
    if (m_hProcess == INVALID_HANDLE_VALUE)
    {
        MessageBox(_T("没有选择进程，请选择进程"), _T("错误"));
        return;
    }
    if (m_strMarkCodeList.IsEmpty())
    {
        MessageBox(_T("特征码列表为空！！！"), _T("错误"));
        return;
    }
    //搜索前先清空list避免重复显示
    m_listResult.DeleteAllItems();

    //分割出每一行的特征码
    std::vector<CString> vecMarkCodeList = SplitString(m_strMarkCodeList,_T('\n'));
    CFeatureCode fc;

    //遍历每一行特征码名字 
    for each (auto var in vecMarkCodeList)
    {
        if (var.IsEmpty())
        {
            continue;
        }
        //获取 注释 特征码 偏移 读取长度 读取方式(基址 偏移 CALL)
        std::vector<CString> vecMarkCodeLine = SplitString(var,_T(','));
        DWORD dwRetAddr[32] = { 0 };
        std::string strMarkCode = CStringA(vecMarkCodeLine[2]);

        DWORD dwBeginAddr = std::stoi(m_dwBeginAddr.GetBuffer(), nullptr, 16);
        DWORD dwEndAddr = std::stoi(m_dwEndAddr.GetBuffer(), nullptr, 16);
        DWORD dwCount = fc.FindMatchingCode(m_hProcess, strMarkCode, dwBeginAddr, 
            dwEndAddr, dwRetAddr, _ttoi(vecMarkCodeLine[3]), 
            vecMarkCodeLine[5].CompareNoCase(_T("CALL"))==0, false);

        if (dwCount>0)
        {
            //取值
            DWORD dwValue;
            ReadProcessMemory(m_hProcess, (LPVOID)dwRetAddr[0], &dwValue, _ttoi(vecMarkCodeLine[4]), NULL);

            //将结果设置到UI上
            CString strResult;
            strResult.Format(_T("0x%08X"), dwValue);
            DWORD dwCount = m_listResult.GetItemCount();
            m_listResult.InsertItem(dwCount, _T(""));
            m_listResult.SetItemText(dwCount, 0, vecMarkCodeLine[0]);//设置名字
            m_listResult.SetItemText(dwCount, 1, strResult);//设置读取结果
            m_listResult.SetItemText(dwCount, 2, vecMarkCodeLine[1]);//设置注释
        }
    }
}

void CSearchFeaturesDlg::OnBnClickedBtnTest()
{
    UpdateData();

    if (m_hProcess == INVALID_HANDLE_VALUE)
    {
        MessageBox(_T("没有选择进程，请选择进程"), _T("错误"));
        return;
    }

    DWORD dwRetAddr[64] = { 0 };
    CFeatureCode fc;

    //由于UI上的起止地址是字符串，所以要转成数字
    DWORD dwBeginAddr = std::stoi(m_dwBeginAddr.GetBuffer(), nullptr, 16);
    DWORD dwEndAddr = std::stoi(m_dwEndAddr.GetBuffer(), nullptr, 16);

    std::string strMarkCode = CStringA(m_strMarkCode);
    DWORD dwCount = fc.FindMatchingCode(m_hProcess, strMarkCode, dwBeginAddr, dwEndAddr, dwRetAddr, m_nOffset, m_btnType==1, true);

    if (dwCount == 0)
    {
        MessageBox(_T("该特征码没有匹配到任何内容 请检查特征码是否提取正确"), _T("错误"));
        return ;
    }
    CString strMsg;

    if (dwCount>1)
    {
        DWORD dwValue;
        ReadProcessMemory(m_hProcess, (LPVOID)dwRetAddr[0], &dwValue, m_uLen, NULL);
        strMsg.Format(_T("该特征码匹配到多个内容，建议更换特征码，第一个内容的值为：%X"), dwValue);
        MessageBox(strMsg, _T("警告"));
        return;
    }
    DWORD dwValue;
    ReadProcessMemory(m_hProcess, (LPVOID)dwRetAddr[0], &dwValue, m_uLen, NULL);

    strMsg.Format(_T("%X"), dwValue);
    MessageBox(strMsg,_T("测试结果"));
}


void CSearchFeaturesDlg::OnBnClickedBtnAddlist()
{
    UpdateData();

    CString strTmp;
    CString strType = _T("基址");
    switch (m_btnType)
    {
    case 0:
        strType = _T("基址");
        break;
    case 1:
        strType = _T("CALL");
        break;
    case 2:
        strType = _T("偏移");
        break;
    default:
        break;
    }
    //格式化特征码列表 显示格式为 名字 注释 特征码 偏移 读取长度 类型
    strTmp.Format(_T("%s,%s,%s,%d,%d,%s\r\n"), m_strName, m_strNotes, m_strMarkCode, m_nOffset, m_uLen,strType);
    m_strMarkCodeList += strTmp;
    
    UpdateData(FALSE);
}


void CSearchFeaturesDlg::OnBnClickedBtnSave()
{
    UpdateData();

    CString strFileName = _T("test.bp");			//默认打开的文件名
    CString strFilter = _T("特征码文件 (*.bp)|*.bp||");	//文件过虑的类型
    CFileDialog openFileDlg(FALSE, NULL, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, NULL);
    if (openFileDlg.DoModal() == IDOK)
    {
        //选择路径后 将特征码列表保存到文件中
        std::string strPath = CStringA(openFileDlg.GetPathName());
        FILE *pFile = nullptr;
        fopen_s(&pFile, strPath.c_str(), "wb+");
        if (pFile == nullptr)
        {
            MessageBox(_T("打开文件失败"),_T("错误"));
            return;
        }
        std::string strTmp = CStringA(m_strMarkCodeList);
        fprintf(pFile, strTmp.c_str());
        fclose(pFile);
    }
}


void CSearchFeaturesDlg::OnBnClickedBtnLoad()
{
    CString fileName = _T("test.bp");			//默认打开的文件名
    CString filter = _T("特征码文件 (*.bp)|*.bp||");	//文件过虑的类型
    CFileDialog openFileDlg(TRUE, NULL, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
    if (openFileDlg.DoModal() == IDOK)
    {
        //选择文件后 从文件中读取特征码列表显示到UI上
        std::string strPath = CStringA(openFileDlg.GetPathName());
        FILE *pFile = nullptr;
        fopen_s(&pFile, strPath.c_str(), "rb");
        if (pFile == nullptr)
        {
            MessageBox(_T("打开文件失败"), _T("错误"));
            return;
        }

        fseek(pFile, 0, SEEK_END);//把文件指针移动到文件尾
        long fileSize = ftell(pFile);//获取文件大小
        char *szMarkCode = new char[fileSize + 2];//分配内存
        memset(szMarkCode, 0, fileSize + 2);
        rewind(pFile);

        //读取文件并显示
        fread(szMarkCode, 1, fileSize, pFile);
        szMarkCode[fileSize] = '\0';
        m_strMarkCodeList = szMarkCode;
        delete szMarkCode;
        fclose(pFile);
        UpdateData(FALSE);
    }
}

std::vector<CString> CSearchFeaturesDlg::SplitString(const CString& str, TCHAR delimiter)
{
    //根据传进来的delimiter分割字符串 并存储到vector中再返回
    std::vector<CString> tokens;
    int start = 0;
    for (int i = 0; i < str.GetLength(); i++)
    {
        if (str[i] == delimiter)
        {
            CString token = str.Mid(start, i - start);
            tokens.push_back(token);
            start = i + 1;
        }
    }
    CString token = str.Mid(start);
    tokens.push_back(token);
    return tokens;
}

void CSearchFeaturesDlg::OnBnClickedBtnCreatecode()
{
    if (m_listResult.GetItemCount() == 0)
    {
        MessageBox(_T("请先搜索出结果再生成代码"), _T("警告"));

        return;
    }

    //根据用户选择来生成对应的代码
    switch (m_comboBoxLanguage.GetCurSel())
    {
    case 0://生成C++代码
    {
        CreateCppCode();
    }
    case 1://生成易语言代码
        break;
    default:
        break;
    }
    ShellExecute(NULL, L"open", L"notepad", _T(CODEPATH), NULL, SW_SHOWNORMAL);
}

void CSearchFeaturesDlg::InitUI()
{
    CheckDlgButton(IDC_RADIO_BASEADDR, BST_CHECKED);

    DWORD dwStyle = m_listResult.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER;
    m_listResult.SetExtendedStyle(dwStyle);
    m_listResult.InsertColumn(0, _T("名  称"), LVCFMT_LEFT, 110);
    m_listResult.InsertColumn(1, _T("结  果"), LVCFMT_LEFT, 100);
    m_listResult.InsertColumn(2, _T("注  释"), LVCFMT_LEFT, 110);

    m_comboBoxLanguage.InsertString(-1, _T("C++"));
    m_comboBoxLanguage.InsertString(-1, _T("易语言"));

    m_comboBoxLanguage.SetCurSel(0);
}

void CSearchFeaturesDlg::CreateCppCode()
{
    FILE *pFile = nullptr;
    fopen_s(&pFile, CODEPATH, "wb+");
    if (pFile == nullptr)
    {
        MessageBox(_T("打开文件失败"), _T("错误"));
        return;
    }
    CString strCode = _T("");

    for (int i = 0; i < m_listResult.GetItemCount(); i++)
    {
        CString strLine;
        strLine.Format(_T("#define %s \t%s \t//%s\r\n"), m_listResult.GetItemText(i, 0), m_listResult.GetItemText(i, 1), m_listResult.GetItemText(i, 2));
        strCode += strLine;

    }
    std::string strTmp = CStringA(strCode);
    fprintf(pFile, strTmp.c_str());

    fclose(pFile);
}