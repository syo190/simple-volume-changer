#include "pch.h"
#include "TaskTray.h"

namespace{
    LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        switch(uMsg){
            case WM_CLOSE:{
                PostQuitMessage(0);
                break;
            }
            case WM_DESTROY:{
                DestroyWindow(hwnd);
                break;
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

TaskTray::TaskTray(){
    this->m_receiver = NULL;
    this->m_hiddenWindow = NULL;
    this->m_callBackMsg = NULL;
    this->m_hIcon = NULL;
    this->m_id = NULL;
    this->m_isAlive = false;
}

TaskTray::TaskTray(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBacklMsg, HICON hIcon):
m_receiver(hwnd), m_hiddenWindow(NULL), m_id(nID), m_isAlive(false), m_callBackMsg(callBacklMsg){
    this->Initialize(hInstance, hwnd, nID, callBacklMsg, hIcon);
}

TaskTray::~TaskTray(){
    if(this->m_isAlive){
        this->Destroy();
    }

    DestroyIcon(this->m_hIcon);
    PostMessage(this->m_hiddenWindow, WM_DESTROY, WPARAM(NULL), LPARAM(NULL));
}

BOOL TaskTray::Initialize(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBackMsg, HICON hIcon){
    if(this->m_hiddenWindow != NULL) return FALSE;

    this->m_receiver = hwnd;
    this->m_id = nID;
    this->m_isAlive = false;
    this->m_callBackMsg = callBackMsg;
    this->m_hIcon = hIcon;

    CString className;
    className.Format(_T("hidden window [%d]"), this->m_id);

	WNDCLASS wndC = {};
	wndC.hInstance = hInstance;
	wndC.lpszClassName = className;
    wndC.lpfnWndProc = DummyWindowProc;

    if(RegisterClass(&wndC)){
        this->m_hiddenWindow = CreateWindowEx(
            0,
            className,
            NULL,
            NULL,
            0, 0, 0, 0,
            NULL,
            NULL,
            hInstance,
            NULL
        );
    }

    return this->m_hiddenWindow != NULL;
}

BOOL TaskTray::SetForegroundDummyWindow(){
    if(this->m_hiddenWindow == NULL) return FALSE;
    return SetForegroundWindow(this->m_hiddenWindow);
}

BOOL TaskTray::Create(){
    if(this->m_hiddenWindow == NULL) return FALSE;
    if(this->m_isAlive) return FALSE;

    NOTIFYICONDATA nd = {0};
    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = this->m_receiver;
    nd.uID = this->m_id;
    nd.uFlags = NIF_MESSAGE | NIF_ICON;
    nd.hIcon = this->m_hIcon;
    nd.uCallbackMessage = this->m_callBackMsg;

    this->m_isAlive = Shell_NotifyIcon(NIM_ADD, &nd);
    return this->m_isAlive && (this->m_hiddenWindow != NULL);
}

BOOL TaskTray::Destroy(){
    NOTIFYICONDATA nd = {0};
    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = this->m_receiver;
    nd.uID = this->m_id;
    if(Shell_NotifyIcon(NIM_DELETE, &nd)){
        this->m_isAlive = false;
    }
    return !this->m_isAlive;
}

