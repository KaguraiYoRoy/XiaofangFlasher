﻿
// XiaofangFlasherGUIDlg.h: 头文件
//

#pragma once

class CXiaofangFlasherGUIDlgAutoProxy;


// CXiaofangFlasherGUIDlg 对话框
class CXiaofangFlasherGUIDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CXiaofangFlasherGUIDlg);
	friend class CXiaofangFlasherGUIDlgAutoProxy;

// 构造
public:
	CXiaofangFlasherGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CXiaofangFlasherGUIDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_XIAOFANGFLASHERGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CXiaofangFlasherGUIDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	static UINT FlasherMain(LPVOID lpParam);
};
