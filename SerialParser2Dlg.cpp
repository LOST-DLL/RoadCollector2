
// SerialParser2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SerialParser2.h"
#include "SerialParser2Dlg.h"
#include "afxdialogex.h"
#include <vector>
#include <string>
#include <thread>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <atlenc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialParser2Dlg 对话框

char* CSerialParser2Dlg::wideCharToMultiByte(wchar_t* pWCStrKey)
{
	//第一次调用确认转换后单字节字符串的长度，用于开辟空间
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	//第二次调用将双字节字符串转换成单字节字符串
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;

	//如果想要转换成string，直接赋值即可
	//string pKey = pCStrKey;
}

std::vector<CString> CSerialParser2Dlg::getComPort()
{
	HKEY hKey;
	wchar_t portName[256], w_commName[256];
	std::vector<CString> comName;
	//打开串口注册表对应的键值  
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hKey))
	{
		int i = 0;
		int mm = 0;
		DWORD  dwLong, dwSize;
		while (TRUE)
		{
			dwLong = dwSize = sizeof(portName);
			//枚举串口
			if (ERROR_NO_MORE_ITEMS == ::RegEnumValue(hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)w_commName, &dwSize))
			{
				break;
			}
			char* commName = wideCharToMultiByte(w_commName);
			comName.push_back((CString)commName);
			delete[] commName;
			i++;
		}
		//关闭注册表
		RegCloseKey(hKey);
	}
	else
	{
		//MessageBox(NULL, "您的计算机的注册表上没有HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm项", "警告", MB_OK);
	}
	//返回串口号
	return comName;
}

CSerialParser2Dlg::CSerialParser2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERIALPARSER2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialParser2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, COM_Selector, COM_SELECTOR);
	DDX_Control(pDX, BaudRate_Selector, BAUDRATE_SELECTOR);
	DDX_Control(pDX, Stop_Selector, STOP_SELECTOR);
	DDX_Control(pDX, Parity_Selector, PARITY_SELECTOR);
	DDX_Control(pDX, Data_Selector, DATA_SELECTOR);
	DDX_Control(pDX, Start_Record, RECORD_BUTTON);
	DDX_Control(pDX, OutPut_Area, OUTPUT_AREA);
	DDX_Control(pDX, IDC_PIC, PIC_VIEW);
	DDX_Control(pDX, Dir_Edit, DIE_EDIT);
	DDX_Control(pDX, Select_Dir_Button, DIE_BUTTON);
}

BEGIN_MESSAGE_MAP(CSerialParser2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Start_Record, &CSerialParser2Dlg::OnBnClickedRecord)
	ON_CBN_SELCHANGE(COM_Selector, &CSerialParser2Dlg::OnCbnSelchangeSelector)
	ON_BN_CLICKED(Select_Dir_Button, &CSerialParser2Dlg::OnBnClickedDirButton)
END_MESSAGE_MAP()


// CSerialParser2Dlg 消息处理程序

BOOL CSerialParser2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码

	for (CString ComPort : getComPort()) {
		COM_SELECTOR.AddString(ComPort);
	}
	BAUDRATE_SELECTOR.SetCurSel(6);
	STOP_SELECTOR.SetCurSel(0);
	DATA_SELECTOR.SetCurSel(0);
	PARITY_SELECTOR.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSerialParser2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSerialParser2Dlg::OnPaint()
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
HCURSOR CSerialParser2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::vector<CString>CSerialParser2Dlg::Split(CString str, CString sep) {
	str += sep;
	int pos = 0, pre_pos = 0;
	int len = sep.GetLength();
	std::vector<CString>res;
	while (-1 != pos) {
		pos = str.Find(sep, pre_pos);
		res.push_back(str.Mid(pre_pos, pos - pre_pos));
		pre_pos = pos + len;
	}
	return res;
}

UINT CSerialParser2Dlg::Camera(LPVOID lparam) {
	CSerialParser2Dlg* ptr = (CSerialParser2Dlg*)lparam;
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	//通过格式化日期，构建View名称
	CString viewTime = m_time.Format(_T("%Y%m%d%H%M%Sview"));
	//将CString转换为string类型
	std::string viewTitleString = CT2A(viewTime.GetBuffer());
	//将string类型转为char*
	char* viewName = (char*)viewTitleString.data();
	cv::namedWindow(viewName, cv::WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(viewName);
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, ptr->GetDlgItem(IDC_PIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	cv::Mat frame;
	//cv::VideoCapture cap(0);
	cv::VideoWriter vw((std::string)(CStringA)(ptr->dir + "\\V" + ptr->hour + L".mp4"), cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 30, cv::Size(640, 480), true);
	while (ptr->recording) {
		ptr->cap >> frame;
		vw << frame;
		cv::imshow(viewName,frame);
		cv::waitKey(30);
	}
	vw.release();
	//cap.release();
	return 0;
}

UINT CSerialParser2Dlg::Read_Data(LPVOID lparam) {
	CSerialParser2Dlg* ptr = (CSerialParser2Dlg*)lparam;
	CString path;
	ptr->GetDlgItem(Dir_Edit)->GetWindowText(path);
	PurgeComm(ptr->Handle_COM, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);//清空串口缓冲区

	DWORD dwError;

	// 数据接收
	DWORD dwWantRead = 4096;
	DWORD dwRead = 0;
	char pReadBuf[4096];
	while (ptr->reading) {
		//CString pReadBuf;
		memset(pReadBuf, 0, 4096);
		
		if (ClearCommError(ptr->Handle_COM, &dwError, NULL))
		{
			PurgeComm(ptr->Handle_COM, PURGE_RXABORT | PURGE_RXCLEAR);
		}

		if (!ReadFile(ptr->Handle_COM, pReadBuf, dwWantRead, &dwRead, NULL))
		{
			//std::cout << "读取失败" << std::endl;
		}
		//std::cout << "读取的内容: ";
		//std::cout << pReadBuf << std::endl;
		if ("$GNGGA" == CString(pReadBuf).Left(6) && "OK*35\r\n" == CString(pReadBuf).Right(7)) {
			CString strData;
			CString remain_str;
			ptr->OUTPUT_AREA.GetWindowText(strData);
			//strData += pReadBuf;
			CString Gtime, date, hour, minute, second;
			std::vector<CString>lines=ptr->Split((CString)pReadBuf,L"\r\n");
			for (CString& line : lines) {
				//strData += line.Left(6);
				//strData += line;
				if ("$GNRMC"==line.Left(6)) {
					auto parts = ptr->Split(line,L",");
					Gtime = parts[1];
					ptr->hour=hour = Gtime.Mid(0, 2);
					minute = Gtime.Mid(2, 2);
					second = Gtime.Mid(4, 2);
					date = parts[9];
					SYSTEMTIME sysTime;
					::GetLocalTime(&sysTime);
					CTime curTime(sysTime);
					CString timestamp;
					timestamp.Format(L"%lld\t",curTime.GetTime());

					remain_str = hour + ":" + minute + ":" + second + "(UTC)\t" +timestamp +parts[3] + parts[4] + "\t" + parts[5] + parts[6] + L"\r\n";
					break;
				}
			}
			strData += remain_str;
			ptr->OUTPUT_AREA.SetWindowText(strData);
			ptr->dir = path + "\\" + date.Right(2) + date.Mid(2, 2) + date.Left(2) + hour;
			if ("00" == minute && "00" == second) {
				ptr->recording=FALSE;
				WaitForSingleObject(ptr->Camera_Thread,100);
				ptr->recording = TRUE;
				ptr->Camera_Thread=AfxBeginThread(ptr->Camera,ptr);
			}
			else if(!ptr->recording){
				ptr->recording = TRUE;
				ptr->Camera_Thread = AfxBeginThread(ptr->Camera, ptr);
			}
			CFile file;
			if (!PathIsDirectory(ptr->dir)) {
				CreateDirectory(ptr->dir, 0);
			}
			file.Open(ptr->dir + "\\" + hour + ".txt",CFile::typeUnicode|CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate);
			file.SeekToEnd();
			DWORD dwFileLen = file.GetLength();
			if (0 == dwFileLen)
			{
				const unsigned char LeadBytes[] = { 0xEF, 0xBB, 0xBF };
				file.Write(LeadBytes, sizeof(LeadBytes));
			}
			int nSrcLen = (int)wcslen(remain_str);//content为要写入的文本
			CStringA utf8String(remain_str);
			int nBufLen = (nSrcLen + 1) * 6;
			LPSTR buffer = utf8String.GetBufferSetLength(nBufLen);
			int nLen = AtlUnicodeToUTF8(remain_str, nSrcLen, buffer, nBufLen);
			//上面的函数AtlUnicodeToUTF8()需头文件：<atlenc.h>
			//功能：将unicode转换成utf-8
			buffer[nLen] = 0;
			utf8String.ReleaseBuffer();
			file.SeekToEnd();
			file.Write(utf8String.GetBuffer(), nLen);
			file.Close();
		}
	}
	ptr->recording = FALSE;
	WaitForSingleObject(ptr->Camera_Thread, 100);
	return 0;
}

void CSerialParser2Dlg::OnBnClickedRecord()
{
	CString Cur_Sel_COM;
	try{
		COM_SELECTOR.GetLBText(COM_SELECTOR.GetCurSel(), Cur_Sel_COM);
	}
	catch (CException* E) {
		MessageBox(TEXT("串口不存在或被占用"), TEXT("打开串口失败"), MB_OK);
		return;
	}
	CString path;
	GetDlgItem(Dir_Edit)->GetWindowText(path);
	if (path.IsEmpty() || !PathFileExists(path)) {
		MessageBox(TEXT("路径不存在"), TEXT("路径不存在"), MB_OK);
		return;
	}
	if (FALSE == reading) {
		Handle_COM =CreateFile(Cur_Sel_COM,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			NULL,
			NULL
		);
		if (INVALID_HANDLE_VALUE==Handle_COM) {
			MessageBox(TEXT("串口不存在或被占用"),TEXT("打开串口失败"),MB_OK);
			return;
		}
		reading = TRUE;
		RECORD_BUTTON.SetWindowText(L"停止录制");
		COM_SELECTOR.EnableWindow(FALSE);
		BAUDRATE_SELECTOR.EnableWindow(FALSE);
		STOP_SELECTOR.EnableWindow(FALSE);
		PARITY_SELECTOR.EnableWindow(FALSE);
		DATA_SELECTOR.EnableWindow(FALSE);
		DIE_EDIT.EnableWindow(FALSE);
		DIE_BUTTON.EnableWindow(FALSE);
		SetupComm(Handle_COM, 4096, 4096);
		SetCommTimeouts(Handle_COM, &Time_Outs);
		CString fkms;
		BAUDRATE_SELECTOR.GetLBText(BAUDRATE_SELECTOR.GetCurSel(), fkms);
		dcb.BaudRate = _ttoi(fkms);
		STOP_SELECTOR.GetLBText(STOP_SELECTOR.GetCurSel(), fkms);
		if ("1" == fkms) {
			dcb.StopBits = ONESTOPBIT;
		}
		else if ("1.5" == fkms) {
			dcb.StopBits = ONE5STOPBITS;
		}
		else if ("2" == fkms) {
			dcb.StopBits = TWOSTOPBITS;
		}
		DATA_SELECTOR.GetLBText(DATA_SELECTOR.GetCurSel(), fkms);
		dcb.ByteSize = _ttoi(fkms);
		PARITY_SELECTOR.GetLBText(PARITY_SELECTOR.GetCurSel(), fkms);
		if ("None" == fkms) {
			dcb.Parity = NOPARITY;
		}
		else if ("Odd" == fkms) {
			dcb.Parity = ODDPARITY;
		}
		else if ("Even" == fkms) {
			dcb.Parity = EVENPARITY;
		}
		SetCommState(Handle_COM, &dcb);
		Read_Thread=AfxBeginThread(Read_Data,this);
		/*cap=cv::VideoCapture(0);*/
	}
	else {
		RECORD_BUTTON.SetWindowText(L"开始录制");
		//cap.release();
		reading = FALSE;
		WaitForSingleObject(Read_Thread, 500);
		//recording = FALSE;
		//WaitForSingleObject(Camera_Thread, 100);
		SetCommMask(Handle_COM, 0);
		EscapeCommFunction(Handle_COM, CLRDTR);
		PurgeComm(Handle_COM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(Handle_COM);
		COM_SELECTOR.EnableWindow(TRUE);
		BAUDRATE_SELECTOR.EnableWindow(TRUE);
		STOP_SELECTOR.EnableWindow(TRUE);
		PARITY_SELECTOR.EnableWindow(TRUE);
		DATA_SELECTOR.EnableWindow(TRUE);
		DIE_EDIT.EnableWindow(TRUE);
		DIE_BUTTON.EnableWindow(TRUE);
	}
}


void CSerialParser2Dlg::OnCbnSelchangeSelector()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSerialParser2Dlg::OnBnClickedDirButton()
{
	CString dir;
	CFolderPickerDialog folder_dialog(NULL, 0,this,0);
	if (IDOK == folder_dialog.DoModal()) {
		dir = folder_dialog.GetPathName();
		GetDlgItem(Dir_Edit)->SetWindowText(dir);
	}
}
