#pragma once
#ifndef TaskTray_HEADER_
#define TaskTray_HEADER_
#include <afxwin.h>

/*
コンストラクタもしくはInitializeで初期化をすること。
引数の無いコンストラクタでは初期化を行いません。

初期化を完了したらCreate関数でタスクバー通知領域に追加されます。
Desroy関数でタスクバー通知領域から削除されます。

デストラクタでリソースは自動解放されます。
*/
class TaskTray {
    HWND m_receiver;
    HWND m_hiddenWindow;
    HICON m_hIcon;
    UINT m_id;
    UINT m_callBackMsg;
    bool m_isAlive;
public:
    TaskTray();
    TaskTray(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBaclMsg, HICON hIcon);
    ~TaskTray();

    BOOL Initialize(HINSTANCE hInstance, HWND hwnd, UINT nID, UINT callBaclMsg, HICON hIcon);
    BOOL SetForegroundDummyWindow();
    BOOL Create();
    BOOL Destroy();
};

#endif
