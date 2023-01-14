#include<windows.h>
#include<atlbase.h>

class TaskTray{
    HWND m_receiver;
    HWND m_hiddenWindow;
    UINT m_id;
    UINT m_callBackMsg;
    bool m_isAlive;
public:
    TaskTray();
    TaskTray(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBaclMsg);
    ~TaskTray();

    BOOL Initialize(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBaclMsg);
    BOOL SetForegroundDummyWindow();
    BOOL Create();
    BOOL Destroy();
};

