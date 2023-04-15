#include "pch.h"
#include "EditBox.h"

namespace{
	const int detectedKeyList[] = {
		VK_SHIFT, VK_CONTROL, VK_MENU, VK_PAUSE, VK_CAPITAL, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT,
		VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, // 0 ~ 9
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, // A ~ J
		0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, // K ~ T
		0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, // U ~ Z
		VK_LWIN, VK_RWIN, VK_APPS,
		VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, // numpad 0 ~ 9
		VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT,
		VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
		VK_NUMLOCK, VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU 
	};
	const LPCTSTR detectedKeyNameList[] = {
		_T("Shift"), _T("Control"), _T("Alt"), _T("Pause"), _T("CapsLock"), _T("Esc"), _T("Space"), _T("PageUp"), _T("PageDown"),
		_T("Left"), _T("Up"), _T("Right"), _T("Down"),
		_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
		_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"),
		_T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"),
		_T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z"),
		_T("LeftWin"), _T("RightWin"), _T("App"),
		_T("Pad0"), _T("Pad1"), _T("Pad2"), _T("Pad3"), _T("Pad4"), _T("Pad5"), _T("Pad6"), _T("Pad7"), _T("Pad8"), _T("Pad9"),
		_T("Multiply"), _T("Add"), _T("Divide"), _T("Subtract"),
		_T("F1"), _T("F2"), _T("F3"), _T("F4"), _T("F5"), _T("F6"), _T("F7"), _T("F8"), _T("F9"), _T("F10"), _T("F11"), _T("F12"),
		_T("NumLock"), _T("LeftShift"), _T("RightShift"), _T("LeftControl"), _T("RightControl"), _T("LeftAlt"), _T("RightAlt")   
	};
    bool IsAccepted(int vk_input, CString& transformedInput){
        for(std::size_t i = 0; i < std::size(detectedKeyList); ++i){
            if(vk_input == detectedKeyList[i]){
                transformedInput = detectedKeyNameList[i];
                return true;
            }
        }
        return false;
    }
}

BEGIN_MESSAGE_MAP(CShortcutEditBox, CEdit)
	ON_WM_SYSCOMMAND()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CShortcutEditBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
    if(nChar == VK_BACK){
        // バックスペースは全消し
		this->SetWindowText(_T(""));
		m_vKey = NULL;
    }else{
        CString transformedInput;
        if(IsAccepted(nChar, transformedInput)){
			m_vKey = nChar;
			this->SetWindowText(transformedInput);
        }
    }
}

void CShortcutEditBox::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_BACK) {
		// バックスペースは全消し
		this->SetWindowText(_T(""));
		m_vKey = NULL;
	}
	else {
		CString transformedInput;
		if (IsAccepted(nChar, transformedInput)) {
			m_vKey = nChar;
			this->SetWindowText(transformedInput);
		}
	}
}

void CShortcutEditBox::SetVKey(std::int32_t vKey)
{
	m_vKey = vKey;
}


void CShortcutEditBox::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();
	CString transformedInput;
	if (IsAccepted(m_vKey, transformedInput)) {
		this->SetWindowText(transformedInput);
	}
}