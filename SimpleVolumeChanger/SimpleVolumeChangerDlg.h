
// SimpleVolumeChangerDlg.h : ヘッダー ファイル
//

#pragma once
#include <vector>
#include "SoundControler.h"
#include "ShortcutKey.h"
#include "TaskTray.h"

// CSimpleVolumeChangerDlg ダイアログ
class CSimpleVolumeChangerDlg : public CDialogEx
{
// コンストラクション
public:
	CSimpleVolumeChangerDlg(CWnd* pParent = nullptr);	// 標準コンストラクター
// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEVOLUMECHANGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OpenSettingDialog();
	afx_msg LRESULT OnTaskTrayClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void UpdateVolume(std::size_t idx);

	SoundControler m_soundControler;
	std::vector<CSliderCtrl*> m_volumeSliders;
	std::vector<CStatic*> m_label;
	int m_sliderIndex;
	Data::ShortucutKeyData m_keyData;

	CBrush m_sliderBkBrush;
	CMenu* m_pMenu;
	bool m_isDlgAlive;
	TaskTray m_tasktray;
	LONG m_exDefaultStyle;
};
