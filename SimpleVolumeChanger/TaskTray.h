#pragma once
#ifndef TaskTray_HEADER_
#define TaskTray_HEADER_
#include <afxwin.h>

/*
�R���X�g���N�^��������Initialize�ŏ����������邱�ƁB
�����̖����R���X�g���N�^�ł͏��������s���܂���B

������������������Create�֐��Ń^�X�N�o�[�ʒm�̈�ɒǉ�����܂��B
Desroy�֐��Ń^�X�N�o�[�ʒm�̈悩��폜����܂��B

�f�X�g���N�^�Ń��\�[�X�͎����������܂��B
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
