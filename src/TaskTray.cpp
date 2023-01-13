#include"..\inc\TaskTray.h"
#include<atlstr.h>

namespace{
    LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

TaskTray::TaskTray(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBacklMsg):
m_receiver(hwnd), m_hiddenWindow(NULL), m_id(nID), m_isAlive(false), m_callBackMsg(callBacklMsg){

    CString className;
    className.Format("hidden window [%d]", this->m_id);

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
}

TaskTray::~TaskTray(){
    if(this->m_isAlive){
        this->Destroy();
    }
}

BOOL TaskTray::Create(){
    NOTIFYICONDATA nd = {0};
    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = this->m_receiver;
    nd.uID = this->m_id;
    nd.uFlags = NIF_MESSAGE;
    nd.uCallbackMessage = this->m_callBackMsg;

    this->m_isAlive = Shell_NotifyIcon(NIM_ADD, &nd);
    return this->m_isAlive && (this->m_hiddenWindow != NULL);
}

BOOL TaskTray::Destroy(){
    NOTIFYICONDATA nd = {0};
    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = this->m_receiver;
    nd.uID = this->m_id;
    return Shell_NotifyIcon(NIM_DELETE, &nd);
}

