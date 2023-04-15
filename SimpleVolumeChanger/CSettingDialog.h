#pragma once
#include <afxdialogex.h>
#include "EditBox.h"
#include "ShortcutKey.h"

class CSettingDialog : public CDialogEx {
	CShortcutEditBox m_upEditBox[Data::ArraySize];
	CShortcutEditBox m_downEditBox[Data::ArraySize];
	Data::ShortucutKeyData m_shortcutKeyData;
	void DoDataExchange(CDataExchange* pDX) override;
public:
	CSettingDialog(UINT nIDTemplate, CWnd* pParent = NULL): CDialogEx(nIDTemplate, pParent){}

	void SetShortcutKeyData(const Data::ShortucutKeyData& data);
	Data::ShortucutKeyData GetShortcutKeyData() const;
protected:
	DECLARE_MESSAGE_MAP()
};
