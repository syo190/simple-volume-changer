#include "pch.h"
#include "CSettingDialog.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(CSettingDialog, CDialogEx)
END_MESSAGE_MAP()


void CSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < Data::ArraySize; ++i) {
		DDX_Control(pDX, IDC_EDIT1 + i, m_upEditBox[i]);
	}
	for (int i = 0; i < Data::ArraySize; ++i) {
		DDX_Control(pDX, IDC_EDIT1 + Data::ArraySize + i, m_downEditBox[i]);
	}
}


void CSettingDialog::SetShortcutKeyData(const Data::ShortucutKeyData& data)
{
	for (int i = 0; i < Data::ArraySize; i++)
	{
		m_upEditBox[i].SetVKey(data.upVkArray[i]);
	}
	for (int i = 0; i < Data::ArraySize; i++)
	{
		m_downEditBox[i].SetVKey(data.downVkArray[i]);
	}
}

Data::ShortucutKeyData CSettingDialog::GetShortcutKeyData() const
{
	Data::ShortucutKeyData result;
	for (int i = 0; i < Data::ArraySize; i++)
	{
		result.upVkArray[i] = m_upEditBox[i].GetVKey();
	}
	for (int i = 0; i < Data::ArraySize; i++)
	{
		result.downVkArray[i] = m_downEditBox[i].GetVKey();
	}

	return result;
}
