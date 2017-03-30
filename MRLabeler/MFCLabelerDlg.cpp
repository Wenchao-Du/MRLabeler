
// MFCLabelerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCLabeler.h"
#include "MFCLabelerDlg.h"
#include "afxdialogex.h"
#include "mrdir.h"
#include "mrutil.h"
#include "mrcvutil.h"
#include "mropencv.h"

#include <sstream>
using namespace  std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMFCLabelerDlg �Ի���

CMFCLabelerDlg::CMFCLabelerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCLabelerDlg::IDD, pParent)
	, m_imgindex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCLabelerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_imgindex);
	DDX_Control(pDX, IDC_COMBO_CLASSNAMES, m_comboxclassnames);
}

BEGIN_MESSAGE_MAP(CMFCLabelerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CMFCLabelerDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PREV, &CMFCLabelerDlg::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CMFCLabelerDlg::OnBnClickedButtonNext)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON_ADDRECTANGLE, &CMFCLabelerDlg::OnBnClickedButtonAddrectangle)
	ON_WM_SETCURSOR()
	ON_CBN_SELCHANGE(IDC_COMBO_CLASSNAMES, &CMFCLabelerDlg::OnCbnSelchangeComboClassnames)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CMFCLabelerDlg ��Ϣ��������

BOOL CMFCLabelerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	GetDlgItem(IDC_PIC)->GetWindowRect(&m_Picrect);
	ScreenToClient(&m_Picrect);
	// TODO:  �ڴ����Ӷ���ĳ�ʼ������
	for (int i = 0; i < voc.classes.size(); i++)
	{
		auto str = ANSIToUnicode(voc.classes[i].c_str());
		m_comboxclassnames.AddString(str);
	}
	m_comboxclassnames.SetCurSel(0);
	m_currentselectedclassname = ANSIToUnicode(voc.classes[0].c_str());
	m_imgdir=voc.rootdir + "/" + voc.imagedir;
	getAllFilesinDir(m_imgdir, m_files);
	ShowImageOfIndex();
	return TRUE;
}

void CMFCLabelerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCLabelerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (!m_img.empty())
		{
			DrawMat2Wnd(GetDlgItem(IDC_PIC), m_showimg);
		}
		UpdateTrackers();
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCLabelerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCLabelerDlg::UpdateTrackers()
{
	CPaintDC dc(this);
	for (int i = 0; i < m_trackers.size(); i++)
	{
		m_trackers[i].Draw(&dc);
	}
}

void CMFCLabelerDlg::ShowImageOfIndex()
{
	if (m_files.size() > 0)
	{
		string filepath = m_imgdir + "/" + m_files[m_imgindex];
		SetWindowText(ANSIToUnicode(m_files[m_imgindex].c_str()));
		m_img = cv::imread(filepath);
		if (m_img.data)
		{
			m_relrects.clear();
			m_trackers.clear();
			string annotationfilepath = voc.rootdir + "/" + voc.annotationdir + "/" + m_files[m_imgindex].substr(0, m_files[m_imgindex].length() - 4) + ".xml";
			LoadAnnotationFile(annotationfilepath);
			cv::resize(m_img, m_showimg, cv::Size(m_Picrect.Width(), m_Picrect.Height()));
			UpdateView();
			UpdateData(FALSE);
		}
	}
}

void CMFCLabelerDlg::OnBnClickedButtonOpen()
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("��ѡ��Ҫ��ǵ��ļ���");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if (lp && SHGetPathFromIDList(lp, szPath))
	{
		m_imgdir = UnicodeToANSI(szPath);
		getAllFilesinDir(m_imgdir, m_files,"*.jpg");
		m_imgindex = 0;
		ShowImageOfIndex();
	}
}

void CMFCLabelerDlg::OnBnClickedButtonPrev()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	if (m_files.size() > 0)
	{
		m_imgindex = (m_imgindex - 1 + m_files.size()) % m_files.size();
		ShowImageOfIndex();
	}
}

void CMFCLabelerDlg::OnBnClickedButtonNext()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	if (m_files.size() > 0)
	{
		string xmlannotationpath = "anno.xml";
		SaveAnnotationFile(xmlannotationpath);
		m_imgindex = (m_imgindex + 1) % m_files.size();
		ShowImageOfIndex();
	}
}

void CMFCLabelerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	m_ptRoomCenter.x = point.x*1.0 / m_Picrect.Width();
	m_ptRoomCenter.y = point.y*1.0 / m_Picrect.Height();

	if (!m_bDragMode)
	{
		CColorRectTracker tracker;
		tracker.m_strName = m_currentselectedclassname;
		tracker.m_rect.SetRect(0, 0, 0, 0);
		tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeInside;
		tracker.TrackRubberBand(this, point, TRUE);
		tracker.m_rect.NormalizeRect();
		CClientDC dc(this);
		tracker.Draw(&dc);
		m_trackers.push_back(tracker);
		CRelativeRect rr;
		rr.strName = tracker.m_strName;
		rr.ptStart.x = tracker.m_rect.left*1.0 / m_Picrect.Width();
		rr.ptStart.y = tracker.m_rect.top*1.0 / m_Picrect.Height();
		rr.ptEnd.x = tracker.m_rect.right*1.0 / m_Picrect.Width();
		rr.ptEnd.y = tracker.m_rect.bottom*1.0 / m_Picrect.Height();
		m_relrects.push_back(rr);
		m_bDragMode = true;
	}
	else
	{
		for (int i = m_trackers.size() - 1; i >= 0; i--)
		{
			int nRetCode = m_trackers[i].HitTest(point);
			if (nRetCode >= 0)
			{
				CColorRectTracker tracker = m_trackers[i];
				tracker.Track(this, point, TRUE);
				m_trackers.erase(m_trackers.begin() + i);
				m_trackers.push_back(tracker);
				CRelativeRect rr;
				m_relrects.erase(m_relrects.begin() + i);
				rr.strName = tracker.m_strName;
				rr.ptStart.x = tracker.m_rect.left*1.0 / m_Picrect.Width();
				rr.ptStart.y = tracker.m_rect.top*1.0 / m_Picrect.Height();
				rr.ptEnd.x = tracker.m_rect.right*1.0 / m_Picrect.Width();
				rr.ptEnd.y = tracker.m_rect.bottom*1.0 / m_Picrect.Height();
				m_relrects.push_back(rr);
				UpdateView();
				return;
			}
		}
	}
// 	m_bDrawing = true;
// 	m_ptRelativeBegin = cv::Point2f(point.x*1.0 / m_Picrect.Width(), point.y *1.0 / m_Picrect.Height());
// 	m_ptRelativeEnd = m_ptRelativeBegin;
//	string pos = "X,Y:" + int2string((int)(m_ptRelativeEnd.x*m_Picrect.Width())) + "," + int2string((int)(m_ptRelativeEnd.y*m_Picrect.Height()));
//	GetDlgItem(IDC_POS)->SetWindowText(ANSIToUnicode(pos).c_str());
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMFCLabelerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (PtInRect(m_Picrect, point))
	{
		CString str;
		str.Format(_T("X,Y:%d,%d"), point.x, point.y);
		GetDlgItem(IDC_POS)->SetWindowText(str);
	}
// 	if (m_bDrawing)
// 	{
// 		m_ptRelativeEnd = cv::Point2f(point.x*1.0 / m_Picrect.Width(), point.y *1.0 / m_Picrect.Height());
// //		string pos = "X,Y:" + int2string((int)(m_ptRelativeEnd.x*m_Picrect.Width())) + "," + int2string((int)(m_ptRelativeEnd.y*m_Picrect.Height()));
// //		GetDlgItem(IDC_POS)->SetWindowText(ANSIToUnicode(pos).c_str());
// //		Invalidate(FALSE);
// 	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CMFCLabelerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
//	m_bDrawing = false;
//	m_ptRelativeEnd = cv::Point2f(point.x *1.0 / m_Picrect.Width(), point.y*1.0 / m_Picrect.Height());
//	string pos = "X,Y:" + int2string((int)(m_ptRelativeEnd.x*m_Picrect.Width())) + "," + int2string((int)(m_ptRelativeEnd.y*m_Picrect.Height()));
//	GetDlgItem(IDC_POS)->SetWindowText(ANSIToUnicode(pos).c_str());
//	Invalidate(FALSE);
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCLabelerDlg::ResizeBottomButton(int nID)
{
	CRect rect;
	GetDlgItem(nID)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top = dlg_rect.bottom -35;
	rect.bottom = dlg_rect.bottom-5;
	GetDlgItem(nID)->MoveWindow(rect);
}
void CMFCLabelerDlg::ResizeBottomStatic(int nID)
{
	CRect rect;
	GetDlgItem(nID)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top = dlg_rect.bottom - 30;
	rect.bottom = dlg_rect.bottom-10;
	GetDlgItem(nID)->MoveWindow(rect);
}
void CMFCLabelerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (GetDlgItem(IDC_PIC) == NULL)return;
 	GetClientRect(&dlg_rect);
	ResizeBottomButton(IDC_BUTTON_OPEN);
	ResizeBottomButton(IDC_BUTTON_ADDRECTANGLE);
	ResizeBottomStatic(IDC_COMBO_CLASSNAMES);
	ResizeBottomStatic(IDC_EDIT1);
	ResizeBottomButton(IDC_BUTTON_PREV);
	ResizeBottomButton(IDC_BUTTON_NEXT);
	ResizeBottomStatic(IDC_POS);

	GetDlgItem(IDC_PIC)->GetWindowRect(&m_Picrect);
	ScreenToClient(&m_Picrect);
	m_Picrect.right = dlg_rect.right;
	m_Picrect.bottom = dlg_rect.bottom-60;
 	GetDlgItem(IDC_PIC)->MoveWindow(m_Picrect);
	if (m_img.data)
	{
		resize(m_img, m_showimg, cv::Size(m_Picrect.right - m_Picrect.left, m_Picrect.bottom - m_Picrect.top));
	}
	UpdateView();
}


void CMFCLabelerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	lpMMI->ptMinTrackSize.x = 640;
	lpMMI->ptMinTrackSize.y = 480;
	lpMMI->ptMaxTrackSize.x = 1600;
	lpMMI->ptMaxTrackSize.y = 900;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

BOOL CMFCLabelerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ�����ר�ô����/����û���
	if (pMsg->message == WM_KEYUP&&pMsg->wParam == VK_DELETE)
	{
		if (m_trackers.size() > 0)
		{
			m_trackers.pop_back();
			m_relrects.pop_back();
			UpdateView();
		}
		if (m_trackers.size() == 0)
			m_bDragMode = false;
		return 0;
	}
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT1))
		{
			CString str;
			GetDlgItem(IDC_EDIT1)->GetWindowText(str);
			m_imgindex = _ttoi(str);
			ShowImageOfIndex();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMFCLabelerDlg::UpdateView()
{
	ConvertRel2Tracker();
	if (m_img.data)
		Invalidate(FALSE);
	else
		Invalidate();
}

void CMFCLabelerDlg::OnBnClickedButtonAddrectangle()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	m_bDragMode = false;
}


BOOL CMFCLabelerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	if ((pWnd == this))
	{
		for (int i = 0; i < m_trackers.size(); i++)
		{
			if (m_trackers[i].SetCursor(this, nHitTest))
				return TRUE;
		}
	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CMFCLabelerDlg::OnCbnSelchangeComboClassnames()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	m_comboxclassnames.GetLBText(m_comboxclassnames.GetCurSel(), m_currentselectedclassname);
}

void CMFCLabelerDlg::ConvertRel2Tracker()
{
	m_trackers.clear();
	for (int i = 0; i < m_relrects.size(); i++)
	{
		CRelativeRect rr=m_relrects[i];
		CColorRectTracker tracker;
		tracker.m_strName = rr.strName;
		tracker.m_rect.SetRect(rr.ptStart.x*m_Picrect.Width(), rr.ptStart.y*m_Picrect.Height(), rr.ptEnd.x*m_Picrect.Width(), rr.ptEnd.y*m_Picrect.Height());
		tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeInside;
		tracker.m_rect.NormalizeRect();
		m_trackers.push_back(tracker);
	}
}

bool CMFCLabelerDlg::SaveAnnotationFile(const string xmlannopath)
{
	if (m_img.data)
	{
		AnnotationFile af;
		af.set_width(m_img.cols);
		af.set_height(m_img.rows);
		af.set_depth(m_img.channels());
		af.set_filename(m_files[m_imgindex]);
		vector<Object>objects;
		for (int i = 0; i < m_relrects.size(); i++)
		{
			CRelativeRect rr = m_relrects[i];
			Object object;
			object.difficult = 0;
			object.truncated = 0;
			object.name = UnicodeToANSI(rr.strName);
			object.xmin = rr.ptStart.x*m_img.cols;
			object.ymin = rr.ptStart.y*m_img.rows;
			object.xmax = rr.ptEnd.x*m_img.cols;
			object.ymax = rr.ptEnd.y*m_img.rows;
			objects.push_back(object);
		}
		af.set_objects(objects);
		af.save_xml(xmlannopath);
		return true;
	}
	return false;
}

void CMFCLabelerDlg::LoadAnnotationFile(const string xmlannopath)
{
	AnnotationFile af;
	if (af.load_file(xmlannopath))
	{
		for (int i = 0; i < af.objects.size(); i++)
		{
			auto object = af.objects[i];
			CRelativeRect rr;
			rr.strName =object.name.c_str();
			rr.ptStart.x = object.xmin *1.0 / af.width;
			rr.ptStart.y = object.ymin *1.0 / af.height;
			rr.ptEnd.x = object.xmax *1.0 / af.width;
			rr.ptEnd.y = object.ymax *1.0 / af.height;
			m_relrects.push_back(rr);
			m_bDragMode = true;
		}
	}
}

BOOL CMFCLabelerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	static float upLimit = 4.0f;
	static float downLinit = 0.25f;
	static float fUp = 1.1f;
	if (zDelta > 0 && m_zoomratio <= upLimit)
	{
		m_zoomratio *= fUp;
	}
	else if (zDelta < 0 && m_zoomratio >= downLinit)
	{
		m_zoomratio /= fUp;
	}
	CString str;
	str.Format(_T("zoom:%f"), m_zoomratio);
	GetDlgItem(IDC_POS)->SetWindowText(str);
	UpdateView();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}