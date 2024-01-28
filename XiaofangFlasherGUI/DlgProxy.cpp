
// DlgProxy.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "XiaofangFlasherGUI.h"
#include "DlgProxy.h"
#include "XiaofangFlasherGUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXiaofangFlasherGUIDlgAutoProxy

IMPLEMENT_DYNCREATE(CXiaofangFlasherGUIDlgAutoProxy, CCmdTarget)

CXiaofangFlasherGUIDlgAutoProxy::CXiaofangFlasherGUIDlgAutoProxy()
{
	EnableAutomation();

	// 为使应用程序在自动化对象处于活动状态时一直保持
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CXiaofangFlasherGUIDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CXiaofangFlasherGUIDlg)))
		{
			m_pDialog = reinterpret_cast<CXiaofangFlasherGUIDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CXiaofangFlasherGUIDlgAutoProxy::~CXiaofangFlasherGUIDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CXiaofangFlasherGUIDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CXiaofangFlasherGUIDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CXiaofangFlasherGUIDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IXiaofangFlasherGUI 的支持来支持类型安全绑定
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {840d6ac7-e7dc-440c-9376-e9da0aae3d3a}
static const IID IID_IXiaofangFlasherGUI =
{0x840d6ac7,0xe7dc,0x440c,{0x93,0x76,0xe9,0xda,0x0a,0xae,0x3d,0x3a}};

BEGIN_INTERFACE_MAP(CXiaofangFlasherGUIDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CXiaofangFlasherGUIDlgAutoProxy, IID_IXiaofangFlasherGUI, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏是在此项目的 pch.h 中定义的
// {3f4537c5-e322-49a7-8f44-98a2ca632176}
IMPLEMENT_OLECREATE2(CXiaofangFlasherGUIDlgAutoProxy, "XiaofangFlasherGUI.Application", 0x3f4537c5,0xe322,0x49a7,0x8f,0x44,0x98,0xa2,0xca,0x63,0x21,0x76)


// CXiaofangFlasherGUIDlgAutoProxy 消息处理程序
