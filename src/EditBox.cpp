#include"..\inc\EditBox.h"
#include<atlbase.h>
#include<map>

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
    WNDPROC defProc;
    std::map<HWND, int*> mp;
    bool IsAccepted(int vk_input, LPCTSTR& transformedInput){
        for(std::size_t i = 0; i < std::size(detectedKeyList); ++i){
            if(vk_input == detectedKeyList[i]){
                transformedInput = detectedKeyNameList[i];
                return true;
            }
        }
        return false;
    }

    LRESULT CALLBACK EditBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        switch(uMsg){
            case WM_SYSKEYUP:
            case WM_KEYUP:{
                if(wParam == VK_BACK){
                    // バックスペースは全消し
                    SetWindowText(hwnd, _T(""));
                }else{
                    LPCTSTR transformedInput;
                    if(IsAccepted(wParam, transformedInput)){
                        SetWindowText(hwnd, transformedInput);
                    }
                }
                return 0;
            }
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            case WM_CHAR:{
                return 0;
            }
            default:{
                break;
            }
        }
        return CallWindowProc(defProc, hwnd, uMsg, wParam, lParam);
    }
}

EditBox::EditBox(){
    this->m_hWndParent = NULL;
    this->m_hwnd = NULL;
    this->m_hInstance = NULL;
}

EditBox::EditBox(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID):
m_hWndParent(hWndParent), m_x(x), m_y(y), m_width(width), m_height(height), m_ctrlID(ctrlID){
    this->Initialize(hWndParent, hInstance, x, y, width, height, ctrlID);
}

void EditBox::Initialize(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID){
    this->m_hWndParent = hWndParent;
    this->m_hInstance = hInstance;
    this->m_x = x;
    this->m_y = y;
    this->m_width = width;
    this->m_height = height;
    this->m_ctrlID = ctrlID;
    this->m_hwnd = NULL;
}

void EditBox::Create(){
    this->m_hwnd = CreateWindow(
        _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        this->m_x, this->m_y, this->m_width, this->m_height, this->m_hWndParent, NULL, this->m_hInstance, NULL
    );

    defProc = WNDPROC(GetWindowLongPtr(
        this->m_hwnd, GWL_WNDPROC
    ));
    SetWindowLongPtr(this->m_hwnd, GWL_WNDPROC, LONG(EditBoxProc));
}

bool EditBox::GetVK(int& vk){
    TCHAR buf[256] = {0};
    GetWindowText(this->m_hwnd, buf, 256);
    for(std::size_t i = 0; i < std::size(detectedKeyList); ++i){
        if(wcscmp(buf, detectedKeyNameList[i]) == 0){
            vk = detectedKeyList[i];
            return true;
        }
    }

    return false;
}