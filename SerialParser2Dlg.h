
// SerialParser2Dlg.h: 头文件
//

#pragma once
#include <string>
#include <vector>
#include <thread>
#include <opencv2/opencv.hpp>

// CSerialParser2Dlg 对话框
class CSerialParser2Dlg : public CDialogEx
{
// 构造
public:
	CSerialParser2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERIALPARSER2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	char* wideCharToMultiByte(wchar_t* pWCStrKey);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	std::vector<CString> getComPort();
	BOOL reading = FALSE;
	BOOL recording=FALSE;
	HANDLE Handle_COM;
	HANDLE Read_Thread;
	HANDLE Camera_Thread;
	HANDLE ThreadEvent;
	_COMMTIMEOUTS Time_Outs{
		100,500,100,0,0
	};
	static UINT  Read_Data(LPVOID lparam);
	static UINT  Camera(LPVOID lparam);
	DCB dcb;
	cv::VideoCapture cap = cv::VideoCapture(0);
	std::vector<CString> Split(CString str,CString sep);
	CFile file;
	CString dir;
	CString hour;
public:
	afx_msg void OnBnClickedRecord();
	afx_msg void OnCbnSelchangeSelector();
	CComboBox COM_SELECTOR;
	CComboBox BAUDRATE_SELECTOR;
	CComboBox STOP_SELECTOR;
	CComboBox PARITY_SELECTOR;
	CComboBox DATA_SELECTOR;
	CMFCButton RECORD_BUTTON;
	CEdit OUTPUT_AREA;
	CStatic PIC_VIEW;
	afx_msg void OnBnClickedDirButton();
	CEdit DIE_EDIT;
	CMFCButton DIE_BUTTON;
};
