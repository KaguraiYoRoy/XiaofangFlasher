
// XiaofangFlasherGUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "XiaofangFlasherGUI.h"
#include "XiaofangFlasherGUIDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include <string>
#include <SetupAPI.h>
#include <Dbt.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXiaofangFlasherGUIDlg 对话框


IMPLEMENT_DYNAMIC(CXiaofangFlasherGUIDlg, CDialogEx);

CXiaofangFlasherGUIDlg::CXiaofangFlasherGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_XIAOFANGFLASHERGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
}

CXiaofangFlasherGUIDlg::~CXiaofangFlasherGUIDlg()
{
	// 如果该对话框有自动化代理，则
	//  此对话框的返回指针为 null，所以它知道
	//  此代理知道该对话框已被删除。
	if (m_pAutoProxy != nullptr)
		m_pAutoProxy->m_pDialog = nullptr;
}

void CXiaofangFlasherGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CXiaofangFlasherGUIDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BUTTON1, &CXiaofangFlasherGUIDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CXiaofangFlasherGUIDlg 消息处理程序
std::string Serials[256];
std::string TargetHexFile;
std::string TargetSerial;
DWORD ThreadID;
HANDLE hThread;
bool deviceFound,isReady;

BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName) {
	HMODULE hInstance = ::GetModuleHandle(NULL);//得到自身实例句柄  

	HRSRC hResID = ::FindResource(hInstance, MAKEINTRESOURCE(dwResName), lpResType);//查找资源  
	HGLOBAL hRes = ::LoadResource(hInstance, hResID);//加载资源  
	LPVOID pRes = ::LockResource(hRes);//锁定资源  

	if (pRes == NULL)//锁定失败  
	{
		return FALSE;
	}
	DWORD dwResSize = ::SizeofResource(hInstance, hResID);//得到待释放资源文件大小  
	HANDLE hResFile = CreateFile(lpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//创建文件  

	if (INVALID_HANDLE_VALUE == hResFile)
	{
		//TRACE("创建文件失败！");  
		return FALSE;
	}

	DWORD dwWritten = 0;//写入文件的大小	 
	WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);//写入文件  
	CloseHandle(hResFile);//关闭文件句柄  

	return (dwResSize == dwWritten);//若写入大小等于文件大小，返回成功，否则失败  
}

DWORD WINAPI FlasherMain(LPVOID lpParam) {
	CXiaofangFlasherGUIDlg* pDlg = (CXiaofangFlasherGUIDlg*)lpParam;

	char command[2048];
	sprintf_s(command, "avrdude.exe -C avrdude.conf -v -v  -p atmega328p -c arduino -P %s -b 115200 -D -U flash:w:\"%s\":i", TargetSerial.c_str(), TargetHexFile.c_str());
	pDlg->SetDlgItemTextA(IDC_STATIC_STATUS, "检测到小方接入，正在烧录，请勿关闭弹出的窗口");
	pDlg->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	bool ret=system(command);
	pDlg->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if(ret)
		pDlg->SetDlgItemTextA(IDC_STATIC_STATUS, "错误：avrdude未正常退出，烧录失败\r\n请检查USB线缆连接后重启软件重试");
	else
		pDlg->SetDlgItemTextA(IDC_STATIC_STATUS, "烧录完成！现在可以退出程序");
	return 0;
}

BOOL CXiaofangFlasherGUIDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)dwData;
	switch (nEventType) {
	case DBT_DEVICEARRIVAL:
		if (!deviceFound && isReady && lpdb->dbch_devicetype == DBT_DEVTYP_PORT) {
			TargetSerial = ((PDEV_BROADCAST_PORT)lpdb)->dbcp_name;
			deviceFound = true;
			hThread = CreateThread(NULL, 0, FlasherMain, (LPVOID)this, 0, &ThreadID);
		}
		break;
	default:
		break;
	}
	return FALSE;
}

BOOL CXiaofangFlasherGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	deviceFound = false;
	isReady = false;

	SYSTEM_INFO cur_system_info;
	GetNativeSystemInfo(&cur_system_info);
	WORD system_str = cur_system_info.wProcessorArchitecture;
	if (system_str == PROCESSOR_ARCHITECTURE_IA64 || system_str == PROCESSOR_ARCHITECTURE_AMD64) {
		FreeResFile(IDR_AVR64, "AVR", "avrdude.exe");
		FreeResFile(IDR_AVR64CONF, "AVR", "avrdude.conf");
	}
	else {
		FreeResFile(IDR_AVR86, "AVR", "avrdude.exe");
		FreeResFile(IDR_AVR86CONF, "AVR", "avrdude.conf");
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXiaofangFlasherGUIDlg::OnPaint()
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
HCURSOR CXiaofangFlasherGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

void CXiaofangFlasherGUIDlg::OnClose()
{
	DeleteFile("avrdude.exe");
	DeleteFile("avrdude.conf");
	if (CanExit())
		CDialogEx::OnClose();
}

void CXiaofangFlasherGUIDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CXiaofangFlasherGUIDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CXiaofangFlasherGUIDlg::CanExit()
{
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != nullptr)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CXiaofangFlasherGUIDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "HEX file(*.hex)|*.hex||", NULL);
	if (dlg.DoModal() != IDOK) {
		MessageBox("请选择更新文件！", "Info", MB_ICONINFORMATION);
		((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
		return;
	}
	TargetHexFile = dlg.GetPathName();
	CString str;
	str.Format("已选择文件，请连接小方", TargetHexFile.c_str());
	SetDlgItemText(IDC_STATIC_STATUS, str);
	isReady = true;
}
