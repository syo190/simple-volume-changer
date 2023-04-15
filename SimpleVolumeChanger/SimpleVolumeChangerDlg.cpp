
// SimpleVolumeChangerDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "SimpleVolumeChanger.h"
#include "SimpleVolumeChangerDlg.h"
#include "afxdialogex.h"
#include <cmath>
#include "CSettingDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
	// 定数リスト
	const int AcceptInputInterval = 100;
	// UI
	const int BlankSpace = 40;
	// スライダー
	const int SliderWidth = 300;
	const int SliderHeight = 20;
	const int SliderMinVal = 0;
	const int SliderMaxVal = 100;
	// ラベル
	const int LabelHeight = 20;
	const int LabelWidth = 300;
	COLORREF SliderBkColor = RGB(200, 200, 200);
	// データ保存先
	const char ConfigFilePath[] = "./shorcutkey.bin";
}


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// CSimpleVolumeChangerDlg ダイアログ



CSimpleVolumeChangerDlg::CSimpleVolumeChangerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEVOLUMECHANGER_DIALOG, pParent),
	m_exDefaultStyle(NULL), m_isDlgAlive(false), m_pMenu(nullptr), m_sliderIndex(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleVolumeChangerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSimpleVolumeChangerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_COMMAND(IDD_MENU_SETTING, OpenSettingDialog)
	ON_MESSAGE(WM_TASK_TRAY_CLICKED, OnTaskTrayClicked)
END_MESSAGE_MAP()


// CSimpleVolumeChangerDlg メッセージ ハンドラー

BOOL CSimpleVolumeChangerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	CRect rc;

	// コントロールの生成
	int channelCnt = m_soundControler.ChannelCount();
	for (int i = 0; i < channelCnt; ++i)
	{
		// スライダーの作成
		rc.left = BlankSpace;
		rc.top = BlankSpace * (i + 1) + SliderHeight * i;
		rc.right = rc.left + SliderWidth;
		rc.bottom = rc.top + SliderHeight;
		m_volumeSliders.emplace_back(new CSliderCtrl);
		m_volumeSliders[i]->Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SLIDER + i);
		m_volumeSliders[i]->SetRange(SliderMinVal, SliderMaxVal, TRUE);
		float normalizedVolume = m_soundControler.GetChannelNormalizedVolume(i);
		int volume = static_cast<int>(std::round(normalizedVolume * (SliderMaxVal - SliderMinVal)));
		m_volumeSliders[i]->SetPos(volume);

		// ラベルの作成
		m_label.emplace_back(new CStatic);
		rc.left = BlankSpace ;
		rc.top = rc.top - (BlankSpace * 2) / 3;
		rc.right = rc.left + LabelWidth;
		rc.bottom = rc.top + LabelHeight;
		m_label[i]->Create(m_soundControler.DeviceName(i), WS_CHILD | WS_VISIBLE | SS_LEFT, rc, this);
	}

	// ウィンドウスタイルの変更（ダイアログではPreCreateWindowが呼ばれないため）
	ModifyStyle(WS_THICKFRAME, WS_MINIMIZEBOX, NULL);
	SetWindowPos(NULL, NULL, NULL, SliderWidth + 2 * BlankSpace, rc.bottom + 2 * BlankSpace + SliderHeight, SWP_NOMOVE);
	m_exDefaultStyle = GetExStyle();

	// その他
	m_sliderBkBrush.CreateSolidBrush(SliderBkColor);
	SetTimer(IDT_ACCEPT_INPUT, AcceptInputInterval, NULL);
	SetWindowText(_T("Simple Volume Changer"));
	m_keyData.ReadShortcutKey(ConfigFilePath);
	m_isDlgAlive = false;
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_tasktray.Initialize(
		hInstance, GetSafeHwnd(), IDI_TASK_TRAY, WM_TASK_TRAY_CLICKED, m_hIcon
	);

	// メニュー設定
	m_pMenu = new CMenu();
	m_pMenu->CreateMenu();
	m_pMenu->AppendMenu(MF_ENABLED | MF_STRING, IDD_MENU_SETTING, _T("設定"));
	SetMenu(m_pMenu);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CSimpleVolumeChangerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_MINIMIZE)
	{
		m_tasktray.Create();
		auto exStyle = GetExStyle();
		exStyle = (exStyle & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW;
		ModifyStyleEx(NULL, exStyle);
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CSimpleVolumeChangerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSimpleVolumeChangerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSimpleVolumeChangerDlg::OnDestroy()
{
	KillTimer(IDT_ACCEPT_INPUT);
	for (std::size_t i = 0, size = m_volumeSliders.size(); i < size; ++i)
	{
		if (m_volumeSliders[i])
		{
			delete m_volumeSliders[i];
		}
		if (m_label[i])
		{
			delete m_label[i];
		}
	}
	if (m_pMenu)
	{
		delete m_pMenu;
	}
}

HBRUSH CSimpleVolumeChangerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctlId = pWnd->GetDlgCtrlID();
	
	// スライダーの色変更
	for (std::size_t i = 0, size = m_volumeSliders.size(); i < size; ++i)
	{
		if (m_label[i] && m_volumeSliders[i]->GetDlgCtrlID() == ctlId)
		{
			return m_sliderBkBrush;
		}
	}
	return hbr;
}

/// <summary>
/// キー入力を探索しスライダーおよび音量を変化させる
/// </summary>
/// <param name="nIDEvent"></param>
void CSimpleVolumeChangerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != IDT_ACCEPT_INPUT || m_isDlgAlive)
	{
		return;
	}
	// 現在のチャンネル取得
	m_sliderIndex = m_soundControler.GetCurrentCannek();

	// キーが押されているかどうかの調査
	int upDown = 0;
	int sum = 0;
	bool changeFlag = true;
	for(std::size_t i = 0; i < std::size(m_keyData.upVkArray); ++i){
		sum += m_keyData.upVkArray[i];
		if(m_keyData.upVkArray[i] == NULL) continue;
		if(!GetAsyncKeyState(m_keyData.upVkArray[i])){
			changeFlag = false;
			break;
		}
	}
	if(sum != 0 && changeFlag) upDown = 1;

	sum = 0;
	changeFlag = true;

	for(std::size_t i = 0; i < std::size(m_keyData.downVkArray); ++i){
		sum += m_keyData.downVkArray[i];
		if(m_keyData.downVkArray[i] == NULL) continue;
		if(!GetAsyncKeyState(m_keyData.downVkArray[i])){
			changeFlag = false;
			break;
		}
	}
	if(sum != 0 && changeFlag) upDown = -1;
	
	// キーが押されていた場合はそれをスライダーおよび音量に反映する
	float normalizedNowVolume = m_soundControler.GetChannelNormalizedVolume(m_sliderIndex);
	int minVol, maxVol;
	m_volumeSliders[m_sliderIndex]->GetRange(minVol, maxVol);
	int nowVolume = static_cast<int>(std::round(normalizedNowVolume * (maxVol - minVol)));
	int newVolume = nowVolume;
	if(nowVolume + upDown >= minVol && nowVolume + upDown <= maxVol) newVolume += upDown;
	m_volumeSliders[m_sliderIndex]->SetPos(newVolume);
	UpdateVolume(m_sliderIndex);
}

/// <summary>
/// キー以外でスライダーを更新した際に音量を処理する
/// </summary>
/// <param name="nSBCode"></param>
/// <param name="nPos"></param>
/// <param name="pScrollBar"></param>
void CSimpleVolumeChangerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(!pScrollBar)
	{
		return;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	int sliderId = pScrollBar->GetDlgCtrlID();
	for (std::size_t i = 0, size = m_volumeSliders.size(); i < size; ++i)
	{
		if (m_volumeSliders[i] && m_volumeSliders[i]->GetDlgCtrlID() == sliderId)
		{
			UpdateVolume(i);
		}
	}
}

/// <summary>
/// 設定ダイアログを開く
/// </summary>
void CSimpleVolumeChangerDlg::OpenSettingDialog()
{
	CSettingDialog dialog(IDD_DIALOG_SETTING, this);
	dialog.SetShortcutKeyData(m_keyData);
	m_isDlgAlive = true;
	auto result = dialog.DoModal();
	if (result == IDOK) {
		m_keyData = dialog.GetShortcutKeyData();
		m_keyData.WriteShortcutKey(ConfigFilePath);
	}
	m_isDlgAlive = false;
}

/// <summary>
/// タスクトレーからのイベントハンドラ
/// </summary>
LRESULT CSimpleVolumeChangerDlg::OnTaskTrayClicked(WPARAM wParam, LPARAM lParam)
{
	auto id = (UINT)(lParam);
	if (id == WM_RBUTTONUP) {
		POINT pt = { 0 };
		GetCursorPos(&pt);

		m_tasktray.SetForegroundDummyWindow();
		HMENU hMenu = CreatePopupMenu();
		InsertMenu(
			hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_OPEN_FROM_TASK_TRAY, _T("VoiceChanger")
		);
		InsertMenu(
			hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CLOSE_FROM_TASK_TRAY, _T("close")
		);
		auto clikedID = TrackPopupMenu(
			hMenu,
			TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,
			pt.x, pt.y, 0,
			GetSafeHwnd(), NULL
		);
		DestroyMenu(hMenu);

		if (clikedID == IDM_CLOSE_FROM_TASK_TRAY) {
			SendMessage(WM_CLOSE, WPARAM(NULL), LPARAM(NULL));
		}
		else if (clikedID == IDM_OPEN_FROM_TASK_TRAY) {
			// タスクバー通知領域 -> タスクバーへ移動
			SetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE, m_exDefaultStyle);
			ShowWindow(SW_SHOW);
			m_tasktray.Destroy();
		}
	}
	return 0;
}

/// <summary>
/// スライダーの現在位置を取得しボリュームを更新する
/// </summary>
/// <param name="idx">対象のスライダー番号</param>
void CSimpleVolumeChangerDlg::UpdateVolume(std::size_t idx)
{
	int minVol, maxVol;
	m_volumeSliders[idx]->GetRange(minVol, maxVol);
	int nowVol = m_volumeSliders[idx]->GetPos();
	float normalizedVolume = float(nowVol) / float(maxVol - minVol);
	m_soundControler.SetChannelVolume(normalizedVolume, idx);
}
